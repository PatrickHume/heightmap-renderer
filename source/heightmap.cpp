#include"../headers/heightmap.h"

// Based on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
Heightmap::Heightmap(const char *file)
{
    // Store filename and file directory.
    std::string fileStr = std::string(file);
    fileName       = fileStr.substr(fileStr.find_last_of('/')+1);
    fileDirectory  = fileStr.substr(0,fileStr.find_last_of('/')+1);
    // Get and display the maximum tessellation level.
    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tessellation level: " << maxTessLevel << std::endl;

    // Create shader for tessellating and viewing the mesh.
    drawShader = std::make_shared<Shader>(
        "resources/shaders/passthrough.vs", 
        "resources/shaders/texture.fs", 
        nullptr,
        "resources/shaders/controlShader.tcs", 
        "resources/shaders/evaluationShader.tes"); 

    // ---------------------------------- Loading Heightmap ----------------------------------
    // Load the heightmap image into a texture.
    heightmapTexture = std::make_shared<Texture>(file, heightmapTexUnit);
    // Get the width and height of the image.
    width = heightmapTexture->getWidth();
    height = heightmapTexture->getHeight();
    // Split the image into a rez*rez grid of vertices to use for our quads.
    for(unsigned int i = 0; i <= rez-1; i++){
        for(unsigned int j = 0; j <= rez-1; j++){
            vertices.push_back(-width/2.0f + width*i/(float)rez);       // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*j/(float)rez);     // v.z
            vertices.push_back(i / (float)rez);                         // u
            vertices.push_back(j / (float)rez);                         // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)rez);   // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*j/(float)rez);     // v.z
            vertices.push_back((i+1) / (float)rez);                     // u
            vertices.push_back(j / (float)rez);                         // v

            vertices.push_back(-width/2.0f + width*i/(float)rez);       // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
            vertices.push_back(i / (float)rez);                         // u
            vertices.push_back((j+1) / (float)rez);                     // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)rez);   // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
            vertices.push_back((i+1) / (float)rez);                     // u
            vertices.push_back((j+1) / (float)rez);                     // v
        }
    }
    // Create a VAO to hold the VBO.
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);
    // Create a VBO to hold the vertices.
    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    // Set a pointer to position attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set a pointer to texCoord attributes.
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    // Set the number of points per patch.
    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

    // ---------------------------------- Generating Buffers ----------------------------------
    // Create the buffers to draw the evaluated tessellation levels to.
    glGenRenderbuffers( 1, &renderbufId );
    glGenRenderbuffers( 1, &depthbufId );
    glGenFramebuffers ( 1, &framebufId   );
    // Bind the render buffer.
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, width, height);
    // Bind the depth buffer - depth testing is not used in the evaluation stage.
    // However, I have had no success if creating a "framebuffer complete" 
    // frame buffer without the depth buffer attached.
    glBindRenderbuffer(GL_RENDERBUFFER, depthbufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    // Set up the framebuffer to hold the renderbuffers.
    glBindFramebuffer(GL_FRAMEBUFFER, framebufId);  
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbufId);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, depthbufId  );

    // Check frame buffer for errors.
    GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);  
    if(stat != GL_FRAMEBUFFER_COMPLETE) { 
        throw std::runtime_error("Heightmap framebuffer is not framebuffer complete.");
    }
    // Set the colour attachment to draw buffer 1.
    const GLenum att[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, att);
    // Unbind the framebuffer to prevent any accidental drawing to it.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create an empty texture to pass the tessellation levels to the shader.
    tessLevelsTexture = std::make_shared<Texture>(rez, rez, 1, tessLevelsTexUnit);
    // Load the tessellation levels per patch.
    tessLevels = loadTessLevelImage();
    // Load the data into the texture.
    tessLevelsTexture->loadData(&tessLevels[0]);
}
Heightmap::~Heightmap()
{
}
// Loads a tessellation level image if it exists. If not, creates one and saves it to the cachedData folder.
std::array<unsigned char, Heightmap::rez*Heightmap::rez> Heightmap::loadTessLevelImage(){
    // Create an array to hold each minimum tessellation level per patch in the rez*rez grid.
    std::array<unsigned char, rez*rez> tessLevelsBuffer;
    // Generate a tessLevels filename based on the image filename.
    std::string fileNameNoExt   = fileName.substr(0,fileName.find_last_of('.'));
    std::string outputFile      = "resources/cachedData/"+fileNameNoExt+"TessLevels.png";

    // Attempt to load the file.
    std::cout << "Searching for file: " << outputFile << std::endl;
    int w, h, c;
    // We should make sure that stbi is not flipping the data.
    stbi_set_flip_vertically_on_load(false);
    unsigned char* bytes = stbi_load(outputFile.c_str(), &w, &h, &c, 0);
    if(bytes){
        // If the file is found, store the data in the buffer and return it.
        std::cout << "File found." << std::endl;
        for(int i = 0; i < tessLevelsBuffer.size(); i++){
            tessLevelsBuffer[i] = (unsigned char)bytes[i];
        }            
        // Delete the image data once we are done with it.
        stbi_image_free(bytes);
        // Return the tessellation levels.
        return tessLevelsBuffer;
    }
    // Otherwise, begin evaluating each tessellation level.
    std::cout << "Creating tessLevels image." << std::endl;

    // Create shader that will perform the tessellation level evaluation and write to the framebuffer.
    evaluationShader = std::make_shared<Shader>(
        "resources/shaders/passthrough.vs", 
        "resources/shaders/heightComparison.fs", 
        nullptr,
        "resources/shaders/controlSetup.tcs", 
        "resources/shaders/evaluationSetup.tes"); 

    // Create some buffers to store patch data.
    std::array<float, rez*rez>  tessLevelError;     // Stores the total pixel error of a patch.
    std::array<bool, rez*rez>   patchIsLocked;      // Records whether or not a patch has been locked.
    // Create some buffers to store screen data.
    std::vector<float>          screenFloatBuffer;  // Stores the value of each pixel in the generated heightmaps.
    // Resize the float buffer to fit the screen / heightmap dimensions.
    screenFloatBuffer.resize(width*height);
    // Initialize the tessellation levels buffer with 1s so we have
    // a value to increment from.
    tessLevelsBuffer.fill((unsigned char)1);
    patchIsLocked.fill(false);

    // Create an orthogonal projection to give the camera a flat view of the resulting mesh.
    // So it appears as if it is a 2D heightmap.
    glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -2.0f, 2.0f);

    // Bind to the auxiliary frame buffer so we aren't drawing to the screen
    // during tessellation level evaluation.
    glBindFramebuffer(GL_FRAMEBUFFER, framebufId);
    // Set the viewport to be the dimensions of the original heightmap.
    glViewport(0,0,width,height);
    // Disable unneccessary depth testing.
    glDisable(GL_DEPTH_TEST);

    // Activate the setup shader and give the camera an orthogonal projection.
    evaluationShader->use();
    evaluationShader->setMat4("projection",ortho);
    // Bind the heightmap texture.
    heightmapTexture->Bind();
    heightmapTexture->Assign(evaluationShader, "heightMap");
    // Bind the vertex array of quads forming the mesh.
    glBindVertexArray(terrainVAO);

    // Begin the tessellation level evaluations.
    for(int currentTessLevel = 1; currentTessLevel <= 32; currentTessLevel++){
        std::cout << "Evaluating tessellation level " << currentTessLevel << std::endl;
        // Load the tess levels into the texture.
        tessLevelsTexture->loadData(&tessLevelsBuffer[0]);
        // Bind the texture.
        tessLevelsTexture->Bind();
        // Attach the new data to the shader.
        tessLevelsTexture->Assign(evaluationShader, "tessLevels");

        // Clear the screen.
        glClearColor(1.0,1.0,1.0,1.0);
        // Clear the colour buffer.
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw the mesh.
        glDrawArrays(GL_PATCHES, 0, 4*rez*rez);

        // Wait until drawing has finished before reading from the frame buffer.
        glFinish();
        glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, &screenFloatBuffer[0]);

        // Set the read buffer in preparation for reading.
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        // Set the read byte alignment to 1.
        // (We cannot rely on the data being aligned as 4 bytes or otherwise.)
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        
        // Clear the error buffer - this is where the error of all pixels in each patch is accumulated.
        tessLevelError.fill(0.0);
        // For each pixel in the screen:
        for(int i = 0; i < width * height; i++){
            // Convert screen array index i into image coordinates.
            int imageY = i/width;
            int imageX = i-(imageY*width);
            // Convert image coordinates into patch coordinates.
            int tessY = (imageY*rez) / height;
            int tessX = (imageX*rez) / width;
            // Convert patch coordinates into patch array index.
            int pos = (tessY*rez) + tessX;
            // Accumulate the pixel error into the patch error array.
            tessLevelError[pos] += screenFloatBuffer[i];    
        }

        // Calculate the heightest possible error patch.
        int pixelsPerPatch = (int)(width/rez) * (int)(height/rez);

        // For each patch:
        for(int i = 0; i < rez*rez; i++){
            // Calculate the average pixel error.
            float averagePixelError = tessLevelError[i]/pixelsPerPatch;
            // Set the tolerance for accepting a patch and locking it.
            const float tolerance = 2.0;
            // If the patch has not been locked already:
            if(!patchIsLocked[i]){
                // Check if it meets the tolerance.
                if(averagePixelError <= tolerance){
                    // If so, lock the patch so it can no longer be evaluated.
                    patchIsLocked[i] = true;
                }else{
                    // Otherwise, increment the tessellation level of the patch.
                    tessLevelsBuffer[i] = (unsigned char)(currentTessLevel + 1);
                }
            }
        }
    }
    // Bind the frame buffer back to 0 for regular drawing to the screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Enable the depth testing we disabled earlier.
    glEnable(GL_DEPTH_TEST);
    // Resize the viewport to match the window size.
    Screen::resizeViewport();

    // Unbind the textures once we're finished evaluating.
    heightmapTexture->Unbind();
    tessLevelsTexture->Unbind();

    // Store the results to the cachedData folder.
    stbi_write_png(outputFile.c_str(), rez, rez, 1, &tessLevelsBuffer[0], rez);

    // Return the results.
    std::cout << "TessLevels image complete." << std::endl;
    return tessLevelsBuffer;
}
// Draw the mesh.
void Heightmap::Draw(Camera& camera)
{
    // Send the camera position to the shader.
    camera.sendPosition(drawShader, "camPos");

    // Bind the draw texture.
    heightmapTexture->Bind();
    heightmapTexture->Assign(drawShader, "heightMap");
    // Bind the tessLevels texture - this sends the minimum tessellation level
    // per patch to the shader.
    tessLevelsTexture->Bind();
    tessLevelsTexture->Assign(drawShader, "tessLevels");

    // Set the model position to be at the center of the screen.
    glm::mat4 model = glm::mat4(1.0f);
    // Activate the shader and set the uniforms.
    drawShader->use();
    drawShader->setMat4("projection",camera.getProjection());
    drawShader->setMat4("view",      camera.getView());
    drawShader->setMat4("model",     model);
    // Bind the vertex array of quads forming the mesh.
    glBindVertexArray(terrainVAO);
    // Draw the mesh.
    glDrawArrays(GL_PATCHES, 0, 4*rez*rez);

    // Unbind the textures once finished drawing.
    heightmapTexture->Unbind();
    tessLevelsTexture->Unbind();
}

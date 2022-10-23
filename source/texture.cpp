#include"../headers/texture.h"

// A texture constructor for creating a blank image.
Texture::Texture(int width, int height, int channels, GLuint slot)
{
    unit = slot;

	// Generate an OpenGL texture object and record the ID.
	glGenTextures(1, &ID);
    // Record the dimensions and colour channels.
    Texture::width = width;
    Texture::height = height;
    Texture::channels = channels;
    // Create an empty image.
    unsigned char* bytes = NULL;
    Load(bytes);
}
// A texture constructor for loading an image file.
Texture::Texture(const char* image, GLuint slot)
{
    unit = slot;

	// Generate an OpenGL texture object and record the ID.
	glGenTextures(1, &ID);
	// Flip the image so it appears right side up.
	stbi_set_flip_vertically_on_load(true);

    // Read the image from a file and stores it in bytes.
	unsigned char* bytes = stbi_load(image, &width, &height, &channels, 0);

    // Free the image data as it is already in the OpenGL Texture object.
    if(bytes){
        // Create the image as a texture.
        Load(bytes);
        stbi_image_free(bytes);
    }else{
        std::cout << "Error: " << image << " not found." << std::endl;
        exit(0);
    }
}
// A texture constructor for a blank texture.
void Texture::loadData(unsigned char* data)
{
    unsigned char* bytes = data;
    // Set a 1 byte unpack alignment
    // as there is no guarantee this data is aligned in any particular way.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Load the data.
    Load(bytes);
}

void Texture::Load(unsigned char*  bytes){
	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch(channels){
    case 4:
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes
        );
        break;
    }
    case 3:
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes
        );
        break;
    }
    case 1:{
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes
        );
        break;
    }
    default:{
        throw std::invalid_argument("Automatic texture type recognition failed.");
    }
    }

	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}
// Frees the texture from memory.
Texture::~Texture(){
    glDeleteTextures(1, &ID);
}
// Assigns a texture slot to the uniform of a shader.
void Texture::Assign(std::shared_ptr<Shader>& shader, const char* uniform)
{
	// Gets the location of the uniform
	GLuint texUnit = glGetUniformLocation(shader->ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader->use();
	// Sets the value of the uniform
	glUniform1i(texUnit, unit);
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::getHeight(){
    return height;
}
int Texture::getWidth(){
    return width;
}
int Texture::getchannels(){
    return channels;
}
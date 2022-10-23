// Based heavily on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
#ifndef HEIGHTMAP_CLASS_H
#define HEIGHTMAP_CLASS_H

#include<iostream>
#include<vector>
#include<array>
#include<string>

#include <glad/glad.h>
#include <learnopengl/shader_t.h>
#include<stb/stb_image_write.h>

#include"vao.h"
#include"camera.h"
#include"texture.h"

class Heightmap
{
    public:
        Heightmap(const char *filename);
        ~Heightmap();
        void Draw(Camera& camera);
        void toggleSimpleShader();
    private:
        // The number of points per patch in the tessellation control shader.
        static constexpr unsigned int NUM_PATCH_PTS = 4;
        // When true, used a shader with a fixed tessellation level in rendering.
        bool viewingSimpleShader = false;
        // Shaders.
        std::shared_ptr<Shader> drawShader;         // This is a pointer to the shadre being used in the draw stage.
        std::shared_ptr<Shader> finalShader;        // Used to view the heightmap.
        std::shared_ptr<Shader> simpleShader;       // Used to view the heightmap with a fixed tessellation level.
        std::shared_ptr<Shader> evaluationShader;   // Used to determine minimum tessellation levels.
        // Textures.
        std::shared_ptr<Texture> heightmapTexture;  // Used to detail the heightmap.
        std::shared_ptr<Texture> tessLevelsTexture; // Used to set minimum tessellation levels.
        // The texture slots for each texture.
        const GLuint heightmapTexUnit  = 0;
        const GLuint tessLevelsTexUnit = 1;
        // Frame and render buffers used to create heightmaps.
        GLuint framebufId;
        GLuint renderbufId;
        GLuint depthbufId;
        // The width, height and number of channels of the heightmap.
        int width, height, channels;
        // The heightmap mesh vertices.
        std::vector<float> vertices;
        // The Vertex Array Object and Vertex Buffer Object.
        GLuint terrainVAO, terrainVBO;
        // Set the number of patches across and down.
        // The total number is rez^2.
        static constexpr unsigned int rez = 20;
        // The miminum tessellation levels for each patch of the mesh.
        std::array<unsigned char, rez*rez> tessLevels;
        // The heightmap image filename and directory.
        std::string fileName;
        std::string fileDirectory;
        // The method used to load chached tessLevel images and generate new ones.
        std::array<unsigned char, rez*rez> loadTessLevelImage();
};

#endif


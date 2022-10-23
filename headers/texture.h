#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<learnopengl/shader_t.h>

class Texture
{
    public:
        GLuint ID;
        GLuint unit;
        Texture(int width, int height, int channels, GLuint slot = 0);
        Texture(const char* image, GLuint slot = 0);
        ~Texture();
        void loadData(unsigned char* data);
        void    Assign(std::shared_ptr<Shader>& shader, const char* uniform);
        void    Bind();
        void    Unbind();
        int     getHeight();
        int     getWidth();
        int     getchannels();
    private:
        int width, height, channels;
        void Load(unsigned char* bytes);
};

#endif
#ifndef VBO_H
#define VBO_H

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>
#include<iostream>

class VBO
{
    public:
        VBO();
        ~VBO();
        void Bind();
        void Unbind();
    private:
        GLuint ID;
};

#endif
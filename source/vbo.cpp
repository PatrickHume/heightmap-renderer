#include"../headers/vbo.h"

// Generates a Vertex Buffer Object and an ID.
VBO::VBO()
{
	glGenBuffers(1, &ID);
}
// Frees the buffer from memory.
VBO::~VBO(){
	glDeleteBuffers(1, &ID);
}
// Binds the VBO.
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}
// Unbinds the VBO.
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

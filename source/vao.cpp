#include"../headers/vao.h"

// Generates a Array Buffer Object and an ID.
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}
// Frees the buffer from memory.
VAO::~VAO()
{
	glDeleteVertexArrays(1, &ID);
}
// Binds the VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}
// Unbinds the VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}

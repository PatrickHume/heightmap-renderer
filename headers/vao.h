#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"vbo.h"

class VAO
{
	public:
		VAO();
		~VAO();
		void Bind();
		void Unbind();
	private:
		GLuint ID;
};
#endif
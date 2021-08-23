#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include "stb_image.h"

class Texture
{
public:
	Texture(std::string imageFilePath) : ID(0), data(nullptr), width(0), height(0), nChannels(0)
	{
		// Create and bind 1 texture
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		// set the texture wrapping/filtering options (on the currently bound texture object)

		// Repeat texture if sampling outside of the 0, 1 region
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Linearly interpolate for minification + magnification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load texture from image, flipped by default since OGL handles textures differently from how it's written to usually
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(imageFilePath.c_str(), &width, &height, &nChannels, 0);

		if (data)
		{
			if (nChannels == 3)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				std::cout << "Successfully loaded " << imageFilePath << " with number of channels: " << nChannels << std::endl;
			}
			else if (nChannels == 4)
			{
				// for the alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				std::cout << "Successfully loaded " << imageFilePath << " with number of channels: " << nChannels << std::endl;
			}
			else
			{
				std::cout << "Failed to load texture (nChannels must be 3 or 4!): " << imageFilePath << " " << std::endl;
			}
		}	
		else
		{
			std::cout << "Failed to load texture (stbi_load() failed): " << imageFilePath << " " << std::endl;
		}
	}
	~Texture()
	{
		stbi_image_free(data);
		unbind();
		glDeleteTextures(1, &ID);
	}

	void bind()
	{
		glBindTexture(GL_TEXTURE_2D, ID);
	}
	void unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int getID()
	{
		return ID;
	}

private:
	unsigned int ID;
	unsigned char* data;

	int width;
	int height;
	int nChannels;
};


#endif
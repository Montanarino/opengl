#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef __EMSCRIPTEN__
    #include <glad/gles2.h>
#else
    #include <glad/gl.h>
#endif
#include <stb_image.h>
#include <iostream>

class Texture {
public:
    unsigned int ID;

    Texture(const char* path) {
        stbi_set_flip_vertically_on_load(true);  
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum imageFormat = GL_RGB;
            GLenum internalFormat = GL_RGB8;

            if (nrChannels == 1) {
                imageFormat = GL_RED;
                internalFormat = GL_R8;
            } else if (nrChannels == 4) {
                imageFormat = GL_RGBA;
                internalFormat = GL_RGBA8;
            }

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << path << std::endl;
        }
    }

    void use() {
        glBindTexture(GL_TEXTURE_2D, ID);
    }
};

#endif
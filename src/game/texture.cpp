#include <SDL3_image/SDL_image.h>
#include "texture.h"

Texture::Texture(const std::string &path)
	: m_rendererID(0), m_filePath(path), m_width(0), m_height(0), m_BPP(0)
{
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (!surface)
	{
		SDL_Log("Failed to load image");
		return;
	}
	surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
	m_localBuffer = FlipSurface(surface);
	if (!m_localBuffer)
	{
		SDL_Log("Failed to save image in m_localBuffer");
		return;
	}

	m_width = m_localBuffer->w;
	m_height = m_localBuffer->h;

	GLCall(glGenTextures(1, &m_rendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer->pixels));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_rendererID));
	if (m_localBuffer)
	{
		SDL_DestroySurface(m_localBuffer);
		m_localBuffer = nullptr;
	}
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererID));
}
void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

SDL_Surface* Texture::FlipSurface(SDL_Surface* surface)
{
	// helper function from gemini
    // Create a new surface with the same dimensions and format
    SDL_Surface* flippedSurface = SDL_CreateSurface(
        surface->w, surface->h, surface->format
    );

    if (!flippedSurface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to create flipped surface: %s",
                     SDL_GetError());
        return nullptr;
    }

    // Get pixel format information (optional, but good for understanding)
    // SDL_PixelFormat* format = surface->format;
    // int bpp = format->BytesPerPixel; // Bytes per pixel

    // Iterate through each row and flip vertically
    for (int y = 0; y < surface->h; ++y) {
        Uint8* srcRow = (Uint8*)surface->pixels + y * surface->pitch;
        // Calculate the destination row from the bottom up
        Uint8* dstRow = (Uint8*)flippedSurface->pixels + (surface->h - 1 - y) * flippedSurface->pitch;

        // Copy entire row of bytes
        SDL_memcpy(dstRow, srcRow, surface->pitch);
    }

    return flippedSurface;
}
#pragma once

#include "renderer.h"

class Texture
{
private:
	unsigned int m_rendererID;
	std::string m_filePath;
	SDL_Surface* m_localBuffer;
	int m_width, m_height, m_BPP;

public:
	Texture(const std::string &path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_width; }
	inline int GetHeight() const { return m_height; }
private:
	SDL_Surface* FlipSurface(SDL_Surface* surface);
};
#pragma once
#include <SDL3_image/SDL_image.h>

enum class AssetType
{
	Texture,
	Audio
};

class Asset
{
private:
	AssetType m_type;
	SDL_Texture *m_texture;
	SDL_ScaleMode m_scaleMode;

public:
	Asset(SDL_Texture *texture, SDL_ScaleMode scaleMode) : m_texture(texture), m_type(AssetType::Texture), m_scaleMode(scaleMode)
	{
		SDL_SetTextureScaleMode(m_texture, scaleMode);
	}
	~Asset()
	{
		switch (m_type)
		{
		case AssetType::Texture:
		{
			SDL_DestroyTexture(m_texture);
			break;
		}
		}
	}
	SDL_Texture *getTexture() { return m_texture; }
};
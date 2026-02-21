#pragma once

#include "../core/GameObject.hpp"
#include <SDL3/SDL_render.h>

class GenericImage: public GameObject
{
    public:
    GenericImage(SDL_Renderer* renderer, std::string filePath)
    {
        Tag = filePath;
        Type = GameObjectTypeEnum::DRAWABLE;

        SDL_Point texture_size = {0};

        char* pngPath = NULL;

        SDL_asprintf(&pngPath, "%s%s", SDL_GetBasePath(), filePath.c_str());

        SDL_Surface* surface = SDL_LoadPNG(pngPath);

        SDL_free(pngPath);

        texture_size.x = surface->w;
        texture_size.y = surface->h;

        Dimensions.w = SourceRect.w = surface->w;
        Dimensions.h = SourceRect.h = surface->h;
        SourceRect.x = 0;
        SourceRect.y = 0;

        Texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_DestroySurface(surface);
    }
};

#pragma once

#include "../core/GameObject.hpp"
#include <SDL3/SDL_render.h>

class BackGround: public GameObject
{
    public:
    BackGround(SDL_Renderer* renderer)
    {
        Tag = "BackGround";
        Type = GameObjectTypeEnum::DRAWABLE;

        SDL_Point texture_size = {0};

        char* pngPath = NULL;

        SDL_asprintf(&pngPath, "%sAssets/MapBase.png", SDL_GetBasePath());

        SDL_Surface* surface = SDL_LoadPNG(pngPath);

        if(surface == NULL)
        {
        	printf("Error loading texture %s\n", SDL_GetError());
        }

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

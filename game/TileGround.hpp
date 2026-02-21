#pragma once

#include "../core/GameObject.hpp"
#include "Car.hpp"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <cmath>

class TileGround: public GameObject
{
    SDL_FRect _tileOffset = {0, -250, 0, 0};
    Car* _car;
    public:

    TileGround(SDL_Renderer* renderer, Car* car)
    {
        snprintf(Tag, 100, "TileGround");
        Type = GameObjectTypeEnum::DRAWABLE;

        char* pngPath = NULL;

        _car = car;
        SDL_asprintf(&pngPath, "%sAssets/Isometric Tiles/Ground G1_E.png", SDL_GetBasePath());
        // SDL_asprintf(&pngPath, "%sAssets/Isometric Tiles/Ground A1_S.png", SDL_GetBasePath());

        SDL_Surface* surface = SDL_LoadPNG(pngPath);

        SDL_free(pngPath);

        SourceRect.h = Dimensions.h = surface->h;
        Dimensions.w = SourceRect.w = surface->w;
        SourceRect.x = 0;
        SourceRect.y = 0;

        Dimensions.y = Dimensions.x = 0;

        Texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_DestroySurface(surface);
    }

    void Update(float deltaTime) override
    {
        GameObject::Update(deltaTime);
        float velocity = 50.;
        _tileOffset.x = -fmod(_car->Dimensions.x, Dimensions.w);
        _tileOffset.y = -fmod(_car->Dimensions.y, Dimensions.w) - 200;
    }

    void Draw(SDL_Renderer* renderer) override
    {
        SDL_FRect tilePosition = {0};

        tilePosition.w = Dimensions.w;
        tilePosition.h = Dimensions.h;

        tilePosition.y = 100;

        int counter = 0;

        for(int j = -100; j < 800; j += 32)
        {
            counter++;
            for(int i = 0; i < 800; i += Dimensions.w)
            {
                if(counter % 2 == 0)
                {
                    tilePosition.x = i + _tileOffset.x;
                } else {
                    tilePosition.x = i + _tileOffset.x - Dimensions.w / 2.;
                }
                tilePosition.y = j + _tileOffset.y;
                SDL_RenderTexture(renderer, Texture, &SourceRect, &tilePosition);
            }
        }
    }
};

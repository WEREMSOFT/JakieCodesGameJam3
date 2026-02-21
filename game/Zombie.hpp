#pragma once
#include "MovingGameObject.hpp"
#include "KeyboardBehavior.hpp"
class Zombie: public MovingGameObject
{
    static constexpr float TWO_PI = 2. * M_PI;
    static const int NUM_FRAMES = 8;
    SDL_FRect _prevDimensions = {0};

    public:
    Zombie(SDL_Renderer* renderer)
    {
        snprintf(Tag, 100, "Zombie");
        Type = GameObjectTypeEnum::DRAWABLE;

        Velocity = 70.;
        AngularVelocity = 2.;

        SDL_Point texture_size = {0};

        char* pngPath = NULL;

        SDL_asprintf(&pngPath, "%sAssets/Walk.png", SDL_GetBasePath());

        SDL_Surface* surface = SDL_LoadPNG(pngPath);

        SDL_free(pngPath);

        texture_size.x = surface->w;
        texture_size.y = surface->h;

        Dimensions.h = Dimensions.w = SourceRect.h = SourceRect.w = 128;
        SourceRect.x = 0;
        SourceRect.y = 0;

        Dimensions.y = Dimensions.x = 128;

        Texture = SDL_CreateTextureFromSurface(renderer, surface);

        AddChild(new KeyboardBehavior());

        SDL_DestroySurface(surface);
    }

    void Update(float deltaTime) override
    {
        static float elapsedFrametime = 0;
        elapsedFrametime += deltaTime;
        GameObject::Update(deltaTime);

        if(elapsedFrametime > 0.1 && (_prevDimensions.x != Dimensions.x || _prevDimensions.y != Dimensions.y))
        {
            elapsedFrametime = 0;
            SourceRect.x += 128;
            if(SourceRect.x > (1920 - 128))
            {
                SourceRect.x = 0;
            }
        }

        float tempAngle = fmodf(Angle, TWO_PI ) - M_PI * 0.5;

        if(tempAngle < 0)
        {
            tempAngle += TWO_PI;
        }


        float sector = TWO_PI / NUM_FRAMES;

        tempAngle += sector * .5;

        int frame = ((int)(tempAngle / sector)) % NUM_FRAMES;

        SourceRect.y = 128 * frame;

        _prevDimensions = Dimensions;
    }
};

#pragma once

#include "MovingGameObject.hpp"
#include "KeyboardBehavior.hpp"
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

class Car: public MovingGameObject
{
    static constexpr float TWO_PI = 2. * M_PI;
    static const int NUM_FRAMES = 16;
    SDL_FRect _prevDimensions = {0};
	
    public:
		bool honk = false;

        Car(SDL_Renderer* renderer)
        {
            Tag = "Car";
            Type = GameObjectTypeEnum::DRAWABLE;

            Velocity = 200.;
            AngularVelocity = 2.;

            SDL_Point texture_size = {0};

            char* pngPath = NULL;

            SDL_asprintf(&pngPath, "%sAssets/PixelWheels_Porsche_Red.png", SDL_GetBasePath());

            SDL_Surface* surface = SDL_LoadPNG(pngPath);

            SDL_free(pngPath);

            texture_size.x = surface->w;
            texture_size.y = surface->h;

            Dimensions.h = Dimensions.w = SourceRect.h = SourceRect.w = 100;
            SourceRect.x = 0;
            SourceRect.y = 0;

            Texture = SDL_CreateTextureFromSurface(renderer, surface);

            AddChild(new KeyboardBehavior(&honk));

            SDL_DestroySurface(surface);

			Dimensions.x = 1596.;
			// Dimensions.x = 5096.;
            Dimensions.y = 1561.;
        }

        void Update(float deltaTime) override
        {
            static float elapsedFrametime = 0;
            elapsedFrametime += deltaTime;
            GameObject::Update(deltaTime);

            if(elapsedFrametime > 0.1 && (_prevDimensions.x != Dimensions.x || _prevDimensions.y != Dimensions.y))
            {
                elapsedFrametime = 0;
                SourceRect.x += 100;
                if(SourceRect.x > 300)
                {
                    SourceRect.x = 0;
                }
            }

            float tempAngle = fmodf(Angle, TWO_PI);
            if(tempAngle < 0)
            {
                tempAngle += TWO_PI;
            }

            float sector = TWO_PI / NUM_FRAMES;

            tempAngle += sector * .5;

            int frame = ((int)(tempAngle / sector)) % NUM_FRAMES;

            SourceRect.y = 100 * frame;

            _prevDimensions = Dimensions;
        }
};

#pragma once

#include "../core/GameObject.hpp"
#include <SDL3/SDL_render.h>

class SplashScreen: public GameObject
{
	float elapsedFrameTime = 0;
	int splashScreenFrame = 0;
    public:
    SplashScreen(SDL_Renderer* renderer, SDL_Texture* pTexture)
    {
        Tag = "SplashScreen";
        Type = GameObjectTypeEnum::DRAWABLE;

        SDL_Point texture_size = {0};

        texture_size.x = pTexture->w;
        texture_size.y = pTexture->h;

		Dimensions = {(800 - 81) / 2, (600 - 32) / 2, 81, 32};

        SourceRect = {0, 417, 81, 32};

        Texture = pTexture;
    }

	void Update(float deltaTime)
	{
		GameObject::Update(deltaTime);
		elapsedFrameTime += deltaTime;

		if(elapsedFrameTime > 0.4)
		{
			elapsedFrameTime = 0;
			splashScreenFrame++;
			splashScreenFrame %= 5;
		}
	}

	void Draw(SDL_Renderer* renderer)
	{
		if(Texture == NULL) return;
		SourceRect.x = splashScreenFrame * 82;
		SDL_RenderTexture(renderer, Texture, &SourceRect, &Dimensions);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); 
		SDL_RenderDebugText(renderer, 320, 320, "Press Space to Start");
	}
};

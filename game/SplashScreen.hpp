#pragma once

#include "../core/GameObject.hpp"
#include <SDL3/SDL_render.h>

class SplashScreen: public GameObject
{
	float elapsedFrameTime = 0;
	int splashScreenFrame = 0;
	SDL_Texture* _instructions;
	SDL_FRect instructionsDimensions;
	SDL_FRect instructionsSourceRect;
	
    public:
    
	bool showInstructions = true;

	SplashScreen(SDL_Renderer* renderer, SDL_Texture* pTexture)
    {
        Tag = "SplashScreen";
        Type = GameObjectTypeEnum::DRAWABLE;

		_instructions = pTexture;

		char* pngPath = NULL;

		SDL_asprintf(&pngPath, "%sAssets/Nuts-and-furry.png", SDL_GetBasePath());

		SDL_Surface* surface = SDL_LoadPNG(pngPath);

		SDL_free(pngPath);

        SDL_Point texture_size = {0};

        texture_size.x = pTexture->w;
        texture_size.y = pTexture->h;

		Dimensions.h = SourceRect.h = surface->h;
		Dimensions.w = SourceRect.w = surface->w;
		SourceRect.x = 0;
		SourceRect.y = 0;

		Texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_DestroySurface(surface);

		instructionsDimensions = {(800 - 81) / 2, 520, 81, 32};

        instructionsSourceRect = {0, 417, 81, 32};

        // Texture = pTexture;
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
		if(!showInstructions) return;

		SDL_RenderTexture(renderer, Texture, &SourceRect, &Dimensions);


		instructionsSourceRect.x = splashScreenFrame * 82;
		SDL_RenderTexture(renderer, _instructions, &instructionsSourceRect, &instructionsDimensions);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); 
		SDL_RenderDebugText(renderer, 320, 560, "Press Space to Start");
	}
};

#pragma once
#include "../core/GameObject.hpp"
#include "Car.hpp"
#include "BackGround.hpp"
#include "ForeGround.hpp"
#include "SplashScreen.hpp"
#include "PalomaSystem.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

class World: public GameObject
{
    SDL_Texture *texture = NULL;
    SDL_Texture *splashScreenTexture = NULL;
    Car* _car;
    BackGround* _backGround;
    PalomaSystem* _palomaSystem;
	bool showSplashScreen = true;

    public:
        World(SDL_Renderer* renderer)
        {
            Type = GameObjectTypeEnum::DRAWABLE;

            Tag = "World";

            _backGround = new BackGround(renderer);
            AddChild(_backGround);

            _car = new Car(renderer);
            _car->Dimensions.x = 5096.;
            _car->Dimensions.y = 1561.;

			_car->Angle = -M_PI / 2;

            char* pngPath = NULL;

            SDL_asprintf(&pngPath, "%sAssets/Pigeon.png", SDL_GetBasePath());

            SDL_Surface* surface = SDL_LoadPNG(pngPath);

            SDL_free(pngPath);

            Texture = SDL_CreateTextureFromSurface(renderer, surface);
            _palomaSystem = new PalomaSystem(Texture, _car);
            SDL_DestroySurface(surface);

            AddChild(_car);
            AddChild(_palomaSystem);

            auto treesFront = new ForeGround(renderer);
            AddChild(treesFront);

			CreateSplashScreen(renderer);
        }

		void CreateSplashScreen(SDL_Renderer* renderer)
		{
			SDL_Point texture_size = {0};

			char* pngPath = NULL;

			SDL_asprintf(&pngPath, "%sAssets/Instructions.png", SDL_GetBasePath());

			SDL_Surface* surface = SDL_LoadPNG(pngPath);

			SDL_free(pngPath);

			texture_size.x = surface->w;
			texture_size.y = surface->h;

			Dimensions.w = SourceRect.w = surface->w;
			Dimensions.h = SourceRect.h = surface->h;
			SourceRect.x = 0;
			SourceRect.y = 0;

			splashScreenTexture = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_DestroySurface(surface);
		}

		void DrawSplashScreen(SDL_Renderer* renderer)
		{
			SDL_FRect position = {(800 - 320) / 2, (600 - 240) / 2, 320, 240};
			SDL_RenderTexture(renderer, splashScreenTexture, NULL, &position);
		}

        ~World()
        {
            SDL_DestroyTexture(Texture);
            printf("destroying world\n");
        }

        void Update(float deltaTime) override
        {
            GameObject::Update(deltaTime);
            auto carPosition = _car->GetWorldPositions();
            // Camera follows car
            Dimensions.x = -_car->Dimensions.x + 412;
            Dimensions.y = -_car->Dimensions.y + 300;

			const bool* keys = SDL_GetKeyboardState(NULL);

            if(keys[SDL_SCANCODE_SPACE])
            {
                showSplashScreen = false;
            }
        }

        void Draw(SDL_Renderer* renderer) override
        {
            GameObject::Draw(renderer);
			if(showSplashScreen)
				DrawSplashScreen(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); 
            SDL_RenderDebugText(renderer, 0, 0, "Esc to quit(on desktop)");
            SDL_RenderDebugText(renderer, 0, 40, "WASD to accelerate, break and turn. H for horn.");
            SDL_RenderDebugText(renderer, 0, 60, "Try to herd the squirrel to the LEFT through the piggeons.");

            char carPositionText[300] = {0};
            snprintf(carPositionText, 300, "Car Position: %.2f, %.2f", _car->Dimensions.x, _car->Dimensions.y);
            SDL_RenderDebugText(renderer, 0, 20, carPositionText);
			
			char startledPiggeonsText[300] = {0};
            snprintf(startledPiggeonsText, 300, "Startled Piggeons %d(%.2f%%)", _palomaSystem->startledPiggeons, (_palomaSystem->startledPiggeons / (float)PALOMAS_COUNT) * 100.);
            SDL_RenderDebugText(renderer, 0, 80, startledPiggeonsText);

            if(_palomaSystem->squirrelPosition.x < 935)
       		{
       			SDL_RenderDebugText(renderer, 100, 100, "YOU CATCHED THE SQUIRREL!!");
       		} else {
       			char text[300] = {0};
       			sprintf(text, "squirrel position %.2f, %.2f", _palomaSystem->squirrelPosition.x, _palomaSystem->squirrelPosition.y);
       			SDL_RenderDebugText(renderer, 100, 100, text);
       		}
            
        }
};

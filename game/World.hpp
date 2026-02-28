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
    Car* _car;
    BackGround* _backGround;
	SplashScreen* splashScreen;
    public:
    	PalomaSystem* _palomaSystem;
		bool showDebuginformation = false;
		bool showHelp = false;

        World(SDL_Renderer* renderer)
        {
            Type = GameObjectTypeEnum::DRAWABLE;

            Tag = "World";

            _backGround = new BackGround(renderer);
            AddChild(_backGround);

            _car = new Car(renderer);

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

			splashScreen = new SplashScreen(renderer, Texture);
			AddChild(splashScreen);  
        }

        ~World()
        {
            SDL_DestroyTexture(Texture);
            printf("destroying world\n");
        }

        void Update(float deltaTime) override
        {
			if(_palomaSystem->isGameWin)
			{
				return;
			}
            GameObject::Update(deltaTime);

            auto carPosition = _car->GetWorldPositions();
            // Camera follows car
            Dimensions.x = -_car->Dimensions.x + 412;
            Dimensions.y = -_car->Dimensions.y + 300;

			const bool* keys = SDL_GetKeyboardState(NULL);

            if(keys[SDL_SCANCODE_SPACE])
            {
				splashScreen->Texture = NULL;
            }
        }

        void Draw(SDL_Renderer* renderer) override
        {
            GameObject::Draw(renderer);
			
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); 
			
			if(showHelp)
			{
				SDL_RenderDebugText(renderer, 0, 0, "Esc to quit(on desktop)");
				SDL_RenderDebugText(renderer, 0, 20, "WASD to accelerate, break and turn. H for horn.");
				SDL_RenderDebugText(renderer, 420, 10, "<--- Use the signal to track the squirrel.");
			}
			else
			{
				SDL_RenderDebugText(renderer, 10, 580, "Press F1 hide/show help.");
			}


			if(showDebuginformation)
			{
				char carPositionText[300] = {0};
				snprintf(carPositionText, 300, "Car Position: %.2f, %.2f", _car->Dimensions.x, _car->Dimensions.y);
				SDL_RenderDebugText(renderer, 550, 40, carPositionText);
				
				char startledPiggeonsText[300] = {0};
				snprintf(startledPiggeonsText, 300, "Startled Piggeons %d(%.2f%%)", _palomaSystem->startledPiggeons, (_palomaSystem->startledPiggeons / (float)PALOMAS_COUNT) * 100.);
				SDL_RenderDebugText(renderer, 550, 60, startledPiggeonsText);
			}

            if(_palomaSystem->isGameWin)
       		{
				SDL_FRect sourceRect = {0, 448, 255, 64};
				SDL_FRect targetRect = {(800 - 255) / 2, (600 - 64) / 2, 255, 64};
				SDL_RenderTexture(renderer, Texture, &sourceRect, &targetRect);
       			SDL_RenderDebugText(renderer, 300, 290, "YOU CATCHED THE SQUIRREL!!");
				char startledPiggeonsText[300] = {0};
				snprintf(startledPiggeonsText, 300, "Startled Piggeons %d(%.2f%%)", _palomaSystem->startledPiggeons, (_palomaSystem->startledPiggeons / (float)PALOMAS_COUNT) * 100.);
				SDL_RenderDebugText(renderer, 300, 300, startledPiggeonsText);
       		}
            
        }
};

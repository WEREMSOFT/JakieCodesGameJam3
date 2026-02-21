#pragma once
#include "../core/GameObject.hpp"
#include "Car.hpp"
#include "BackGround.hpp"
#include "ForeGround.hpp"
#include "PalomaSystem.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

class World: public GameObject
{
    SDL_Texture *texture = NULL;
    Car* _car;
    BackGround* _backGround;
    PalomaSystem* _palomaSystem;

    public:
        World(SDL_Renderer* renderer)
        {
            Type = GameObjectTypeEnum::DRAWABLE;

            Tag = "World";

            _backGround = new BackGround(renderer);
            Dimensions.x = -_backGround->Dimensions.w / 2.;
            Dimensions.y = -_backGround->Dimensions.h / 2.;
            AddChild(_backGround);

            _car = new Car(renderer);
            _car->Dimensions.x = _backGround->Dimensions.w / 2.;
            _car->Dimensions.y = _backGround->Dimensions.h / 2.;

            _car->Dimensions.x += (1991.f - 906.f) + 200;

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
        }

        void Draw(SDL_Renderer* renderer) override
        {
            GameObject::Draw(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  /* white, full alpha */
            SDL_RenderDebugText(renderer, 0, 0, "Esc to quit(on desktop)");
            SDL_RenderDebugText(renderer, 0, 40, "WASD to accelerate, break and turn. H for horn.");
            SDL_RenderDebugText(renderer, 0, 60, "Try to herd the squirrel to the LEFT through the piggeons.");

            char carPositionText[300] = {0};
            snprintf(carPositionText, 300, "Car Position: %.2f, %.2f", _car->Dimensions.x, _car->Dimensions.y);

            SDL_RenderDebugText(renderer, 0, 20, carPositionText);
        }
};

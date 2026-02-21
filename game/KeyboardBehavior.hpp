#pragma once
#include <SDL3/SDL_scancode.h>
#include <math.h>
#include <stdio.h>
#include "MovingGameObject.hpp"
#include "../core/Vector2.hpp"

class Car;

#include <SDL3/SDL.h>

class KeyboardBehavior: public GameObject
{
	bool* _honk;
    public:
        KeyboardBehavior(bool* honk)
        {
			_honk = honk;
            Type = GameObjectTypeEnum::BEHAVIOR;
            Tag = "Keyboard Behavior";
        }

        void Update(float deltaTime) override
        {
            Vector2f direction = { 0, 1. };
            GameObject::Update(deltaTime);
			auto car = (Car*)Parent;

            auto movingParent = (MovingGameObject*)Parent;

            static float phase = 0;

            phase += 2. * deltaTime;

            const bool* keys = SDL_GetKeyboardState(NULL);

            if(keys[SDL_SCANCODE_SPACE])
            {
                Parent->Angle = M_PI;
            }

            if (keys[SDL_SCANCODE_D]) {
                Parent->Angle += movingParent->AngularVelocity * deltaTime;
            }

            if (keys[SDL_SCANCODE_A]) {
                Parent->Angle -= movingParent->AngularVelocity * deltaTime;
            }

            direction = Rotate(direction, Parent->Angle);

            if (keys[SDL_SCANCODE_W]) {
                Vector2f vecIncrement = Scale(direction, -movingParent->Velocity * deltaTime);
                Parent->Dimensions.x += vecIncrement.x;
                Parent->Dimensions.y += vecIncrement.y;
            }

            if (keys[SDL_SCANCODE_S]) {
                Vector2f vecIncrement = Scale(direction, movingParent->Velocity * deltaTime);
                Parent->Dimensions.x += vecIncrement.x;
                Parent->Dimensions.y += vecIncrement.y;
            }

			if (keys[SDL_SCANCODE_H]) {
				*_honk = true;
            } else {
				*_honk = false;
			}
        }
};

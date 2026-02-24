#pragma once
#include "Animal.hpp"
#include "Car.hpp"

class Piggeon
{
    public:

    static void Init(Animal* paloma)
    {
        paloma->Type = AnimalTypeEnum::Piggeon;
        paloma->Velocity = 100.;
        paloma->Dimensions.h = paloma->Dimensions.w = paloma->SourceRect.h = paloma->SourceRect.w = 32;
        paloma->SourceRect.x = 0;
        paloma->SourceRect.y = 0;

        float piggeonX = SDL_randf() * 2500.;
        float piggeonY = SDL_randf() * 2500.;

		Vector2f rotatedPosition = {piggeonX, piggeonY};

		rotatedPosition = Rotate(rotatedPosition, M_PI / 4.);

        paloma->Dimensions.x = rotatedPosition.x + 3190.;
        paloma->Dimensions.y = rotatedPosition.y * 0.5  + 700.;
    }

	static void UpdateStateIdle(Animal* paloma, Car* _car)
	{
        Vector2f distanceV;
        float distance;
        distanceV = (Vector2f){ paloma->Dimensions.x - (_car->Dimensions.x + _car->Dimensions.w * .3f), paloma->Dimensions.y - (_car->Dimensions.y + _car->Dimensions.h * .3f) };
        distance = Length2(distanceV);
              if(paloma->elapsedIddleTime > 2.)
              {
                  paloma->elapsedIddleTime = 0;
                  paloma->Animation = (int)(random() % 2 == 0 ? PiggeonAnimationEnum::IDLE_1 : PiggeonAnimationEnum::IDLE_2);
              }

              if(distance < (_car->honk?14400.:2500.))
              {
              	distanceV = Normalize(distanceV);
                  paloma->State = (int)State::FLYING;
                  paloma->Animation = (int)PiggeonAnimationEnum::FLYING;
                  paloma->baseY = paloma->Dimensions.y;
                  paloma->velocityY = -100.;
                  paloma->Angle = SDL_randf() * M_PI * 0.5 - M_PI * 0.25;
                  paloma->direction = Rotate(distanceV, paloma->Angle);
              }
	}

	static void UpdateStateFlying(Animal* paloma, float deltaTime)
	{
		 const float gravity = 50.8;

		 paloma->velocityY += gravity * deltaTime;
		 paloma->Dimensions.y += paloma->velocityY * deltaTime ;

		 Vector2f vecIncrement = Scale(paloma->direction, paloma->Velocity * deltaTime);
		 paloma->Dimensions.x += vecIncrement.x;
		 paloma->Dimensions.y += vecIncrement.y;
		 paloma->baseY += vecIncrement.y;
		 paloma->baseDifferenceY = paloma->baseY - paloma->Dimensions.y;

		 if(paloma->Dimensions.y > paloma->baseY)
		 {
		     paloma->velocityY = -15.;
		     paloma->Dimensions.y = paloma->baseY;
		     paloma->State = (int)State::IDLE;
		 }
	}

    static void UpdateAnimation(Animal* paloma, float deltaTime, float elapsedFrametime)
    {
        paloma->elapsedIddleTime += deltaTime;
        paloma->SourceRect.y = 32 * paloma->Animation;


        if(paloma->baseY == 0)
        {
            paloma->baseY = paloma->Dimensions.y;
        }

        if(elapsedFrametime > 0.1)
        {
            paloma->SourceRect.x += 32;
            if(paloma->SourceRect.x > (32 * 3))
            {
                paloma->SourceRect.x = 0;
            }
        }
    }
};


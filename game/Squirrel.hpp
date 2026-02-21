#pragma once
#include "Animal.hpp"
#include "Car.hpp"

class Squirrel
{
    public:
    
    static void Init(Animal* squirrel, Car* _car)
    {
        squirrel->Type = AnimalTypeEnum::Squirrel;
        squirrel->Animation = (int)SquirrelAnimationEnum::IDLE_3;

        squirrel->Velocity = 100.;
        squirrel->Dimensions.h = squirrel->Dimensions.w = squirrel->SourceRect.h = squirrel->SourceRect.w = 32;
        squirrel->SourceRect.x = 0;
        squirrel->SourceRect.y = 0;

        squirrel->Dimensions.x = _car->Dimensions.x - 100;
        squirrel->Dimensions.y = _car->Dimensions.y;
    }

    static inline void Update(Animal* squirrel, float deltaTime, Car* _car, float elapsedFrametime, AnimationDefinition* squirrelAnimations)
    {
        static const int FrameStart = 128 + 32;
        squirrel->elapsedIddleTime += deltaTime;
        squirrel->SourceRect.y = 32 * squirrel->Animation + FrameStart;

        Vector2f distanceV;
        float distance;

        if(squirrel->baseY == 0)
        {
            squirrel->baseY = squirrel->Dimensions.y;
        }

        distanceV = (Vector2f){ squirrel->Dimensions.x - (_car->Dimensions.x + _car->Dimensions.w * .5f), squirrel->Dimensions.y - (_car->Dimensions.y + _car->Dimensions.h * .5f) };
        distance = Length(distanceV);

        switch (squirrel->State) {
            case int(State::IDLE):
                if(squirrel->elapsedIddleTime > 2.)
                {
                    squirrel->elapsedIddleTime = 0;
                    squirrel->Animation = (int)(random() % 2 == 0 ? SquirrelAnimationEnum::IDLE_1 : SquirrelAnimationEnum::IDLE_3);
                }

  	            if(distance < (_car->honk?120.:50.))
                {
                	distanceV = Normalize(distanceV);
                    squirrel->State = (int)State::FLYING;
                    squirrel->Animation = (int)SquirrelAnimationEnum::FLYING;
                    squirrel->baseY = squirrel->Dimensions.y;
                    squirrel->velocityY = -100.;
                    squirrel->Angle = SDL_randf() * M_PI * 0.5 - M_PI * 0.25;
                    squirrel->direction = Rotate(distanceV, squirrel->Angle);
                }


                break;
            case (int)State::FLYING:
                const float gravity = 50.8;

                squirrel->velocityY += gravity * deltaTime;
                squirrel->Dimensions.y += squirrel->velocityY * deltaTime ;

                Vector2f vecIncrement = Scale(squirrel->direction, squirrel->Velocity * deltaTime);
                squirrel->Dimensions.x += vecIncrement.x;
                squirrel->Dimensions.y += vecIncrement.y;
                squirrel->baseY += vecIncrement.y;
                squirrel->baseDifferenceY = squirrel->baseY - squirrel->Dimensions.y;
                if(squirrel->Dimensions.y > squirrel->baseY)
                {
                    squirrel->velocityY = -15.;
                    squirrel->Dimensions.y = squirrel->baseY;
                    squirrel->State = (int)State::IDLE;
                }
        }


        if(elapsedFrametime > 0.1)
        {
            squirrel->SourceRect.x += 32;
            if(squirrel->SourceRect.x > (32 * squirrelAnimations[squirrel->Animation].frames))
            {
                squirrel->SourceRect.x = 0;
            }
        }
    }
};
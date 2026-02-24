#pragma once
#include <SDL3/SDL.h>
#include "../core/Vector2.hpp"

enum class SquirrelAnimationEnum
{
    IDLE_1 = 0,
    IDLE_2,
    FLYING,
    WALKING,
    IDLE_3,
    COUNT
};

enum class State{
    IDLE,
    FLYING,
    COUNT
};

enum class PiggeonAnimationEnum
{
    IDLE_1,
    WALKING,
    IDLE_2,
    FLYING,
    COUNT
};

enum AnimalTypeEnum
{
    Piggeon,
    Squirrel
};

struct AnimationDefinition
{
    int frames;
};

struct Animal
{
    SDL_FRect Dimensions;
    SDL_FRect SourceRect;
    Vector2f direction;
    int Animation;
    int State;
    AnimalTypeEnum Type;
    float elapsedIddleTime;
    float baseY;
    float Angle;
    float velocityY;
    float baseDifferenceY;
    float Velocity;
	bool startled;
};

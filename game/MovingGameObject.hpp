#pragma once
#include "../core/GameObject.hpp"
class MovingGameObject: public GameObject
{
    public:
    float Velocity = 0;
    float AngularVelocity = 0;
    MovingGameObject(GameObjectTypeEnum type = GameObjectTypeEnum::UNKNOWN): GameObject(type)
    {
    }
};

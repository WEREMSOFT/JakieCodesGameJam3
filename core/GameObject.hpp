#pragma once
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <stdio.h>
#include <vector>
#include <string>

#include <SDL3/SDL.h>

#include "GameObjectTypeEnum.hpp"

class GameObject {
    protected:

    public :
        std::vector<GameObject*> Children;
        float Angle = 0;
        std::string Tag = "GameObject";
        SDL_Texture* Texture;
        GameObjectTypeEnum Type;
        GameObject* Parent = nullptr;
        SDL_FRect Dimensions = {0};
        SDL_FRect SourceRect = {0};

        GameObject(GameObjectTypeEnum type = GameObjectTypeEnum::UNKNOWN)
        {
            Type = type;
            Children.reserve(2);
        }

        virtual ~GameObject()
        {
            if(Tag == "GameObject")
            {
                printf("destroying game object\n");
            }
            else
            {
                printf("Destroying gameobject with tag: %s\n", Tag.c_str());
            }

            for(int i = 0; i < Children.size(); i++)
            {
                delete Children[i];
            }
        }

        virtual void Update(float deltaTime)
        {
            for(int i = 0; i < Children.size(); i++)
            {
                Children[i]->Update(deltaTime);
            }
        }

        virtual void Draw(SDL_Renderer* renderer)
        {
            if(Texture != NULL)
            {
                auto tempParent = Parent;
                SDL_FRect worldPosition = GetWorldPositions();

                if(SourceRect.h == 0 || SourceRect.w == 0)
                    SDL_RenderTexture(renderer, Texture, NULL, &worldPosition);
                else
                {
                    SDL_RenderTexture(renderer, Texture, &SourceRect, &worldPosition);
                }
            }

            for(int i = 0; i < Children.size(); i++)
            {
                if(Children[i]->Type == GameObjectTypeEnum::DRAWABLE)
                {
                    Children[i]->Draw(renderer);
                }
            }
        }

        virtual void AddChild(GameObject* child)
        {
            child->Parent = this;
            Children.push_back(child);
        }

        static GameObject* CreateDrawable(SDL_Texture* texture)
        {
            GameObject *gameObject = new GameObject(GameObjectTypeEnum::DRAWABLE);
            gameObject->Texture = texture;
            gameObject->Dimensions.w = texture->w;
            gameObject->Dimensions.h = texture->h;

            return gameObject;
        }

        virtual SDL_FRect GetWorldPositions()
        {
            auto tempParent = Parent;
            SDL_FRect worldPosition = { Dimensions.x, Dimensions.y, Dimensions.w, Dimensions.h};
            while(tempParent != nullptr)
            {
                worldPosition.x += tempParent->Dimensions.x;
                worldPosition.y += tempParent->Dimensions.y;
                tempParent = tempParent->Parent;
            }
            return worldPosition;
        }
};

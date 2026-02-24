#pragma once
#include "../core/GameObject.hpp"
#include <SDL3/SDL_rect.h>
#include "../core/Vector2.hpp"
#include <SDL3/SDL_render.h>
#include <cstdlib>
#include "Car.hpp"
#include "Animal.hpp"
#include "Piggeon.hpp"
#include "Squirrel.hpp"
#include <assert.h>

#define SQUIRREL_COUNT 1
#define PALOMAS_COUNT 200000

#define ENTITY_COUNT (PALOMAS_COUNT + SQUIRREL_COUNT)

#define VERTS_PER_QUAD 4
#define INDICES_PER_QUAD 6

#define MAX_QUADS ENTITY_COUNT
#define MAX_VERTS (MAX_QUADS * VERTS_PER_QUAD)
#define MAX_IDXS (MAX_QUADS * INDICES_PER_QUAD)

struct IndexSOA
{
	float g_xy[MAX_VERTS * 2];
	float g_uv[MAX_VERTS * 2];
	SDL_FColor g_col[MAX_VERTS];
	Uint32 g_idx[MAX_IDXS];
};

class PalomaSystem : public GameObject
{
	float elapsedFrametime = 0;
	float elapsedSignalTime = 0;
	int previousSquirrelState = (int)State::IDLE;
	bool emmitSignal = true;
	Car *_car;
	SDL_FRect shadowSource = {0};
	AnimationDefinition squirrelAnimations[(int)SquirrelAnimationEnum::COUNT] = {0};

	static float positions[ENTITY_COUNT * 2];

	IndexSOA palomasSOA = {0};
	IndexSOA sombrasSOA = {0};

	int textW;
	int textH;

	Vector2f squirrelPosition = {0};

public:
	int startledPiggeons = 0;
	Animal *Palomas;
	SDL_Texture *Texture;
	PalomaSystem(SDL_Texture *texture, Car *car)
	{
		Palomas = (Animal *)calloc(ENTITY_COUNT, sizeof(Animal));
		Texture = texture;
		Type = GameObjectTypeEnum::DRAWABLE;
		_car = car;

		for (int i = 0; i < PALOMAS_COUNT; i++)
		{
			Piggeon::Init(&Palomas[i]);
		}

		for(int i = 0; i < SQUIRREL_COUNT; i++)
		{
			Squirrel::Init(&Palomas[PALOMAS_COUNT + i], _car);
			#define SQUIRREL_STARTING_COORD_X 3545.f
			#define SQUIRREL_STARTING_COORD_Y 1206.f
			Palomas[PALOMAS_COUNT + i].Dimensions.x = SQUIRREL_STARTING_COORD_X + SDL_randf() * (3900.f - SQUIRREL_STARTING_COORD_X);
			Palomas[PALOMAS_COUNT + i].Dimensions.y = SQUIRREL_STARTING_COORD_Y + SDL_randf() * (1991.f - SQUIRREL_STARTING_COORD_Y);
		}


		
		squirrelPosition.x = Palomas[ENTITY_COUNT - 1].Dimensions.x;
		squirrelPosition.y = Palomas[ENTITY_COUNT - 1].Dimensions.y;

		shadowSource.x = 0;
		shadowSource.y = 128;
		shadowSource.w = 32;
		shadowSource.h = 32;

		squirrelAnimations[(int)SquirrelAnimationEnum::IDLE_1].frames = 5;
		squirrelAnimations[(int)SquirrelAnimationEnum::IDLE_2].frames = 5;
		squirrelAnimations[(int)SquirrelAnimationEnum::IDLE_3].frames = 1;
		squirrelAnimations[(int)SquirrelAnimationEnum::FLYING].frames = 7;

		InitQuadIndicesSOA();

		textH = Texture->h;
		textW = Texture->w;
	}

	~PalomaSystem()
	{
		free(Palomas);
	}

	void Update(float deltaTime) override
	{
		elapsedFrametime += deltaTime;
		elapsedSignalTime += deltaTime * 10.;

		Vector2f distanceV = {0};
		float distance = 0;

		auto worldPosition = GetWorldPositionOffset();
		SDL_FRect pos = { 0, 0, Palomas->Dimensions.w, Palomas->Dimensions.h};
		
		
		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			pos.x = worldPosition.x + Palomas[i].Dimensions.x;
			pos.y = worldPosition.y + Palomas[i].Dimensions.y;

			// if (pos.x < -15 || pos.y < -15 || pos.x > 780 || pos.y > 680)
			// continue;
		
			switch (Palomas[i].Type)
			{
			case AnimalTypeEnum::Piggeon:
				Piggeon::UpdateAnimation(&Palomas[i], deltaTime, elapsedFrametime);

		        switch (Palomas[i].State) 
		        {
		            case int(State::IDLE):
            			if (pos.x < -15 || pos.y < -15 || pos.x > 780 || pos.y > 680)
							continue;

	            		Piggeon::UpdateStateIdle(&Palomas[i], _car);
						if(Palomas[i].State == (int)State::FLYING)
						{
							startledPiggeons++;
						}
		                break;
		            case (int)State::FLYING:
		            	Piggeon::UpdateStateFlying(&Palomas[i], deltaTime);
		        }

				// if(emmitSignal)
					ResetAnimationBasedOnPosition(&Palomas[i], squirrelPosition);

				break;
			case AnimalTypeEnum::Squirrel:
				previousSquirrelState = Palomas[i].State;
				Squirrel::Update(&Palomas[i], deltaTime, _car, elapsedFrametime, squirrelAnimations);
				if(previousSquirrelState != Palomas[i].State)
				{
					squirrelPosition.x = Palomas[i].Dimensions.x;
					squirrelPosition.y = Palomas[i].Dimensions.y;
					emmitSignal = true;
					elapsedSignalTime = 0;
				}
				break;
			}

			ConstraintObjectsToMap(&Palomas[i].Dimensions, &Palomas[i].Angle, &Palomas[i].direction);
			ConstraintCarToMap(&_car->Dimensions);
		}

		if (elapsedFrametime > 0.1)
		{
			elapsedFrametime = 0;
			qsort(Palomas, ENTITY_COUNT, sizeof(Animal), comparePaloma);
		}

		if(elapsedSignalTime > 500.0)
		{
			elapsedSignalTime = 0;
			// emmitSignal = false;
		}
	}

	void ResetAnimationBasedOnPosition(Animal* animal, Vector2f position)
	{
		Vector2f distanceV = {animal->Dimensions.x - position.x, animal->Dimensions.y - position.y};
		float distance = Length2(distanceV);

		float sinElapsedTime  = sin(elapsedSignalTime);

		if(distance > 5000. * elapsedSignalTime && distance < 5010. * elapsedSignalTime )
		{
			if(animal->State == (int)State::IDLE)
			{
				animal->Animation = (int)PiggeonAnimationEnum::IDLE_2;
			}
			animal->elapsedIddleTime = 0.;
		} 
	}


	void Draw(SDL_Renderer *renderer) override
	{
		int quad_count = 0;
		int sombra_quad_count = 0;

		auto worldPosition = GetWorldPositionOffset();
		SDL_FRect pos = { 0, 0, Palomas->Dimensions.w, Palomas->Dimensions.h};

		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			pos.x = worldPosition.x + Palomas[i].Dimensions.x;
			pos.y = worldPosition.y + Palomas[i].Dimensions.y;

			if (pos.x < -100 || pos.y < -100 || pos.x > 880 || pos.y > 780)
				continue;

			int flip = (Palomas[i].direction.x < 0);

			PushQuadPaloma(
				quad_count,
				&pos,
				&Palomas[i].SourceRect,
				flip,
				textW,
				textH);

				
			if(Palomas[i].baseDifferenceY > 0)
			{
				pos.y += Palomas[i].baseDifferenceY;
				PushQuadSombras(
					sombra_quad_count,
					&pos,
					&shadowSource,
					0,
					textW,
					textH);

				sombra_quad_count++;
			}

			quad_count++;
		}

		if(sombra_quad_count > 0)
		{
			SDL_RenderGeometryRaw(
				renderer, Texture,
				sombrasSOA.g_xy, sizeof(float) * 2,
				sombrasSOA.g_col, sizeof(SDL_FColor),
				sombrasSOA.g_uv, sizeof(float) * 2,
				sombra_quad_count * 4,
				sombrasSOA.g_idx, sombra_quad_count * 6, sizeof(Uint32));
		}


		if (quad_count > 0)
		{
			SDL_RenderGeometryRaw(
				renderer, Texture,
				palomasSOA.g_xy, sizeof(float) * 2,
				palomasSOA.g_col, sizeof(SDL_FColor),
				palomasSOA.g_uv, sizeof(float) * 2,
				quad_count * 4,
				palomasSOA.g_idx, quad_count * 6, sizeof(Uint32));
		}
	}

	void InitQuadIndicesSOA(void)
	{
		for (int i = 0; i < MAX_QUADS; i++)
		{
			Uint32 base = i * 4;
			int o = i * 6;

			palomasSOA.g_idx[o + 0] = base + 0;
			palomasSOA.g_idx[o + 1] = base + 1;
			palomasSOA.g_idx[o + 2] = base + 2;
			palomasSOA.g_idx[o + 3] = base + 2;
			palomasSOA.g_idx[o + 4] = base + 3;
			palomasSOA.g_idx[o + 5] = base + 0;

			sombrasSOA.g_idx[o + 0] = base + 0;
			sombrasSOA.g_idx[o + 1] = base + 1;
			sombrasSOA.g_idx[o + 2] = base + 2;
			sombrasSOA.g_idx[o + 3] = base + 2;
			sombrasSOA.g_idx[o + 4] = base + 3;
			sombrasSOA.g_idx[o + 5] = base + 0;
		}
	}
	
	inline void PushQuadSombras(
		int quad,
		const SDL_FRect *dst,
		const SDL_FRect *src,
		int flip_x,
		int tex_w,
		int tex_h)
	{
		int v = quad * 4;
		int p = v * 2;

		sombrasSOA.g_xy[p + 0] = dst->x;
		sombrasSOA.g_xy[p + 1] = dst->y;

		sombrasSOA.g_xy[p + 2] = dst->x + dst->w;
		sombrasSOA.g_xy[p + 3] = dst->y;

		sombrasSOA.g_xy[p + 4] = dst->x + dst->w;
		sombrasSOA.g_xy[p + 5] = dst->y + dst->h;

		sombrasSOA.g_xy[p + 6] = dst->x;
		sombrasSOA.g_xy[p + 7] = dst->y + dst->h;

		float u0 = src->x / tex_w;
		float v0 = src->y / tex_h;
		float u1 = (src->x + src->w) / tex_w;
		float v1 = (src->y + src->h) / tex_h;

		if (flip_x)
		{
			float t = u0;
			u0 = u1;
			u1 = t;
		}

		sombrasSOA.g_uv[p + 0] = u0;
		sombrasSOA.g_uv[p + 1] = v0;
		sombrasSOA.g_uv[p + 2] = u1;
		sombrasSOA.g_uv[p + 3] = v0;
		sombrasSOA.g_uv[p + 4] = u1;
		sombrasSOA.g_uv[p + 5] = v1;
		sombrasSOA.g_uv[p + 6] = u0;
		sombrasSOA.g_uv[p + 7] = v1;

		for (int i = 0; i < 4; i++)
			sombrasSOA.g_col[v + i] = (SDL_FColor){1.0, 1.0, 1.0, 1.0};
	}

	inline void PushQuadPaloma(
		int quad,
		const SDL_FRect *dst,
		const SDL_FRect *src,
		int flip_x,
		int tex_w,
		int tex_h)
	{
		int v = quad * 4;
		int p = v * 2;

		palomasSOA.g_xy[p + 0] = dst->x;
		palomasSOA.g_xy[p + 1] = dst->y;

		palomasSOA.g_xy[p + 2] = dst->x + dst->w;
		palomasSOA.g_xy[p + 3] = dst->y;

		palomasSOA.g_xy[p + 4] = dst->x + dst->w;
		palomasSOA.g_xy[p + 5] = dst->y + dst->h;

		palomasSOA.g_xy[p + 6] = dst->x;
		palomasSOA.g_xy[p + 7] = dst->y + dst->h;

		float u0 = src->x / tex_w;
		float v0 = src->y / tex_h;
		float u1 = (src->x + src->w) / tex_w;
		float v1 = (src->y + src->h) / tex_h;

		if (flip_x)
		{
			float t = u0;
			u0 = u1;
			u1 = t;
		}

		palomasSOA.g_uv[p + 0] = u0;
		palomasSOA.g_uv[p + 1] = v0;
		palomasSOA.g_uv[p + 2] = u1;
		palomasSOA.g_uv[p + 3] = v0;
		palomasSOA.g_uv[p + 4] = u1;
		palomasSOA.g_uv[p + 5] = v1;
		palomasSOA.g_uv[p + 6] = u0;
		palomasSOA.g_uv[p + 7] = v1;

		for (int i = 0; i < 4; i++)
			palomasSOA.g_col[v + i] = (SDL_FColor){1.0, 1.0, 1.0, 1.0};
	}

	void DrawPalomasBatchSOA(
		Animal *palomas,
		SDL_Renderer *renderer,
		SDL_Texture *texture,
		int tex_w,
		int tex_h)
	{
		int quad_count = 0;

		auto worldPosition = GetWorldPositionOffset();
		SDL_FRect pos = { 0, 0, palomas->Dimensions.w, palomas->Dimensions.h};

		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			Animal *p = &palomas[i];

			pos.x = worldPosition.x + p->Dimensions.x;
			pos.y = worldPosition.y + p->Dimensions.y;

			if (pos.x < -15 || pos.y < -15 || pos.x > 780 || pos.y > 680)
				continue;

			int flip = (p->direction.x < 0);

			PushQuadPaloma(
				quad_count++,
				&pos,
				&p->SourceRect,
				flip,
				tex_w,
				tex_h);

			pos.y += p->baseDifferenceY;

			PushQuadSombras(
				quad_count++,
				&pos,
				&shadowSource,
				0,
				tex_w,
				tex_h);
		}

		if (quad_count > 0)
		{
			SDL_RenderGeometryRaw(
				renderer, texture,
				sombrasSOA.g_xy, sizeof(float) * 2,
				sombrasSOA.g_col, sizeof(SDL_FColor),
				sombrasSOA.g_uv, sizeof(float) * 2,
				quad_count * 4,
				sombrasSOA.g_idx, quad_count * 6, sizeof(Uint32));

			SDL_RenderGeometryRaw(
				renderer, texture,
				palomasSOA.g_xy, sizeof(float) * 2,
				palomasSOA.g_col, sizeof(SDL_FColor),
				palomasSOA.g_uv, sizeof(float) * 2,
				quad_count * 4,
				palomasSOA.g_idx, quad_count * 6, sizeof(Uint32));
		}
	}

	static int comparePaloma(const void *a, const void *b)
	{
		const Animal *pa = (const Animal *)a;
		const Animal *pb = (const Animal *)b;

		if (pa->Dimensions.y < pb->Dimensions.y)
			return -1;
		if (pa->Dimensions.y > pb->Dimensions.y)
			return 1;
		return 0;
	}

	SDL_FRect GetWorldPositionOffset()
	{
		auto tempParent = Parent;
		SDL_FRect worldPosition = {0};
		while (tempParent != nullptr)
		{
			worldPosition.x += tempParent->Dimensions.x;
			worldPosition.y += tempParent->Dimensions.y;
			tempParent = tempParent->Parent;
		}
		return worldPosition;
	}

	void ConstraintObjectsToMap(SDL_FRect* location, float* angle, Vector2f* direction)
	{
		Vector2f prevPosition = {location->x, location->y};
		location->y = SDL_clamp(location->y, .5f * location->x - 1250, .5f * location->x + 1240);
		location->y = SDL_clamp(location->y, -.5f * location->x + 1930, -.5f * location->x + 4430);
		location->x = SDL_clamp(location->x, 724, 5580);

		if(location->x != prevPosition.x || location->y != prevPosition.y)
		{
			*angle += M_PI;
			*direction = Rotate(*direction, *angle);
		}
	}

	void ConstraintCarToMap(SDL_FRect* location)
	{
		Vector2f prevPosition = {location->x, location->y};
		location->y = SDL_clamp(location->y, .5f * location->x - 1250, .5f * location->x + 1200);
		location->y = SDL_clamp(location->y, -.5f * location->x + 1930, -.5f * location->x + 4350);
		location->x = SDL_clamp(location->x, 724, 5580);
	}
};

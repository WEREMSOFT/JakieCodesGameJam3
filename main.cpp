#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <cstdio>
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
SDL_COMPILE_TIME_ASSERT(SDL_Version, SDL_VERSION_ATLEAST(3, 4, 0));
#include "game/World.hpp"
#include <math.h>
#include "DebugFlags.h"

#ifndef FORCE_VULKAN
#include <SDL3/SDL_opengl.h>
#endif

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

Uint64 last = SDL_GetPerformanceCounter();

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{

    const int compiled = SDL_VERSION;  /* hardcoded number from SDL headers */
    const int linked = SDL_GetVersion();  /* reported by linked SDL library */

    SDL_Log("We compiled against SDL version %d.%d.%d ...\n",
            SDL_VERSIONNUM_MAJOR(compiled),
            SDL_VERSIONNUM_MINOR(compiled),
            SDL_VERSIONNUM_MICRO(compiled));

    SDL_Log("But we are linking against SDL version %d.%d.%d.\n",
            SDL_VERSIONNUM_MAJOR(linked),
            SDL_VERSIONNUM_MINOR(linked),
            SDL_VERSIONNUM_MICRO(linked));

    SDL_SetAppMetadata("SDL3 Test", "1.0", "none");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef FORCE_VULKAN
	 SDL_SetHint(SDL_HINT_RENDER_DRIVER, "vulkan");
#endif

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Squirrel Herding!!!", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
	char* renderName = (char *)SDL_GetStringProperty(props, SDL_PROP_RENDERER_NAME_STRING, 0);


    SDL_SetRenderLogicalPresentation(renderer, 800, 600, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_SetRenderVSync(renderer, 0);

    World* world = new World(renderer);

    *appstate = world;

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

bool showWireframe = false;

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    if(event->type == SDL_EVENT_KEY_DOWN)
    {
        if(event->key.scancode == SDL_SCANCODE_ESCAPE)
        {
            return SDL_APP_SUCCESS;
        }
    }

    if(event->type == SDL_EVENT_KEY_DOWN)
    {
        if(event->key.scancode == SDL_SCANCODE_1)
        {
			showWireframe = !showWireframe;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void printFPSInformation(SDL_Renderer* renderer, float deltaTime)
{
    #define FPS_HISTORY 50
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    char fpsText[100] = {0};
    char frameTimeText[100] = {0};
    static float fps[FPS_HISTORY] = {0};

    static int fpsCounter = 0;
    fpsCounter++;
    fpsCounter %= FPS_HISTORY;
    fps[fpsCounter] = 1.f / deltaTime;

    float average = 0.;

    for(int i = 0; i < FPS_HISTORY; i++)
    {
        average += fps[i];
    }

    average /= FPS_HISTORY;

    snprintf(fpsText, 100, "fps: %.3f", average);
    SDL_RenderDebugText(renderer, 300, 0, fpsText);
    
    snprintf(frameTimeText, 100, "frameTime: %.3f", deltaTime);
    SDL_RenderDebugText(renderer, 300, 20, frameTimeText);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint64 now = SDL_GetPerformanceCounter();
    double deltaTime =
        (double)(now - last) / (double)SDL_GetPerformanceFrequency();
    last = now;

    SDL_SetRenderDrawColor(renderer, 15, 50, 90, SDL_ALPHA_OPAQUE);  /* dark gray, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */

    World* world = (World *)appstate;

    world->Update(deltaTime);
    world->Draw(renderer);

	if(world->showDebuginformation)
	{
		printFPSInformation(renderer, deltaTime);
	}

	

#ifndef FORCE_VULKAN

#ifndef __EMSCRIPTEN__
	if(showWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#endif
#endif
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    delete (World*)appstate;
}

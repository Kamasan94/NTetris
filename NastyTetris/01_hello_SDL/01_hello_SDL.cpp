/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

/* Headers */
//Using SDL and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "../LTexture.h"

/* Constants */
//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };

/* Function Prototypes */
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The surface contained by the window
SDL_Renderer* gRenderer{ nullptr };

//The PNG image we will render
LTexture gPngTexture;

/* Function Implementations */
bool init()
{
	//Initialization flag
	bool success{ true };

	

	//Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
	
		if (!SDL_CreateWindowAndRenderer("SDL3 Tutorial: Hello SDL3", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer))
		{
			SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Initialize PNG loading
			int imgFlags = 2;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				SDL_Log("SDL_image could not initialize! SDL_image error: %s\n", SDL_GetError());
				success = false;
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//File loading flag
	bool success{ true };
    success = gPngTexture.loadFromFile("02-textures-and-extension-libraries/loaded.png");
	//Load splash image
	if ( !success)
	{
		SDL_Log("Unable to load png image!\n");
	}

	return success;
}

void close()
{
	//Clean up texture
	gPngTexture.destroy();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
    //Final exit code
    int exitCode{ 0 };

    //Initialize
    if (!init())
    {
        SDL_Log("Unable to initialize program!\n");
        exitCode = 1;
    }
    else
    {
        //Load media
        if (!loadMedia())
        {
            SDL_Log("Unable to load media!\n");
            exitCode = 2;
        }
        else
        {
            //The quit flag
            bool quit{ false };

            //The event data
            SDL_Event e;
            SDL_zero(e);

            //The main loop
            while (quit == false)
            {
                //Get event data
                while (SDL_PollEvent(&e))
                {
                    //If event is quit type
                    if (e.type == SDL_EVENT_QUIT)
                    {
                        //End the main loop
                        quit = true;
                    }
                }

                //Fill the background white
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                //Render image on screen
                gPngTexture.render(0.f, 0.f);

                //Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    //Clean up
    close();

    return exitCode;
}
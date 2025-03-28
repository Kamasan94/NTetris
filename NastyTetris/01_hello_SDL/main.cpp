/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

/* Headers */
//Using SDL and STL string
//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

/* Constants */

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//Key press surfaces constants
//Default to start counting at 0 and go up by one for each enumeration declared
enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

//Loads individual image
SDL_Surface* loadSurface(std::string path);

//Current displayed image
SDL_Surface* gCurrentSurface = NULL;

//Current displayed image stretched
SDL_Surface* gStretchedSurface = NULL;

/***Texture hardware based rendering***/

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

/************************************/

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image ad specified path
	bool loadFromFile(std::string path);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;
LTexture gModulatedTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuaad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuaad.w = clip->w;
		renderQuaad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuaad);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at a specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s\n!, SDL_Image error:", path.c_str(), SDL_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture form surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture form %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Module texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}


//Scene sprites
SDL_Rect gSpriteClips[4];
LTexture gSpriteSheetTexture;



//The images that correspond to a key press
//Array of pointers to SDL surfaces to contain all images we'll be using
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str()); //IMG_Load can load different image types

	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL error: %s\n", path.c_str(), SDL_GetError());
		}
		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return optimizedSurface;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Rendere could not be created! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_Image Error:%s\n", IMG_GetError());
					success = false;
				}
				else
				{
					//Get window surface
					gScreenSurface = SDL_GetWindowSurface(gWindow);
				}
			}
		}
	}

	return success;
}

SDL_Texture* loadTexture(std::string path) {
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at a specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s\n!, SDL_Image error:%s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		//Create texture form surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture form %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}


bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load default surface
	/*
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("assets/images/press.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("Failed to load default image!\n");
		success = false;
	}

	//Load up surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("assets/images/up.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
	{
		printf("Failed to load up image!\n");
		success = false;
	}

	//Load down surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("assets/images/down.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
	{
		printf("Failed to load down image!\n");
		success = false;
	}

	//Load left surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("assets/images/left.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
	{
		printf("Failed to load left image!\n");
		success = false;
	}

	//Load right surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("assets/images/right.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
	{
		printf("Failed to load right image!\n");
		success = false;
	}
	*/

	//Load PNG texture
	/*gTexture = loadTexture("assets/textures/texture.png");
	if (gTexture == NULL)
	{
		printf("Failed to load teture image!\n");
		success = false;
	}*/


	//Load Foo texture
	//if (!gFooTexture.loadFromFile("assets/images/foo.png"))
	//{
	//	printf("Failed to load Foo texture image!\n");
	//	success = false;
	//}

	////Load bacground texture
	//if (!gBackgroundTexture.loadFromFile("assets/images/background.png"))
	//{
	//	printf("Failed to load background texture image!\n");
	//	success = false;
	//}

	//Load sprite sheet texture
	//if (!gSpriteSheetTexture.loadFromFile("assets/images/dots.png"))
	//{
	//	printf("Failed to load sprite sheet texture!\n");
	//	success = false;
	//}
	//else
	//{
	//	//Set top left sprite
	//	gSpriteClips[0].x = 0;
	//	gSpriteClips[0].y = 0;
	//	gSpriteClips[0].w = 100;
	//	gSpriteClips[0].h = 100;

	//	//Set top right sprite
	//	gSpriteClips[1].x = 100;
	//	gSpriteClips[1].y = 0;
	//	gSpriteClips[1].w = 100;
	//	gSpriteClips[1].h = 100;

	//	//Set bottom left sprite
	//	gSpriteClips[2].x = 0;
	//	gSpriteClips[2].y = 100;
	//	gSpriteClips[2].w = 100;
	//	gSpriteClips[2].h = 100;

	//	//Set bottom right sprite
	//	gSpriteClips[3].x = 100;
	//	gSpriteClips[3].y = 100;
	//	gSpriteClips[3].w = 100;
	//	gSpriteClips[3].h = 100;
	//}


	//Load front alpha texture
	if (!gModulatedTexture.loadFromFile("assets/images/fadeout.png"))
	{
		printf("Failed to load front texture!\n");
		success = false;
	}
	else
	{
		//Set standard alpha blending
		gModulatedTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	}

	//Load background texture
	if (!gBackgroundTexture.loadFromFile("assets/images/fadein.png"))
	{
		printf("Failed to load background texture!\n");
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded image
	gFooTexture.free();
	gBackgroundTexture.free();
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;


	//Deallocate surface
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load Media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			

			//Hack to get window to stay up
			SDL_Event e; 
			bool quit = false; 

			//Set default current surface
			//gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

			//Modulation components
			Uint8 r = 255;
			Uint8 g = 255;
			Uint8 b = 255;
			Uint8 a = 255;

			//Game Loop
			while (quit == false) 
			{ 
				while (SDL_PollEvent(&e)) 
				{ 
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//User presses a key
					else if (e.type == SDL_KEYDOWN)
					{
						if (e.key.keysym.sym == SDLK_w)
						{
							//Cap if over 255
							if (a + 32 > 255)
							{
								a = 255;
							}
							//Increment otherwise
							else
							{
								a += 32;
							}
						}
						//Decrease alpha on s
						else if (e.key.keysym.sym == SDLK_s)
						{
							//Cap if below 0
							if (a - 32 < 0)
							{
								a = 0;
							}
							//Decrement otherwise
							else
							{
								a -= 32;
							}
						}

						//switch (e.key.keysym.sym)
						//{
						//	//Select surfaces based on key press
						//	/*case SDLK_UP:
						//		gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
						//		break;

						//	case SDLK_DOWN:
						//		gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
						//		break;

						//	case SDLK_LEFT:
						//		gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
						//		break;

						//	case SDLK_RIGHT:
						//		gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
						//		break;*/

						//		//Increase alpha on w
						//	
	
						//	////Increase red
						//	//case SDLK_q:
						//	//	r += 32;
						//	//	break;
						//	//	//Increase green
						//	//case SDLK_w:
						//	//	g += 32;
						//	//	break;

						//	//	//Increase blue
						//	//case SDLK_e:
						//	//	b += 32;
						//	//	break;

						//	//	//Decrease red
						//	//case SDLK_a:
						//	//	r -= 32;
						//	//	break;

						//	//	//Decrease green
						//	//case SDLK_s:
						//	//	g -= 32;
						//	//	break;

						//	//	//Decrease blue
						//	//case SDLK_d:
						//	//	b -= 32;
						//	//	break;

						//	//default:
						//	//	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
						//	//	break;
						//}
						
					}
				} 

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render texture screen
				//SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

				//Render red filled quad
				//SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
				//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
				//SDL_RenderFillRect(gRenderer, &fillRect);

				////Render green outlined quad
				//SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
				//SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
				//SDL_RenderDrawRect(gRenderer, &outlineRect);

				////Draw blue horizontal line
				//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
				//SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

				////Draw vertical line of yellow dots
				//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
				//for (int i = 0; i < SCREEN_HEIGHT; i += 4) 
				//{
				//	SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH / 2, i);
				//}

				///*************VIEWPORTS**************/
				////Top left corner viewport
				//SDL_Rect topLeftViewport;
				//topLeftViewport.x = 0;
				//topLeftViewport.y = 0;
				//topLeftViewport.w = SCREEN_WIDTH / 2;
				//topLeftViewport.h = SCREEN_HEIGHT / 2;
				//SDL_RenderSetViewport(gRenderer, &topLeftViewport);

				//SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

				////Top right corner viewport
				//SDL_Rect topRightViewport;
				//topRightViewport.x = SCREEN_WIDTH / 2;
				//topRightViewport.y = 0;
				//topRightViewport.w = SCREEN_WIDTH / 2;
				//topRightViewport.h = SCREEN_HEIGHT / 2;
				//SDL_RenderSetViewport(gRenderer, &topRightViewport);

				//SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

				////Bottom viewport
				//SDL_Rect bottomViewport;
				//bottomViewport.x = 0;
				//bottomViewport.y = SCREEN_HEIGHT / 2;
				//bottomViewport.w = SCREEN_WIDTH;
				//bottomViewport.h = SCREEN_HEIGHT / 2;
				//SDL_RenderSetViewport(gRenderer, &bottomViewport);

				////Render texture to screen
				//SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

				////Render background texture 
				//gBackgroundTexture.render(0, 0);

				////Render Foo to the screen
				//gFooTexture.render(240, 190);

				////Render top left sprite
				//gSpriteSheetTexture.render(0, 0, &gSpriteClips[0]);

				////Render top right sprite
				//gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);

				////Render bottom left sprite
				//gSpriteSheetTexture.render(0, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[2]);

				////Render center sprite
				//gSpriteSheetTexture.render(SCREEN_WIDTH / 2 - gSpriteClips[3].w / 2, SCREEN_HEIGHT / 2 - gSpriteClips[3].w / 2, &gSpriteClips[3]);


				//Modulate and render texture
				/*gModulatedTexture.loadFromFile("assets/images/colors.png");
				gModulatedTexture.setColor(r, g, b);
				gModulatedTexture.render(0, 0);*/

				//Render background
				gBackgroundTexture.render(0, 0);

				//Render front blended
				gModulatedTexture.setAlpha(a);
				gModulatedTexture.render(0, 0);

				//Update screen
				SDL_RenderPresent(gRenderer);

				//Apply the image 
				//SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);

				//Apply the image stretched
				/*SDL_Rect stretchRect;
				stretchRect.x = 0;
				stretchRect.y = 0;
				stretchRect.w = SCREEN_WIDTH;
				stretchRect.h = SCREEN_HEIGHT;
				SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect);
				*/

				//Update the surface
				//SDL_UpdateWindowSurface(gWindow);

				
			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}
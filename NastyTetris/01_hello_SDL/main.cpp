/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

/* Headers */
//Using SDL and STL string
//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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

//Globally used font
TTF_Font* gFont = NULL;



/************************************/

//Button constants
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3,
	BUTTON_SPRITE_TOTAL = 4
};

//Mouse button sprites
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];

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

	//Creates image from font string
#if defined(SDL_TTF_MAJOR_VERSION)
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

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

//The mouse button
class LButton
{
public:
	//Initializes internal variables
	LButton();
	
	//Sets top left position
	void setPosition(int x, int y);

	//Handles mouse event
	void handleEvent(SDL_Event* e);

	//Shows button sprite
	void render();
private:
	//Top left position
	SDL_Point mPosition;

	//Currently used global sprite
	LButtonSprite mCurrentSprite;
};



//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;
LTexture gModulatedTexture;
LTexture gArrowTexture;

//Rendered Texture
LTexture gTextTexture;
LTexture* gMenuTextures[2];

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

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
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
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuaad, angle, center, flip);
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
		printf("Unable to load image %s\n!, SDL_Image error%s\n:", path.c_str(), SDL_GetError());
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

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rif of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text!SDL Error : % s\n", SDL_GetError());
		}
		else
		{
			//Get Image dimesnions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid opf old surface
		SDL_FreeSurface(textSurface);
	}

	//Retyurn success
	return mTexture != NULL;
}
#endif

LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;

	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
	mPosition.x = x;
	mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		//Check if mouse is in button
		bool inside = true;

		//Mouse is left of the button
		if (x < mPosition.x)
		{
			inside = false;
		}
		//Mouse is right of the button
		else if (x > mPosition.x + BUTTON_WIDTH)
		{
			inside = false;
		}
		//Mouse above the button
		else if (y < mPosition.y)
		{
			inside = false;
		}
		else if (y > mPosition.y + BUTTON_HEIGHT)
		{
			inside = false;
		}

		//Mouse is outside button
		if (!inside)
		{
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		//Mouse inside button
		else
		{
			//Set mouse over sprite
			switch (e->type)
			{
			case SDL_MOUSEMOTION:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;

			case SDL_MOUSEBUTTONDOWN:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			}
		}
	}

}

LTexture gButtonSpriteSheetTexture;

void LButton::render()
{
	//Show current button sprite
	gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}

//Scene sprites
//SDL_Rect gSpriteClips[4];
//LTexture gSpriteSheetTexture;



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



/********************************/
/**********ANIMATION*************/
/********************************/

//Walking animation
const int WALKING_ANIMATION_FRAMES = 4;
//SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;
LButton gButtons[TOTAL_BUTTONS];
Uint32 startTime;
int animationRate = 10;




bool init()
{
	//Initialization flag
	bool success = true;

	startTime = SDL_GetTicks64();

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
			//Create VSYNCED renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error:%s\n", TTF_GetError());
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
	//if (!gModulatedTexture.loadFromFile("assets/images/fadeout.png"))
	//{
	//	printf("Failed to load front texture!\n");
	//	success = false;
	//}
	//else
	//{
	//	//Set standard alpha blending
	//	gModulatedTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	//}

	////Load background texture
	//if (!gBackgroundTexture.loadFromFile("assets/images/fadein.png"))
	//{
	//	printf("Failed to load background texture!\n");
	//	success = false;
	//}

	//Load sprite sheet texture
	//if (!gSpriteSheetTexture.loadFromFile("assets/images/foo.png"))
	//{
	//	printf("Failed to laod walking animation texture!\n");
	//	success = false;
	//}
	//else
	//{
	//	//Set sprite clips
	//	gSpriteClips[0].x = 0;
	//	gSpriteClips[0].y = 0;
	//	gSpriteClips[0].w = 64;
	//	gSpriteClips[0].h = 205;

	//	gSpriteClips[1].x = 64;
	//	gSpriteClips[1].y = 0;
	//	gSpriteClips[1].w = 64;
	//	gSpriteClips[1].h = 205;

	//	gSpriteClips[2].x = 128;
	//	gSpriteClips[2].y = 0;
	//	gSpriteClips[2].w = 64;
	//	gSpriteClips[2].h = 205;

	//	gSpriteClips[3].x = 192;
	//	gSpriteClips[3].y = 0;
	//	gSpriteClips[3].w = 64;
	//	gSpriteClips[3].h = 205;
	//}

	//if (!gArrowTexture.loadFromFile("assets/images/arrow.png"))
	//{
	//	printf("Failed to laod arrow image!\n");
	//	success = false;
	//}

	////Open the font
	//gFont = TTF_OpenFont("assets/fonts/lazy.ttf", 28);
	//if (gFont == NULL)
	//{
	//	printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
	//	success = false;
	//}
	//else
	//{
	//	//Render text
	//	SDL_Color textColor = { 0, 0, 0, 0 };
	//	if (!gTextTexture.loadFromRenderedText("The quick brown fox jumps over the lazy dog", textColor))
	//	{
	//		printf("Failed to render text texture!\n");
	//		success = false;
	//	}
	//}
	if (!gButtonSpriteSheetTexture.loadFromFile("assets/images/button.png"))
	{
		printf("Failed to load button sprite texture!\n");
		success = false;
	}
	else
	{
		//Set sprites
		for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i)
		{
			gSpriteClips[i].x = 0;
			gSpriteClips[i].y = i * 200;
			gSpriteClips[i].w = BUTTON_WIDTH;
			gSpriteClips[i].h = BUTTON_HEIGHT;
		}

		//Set buttons in corners
		gButtons[0].setPosition(0, 0);
		gButtons[1].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, 0);
		gButtons[2].setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);
		gButtons[3].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT);
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
	TTF_Quit();
}


bool loadMenu()
{
	//Loading success flag
	bool success = true;
	int menuEntriesSize = 2;
	std::string menuEntries[2] = {
		"Hello SDL\n",
		"Getting an Image on the Screen\n"
	};

	//Open the font
	gFont = TTF_OpenFont("assets/fonts/lazy.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render text
		SDL_Color textColor = { 138, 138, 138};
		for (int i = 0; i < menuEntriesSize; i++)
		{
			gMenuTextures[i] = new LTexture();
			if (!gMenuTextures[i]->loadFromRenderedText(menuEntries[i], textColor))
			{
				printf("Failed to render text texture!\n");
				success = false;
			}
		}		
	}

	return success;
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
		//if (!loadMedia())
		//{
		//	printf("Failed to load media!\n");
		//}
		if (!loadMenu())
		{
			printf("Failed to load menu!\n");
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

			//Current animation frame
			int frame = 0;

			//Angle of rotation
			double degrees = 0;

			//Flip type 
			SDL_RendererFlip flipType = SDL_FLIP_NONE;

			//Index of menu selcted
			int indexSelected = 0;
			int menuEntriesNumber = 2;

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
						/*switch (e.key.keysym.sym)
						{
						case SDLK_a:
							degrees -= 60;
							break;

						case SDLK_d:
							degrees += 60;
							break;

						case SDLK_q:
							flipType = SDL_FLIP_HORIZONTAL;
							break;

						case SDLK_w:
							flipType = SDL_FLIP_NONE;
							break;

						case SDLK_e:
							flipType = SDL_FLIP_VERTICAL;
							break;
						}*/

						//if (e.key.keysym.sym == SDLK_w)
						//{
						//	//Cap if over 255
						//	if (a + 32 > 255)
						//	{
						//		a = 255;
						//	}
						//	//Increment otherwise
						//	else
						//	{
						//		a += 32;
						//	}
						//}
						////Decrease alpha on s
						//else if (e.key.keysym.sym == SDLK_s)
						//{
						//	//Cap if below 0
						//	if (a - 32 < 0)
						//	{
						//		a = 0;
						//	}
						//	//Decrement otherwise
						//	else
						//	{
						//		a -= 32;
						//	}
						//}


						//Menu control
						switch (e.key.keysym.sym)
						{
							
						case SDLK_UP:
							indexSelected--;
							break;

						case SDLK_DOWN:
							indexSelected++;
							break;
						case 13:
							printf("Premuto invio");
						

						default:
							indexSelected = 0;
							break;
						}

						//Adjust menu index
						if (indexSelected >= menuEntriesNumber)
						{
							indexSelected = 0;
						}
						else if (indexSelected < 0)
						{
							indexSelected = menuEntriesNumber - 1;
						}

								//switch (e.key.keysym.sym)
								//{
								//	//Select surfaces based on key press
								//case SDLK_UP:
								//	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
								//	break;

								//case SDLK_DOWN:
								//	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
								//	break;

								//case SDLK_LEFT:
								//	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
								//	break;

								//case SDLK_RIGHT:
								//	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
								//	break;

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
					else
					{
						for (int i = 0; i < TOTAL_BUTTONS; i++)
						{
							gButtons[i].handleEvent(&e);
						}
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

				////Render background
				//gBackgroundTexture.render(0, 0);

				////Render front blended
				//gModulatedTexture.setAlpha(a);
				//gModulatedTexture.render(0, 0);


				////Render current frame
				//SDL_Rect* currentClip = &gSpriteClips[frame];
				//gSpriteSheetTexture.render((SCREEN_WIDTH - currentClip->w) / 2, (SCREEN_HEIGHT - currentClip->h) / 2, currentClip);

				

				////Go to next frame
				//frame = ((SDL_GetTicks64() - startTime) * animationRate / 1000) % 4;

				////Cycle animation
				//if (frame / 4 >= WALKING_ANIMATION_FRAMES)
				//{
				//	frame = 0;
				//}

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

				//Render arrow
				//gArrowTexture.render((SCREEN_WIDTH - gArrowTexture.getWidth()) / 2, (SCREEN_HEIGHT - gArrowTexture.getHeight()) / 2, NULL, degrees, NULL, flipType);

				//Render current frame
				//gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight()) / 2);

				//Update the surface
				//SDL_UpdateWindowSurface(gWindow);

				//Render buttons
				/*for (int i = 0; i < TOTAL_BUTTONS; ++i)
				{
					gButtons[i].render();
				}*/
					
				
				//Set the index selected bold
				gMenuTextures[indexSelected]->setColor(255, 255, 0);

				//Render menu entries
				for (int i = 0; i < menuEntriesNumber; i++)
				{
					if (i != indexSelected)
					{
						gMenuTextures[i]->setColor(138, 138, 138);
					}
					gMenuTextures[i]->render((SCREEN_WIDTH - gMenuTextures[i]->getWidth()) / 2, (SCREEN_HEIGHT - gMenuTextures[i]->getHeight() + (50*i)) / 2);
				}
				
				


				//Update screen
				SDL_RenderPresent(gRenderer);


				
			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}
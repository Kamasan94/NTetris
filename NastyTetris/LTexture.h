#pragma once
/* Headers */
//Using SDL, SDL_image, and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>

class LTexture
{
public:
	//Initializes texture variables
	LTexture();

	//Cleans up the texture variables
	~LTexture();

	//Loads texture from disk
	bool loadFromFile(std::string patch);

	//Clearns up textures
	void destroy();

	//Draws texture
	void render(float x, float y);

	//Gets texture dimensions
	int getWidth();
	int getHeight();

private:
	//Contains texture data
	SDL_Texture* mTexture;
	SDL_Renderer* mRenderer;

	//Texture dimensions
	int mWidth;
	int mHeight;
};


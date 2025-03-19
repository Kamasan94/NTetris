#include "LTexture.h"

//LTexture Implementation
LTexture::LTexture() :
	//Initialize texture variables
	mTexture{ nullptr },
	mWidth{ 0 },
	mHeight{ 0 }
{

}

LTexture::~LTexture()
{
	//Clean up texture
	destroy();
}

bool LTexture::loadFromFile(std::string path)
{
	//Clean up texture if it already exists
	destroy();

	//Load surface
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == nullptr)
	{
		SDL_Log("Unable to load image %s! SDL_Image error: $\n", path.c_str(), SDL_GetError());
	}
	else
	{
		//Create texture from surface
		mTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
		if (mTexture == nullptr)
		{
			SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
		}
		else
		{
			//Get Image dimension
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Clear up loaded surface
		SDL_DestroySurface(loadedSurface);
	}

	//Return success if texture loaded
	return mTexture != nullptr;
}

void LTexture::destroy()
{
	//Clean up texture
	SDL_DestroyTexture(mTexture);
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
}

void LTexture::render(float x, float y)
{
	//Set texture position
	SDL_FRect dstRect = { x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

	//Render texture
	SDL_RenderTexture(mRenderer, mTexture, nullptr, &dstRect);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}
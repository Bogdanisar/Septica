#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>
#include <windows.h>

using namespace std;

#ifndef cTextureGuard
#define cTextureGuard

class cTexture {
    private:
    SDL_Renderer *mRend;
    SDL_Texture* mTexture;
    TTF_Font* mFont;
    int mWidth,mHeight;

    public:
    cTexture() {
        mWidth = mHeight = 0;
        mTexture = NULL;
        mFont = NULL;
    }
    void setRendFont(SDL_Renderer *_mRend = NULL,TTF_Font *_mFont = NULL) {
        mTexture = NULL;
        mRend = _mRend;
        mFont = _mFont;
    }

    ~cTexture() {
        free();
    }

    void free() {
        if (mTexture != NULL) {
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
            mWidth = mHeight = 0;
        }
    }

    bool loadTexture(const string& imagePath) {
        free();

        SDL_Surface *surf = IMG_Load(imagePath.c_str());
        if (surf == NULL) {
            printf("Error loading image at %s: %s\n",imagePath.c_str(),IMG_GetError());
            return false;
        }

        //SDL_SetColorKey(surf,SDL_TRUE,SDL_MapRGB(surf->format, 0, 0xFF, 0xFF));

        mTexture = SDL_CreateTextureFromSurface(mRend,surf);
        mWidth = surf->w;
        mHeight = surf->h;
        SDL_FreeSurface(surf);

        if (mTexture == NULL) {
            printf("Error creating texture from surface: %s\n",SDL_GetError());
            return false;
        }

        return true;
    }

    bool loadTexture(const string& text,SDL_Color textColor) {
        free();

        //cout<<text;
        SDL_Surface *surf = TTF_RenderText_Solid(mFont,text.c_str(),textColor);
        if (surf == NULL) {
            printf("Unable to load TTF from %s: %s!\n",text.c_str(),SDL_GetError());
            return false;
        }

        mTexture = SDL_CreateTextureFromSurface(mRend,surf);
        mWidth = surf->w;
        mHeight = surf->h;
        SDL_FreeSurface(surf);

        if (mTexture == NULL) {
            printf("Unable to create texture from surface: %s!\n",SDL_GetError());
            return false;
        }

        return true;
    }

    int getWidth()
    {
        return mWidth;
    }

    int getHeight()
    {
        return mHeight;
    }

    void render(int x,int y) {
        SDL_Rect dest = {x,y,mWidth,mHeight};

        SDL_RenderCopy(mRend,mTexture,NULL,&dest);
    }


    void render(SDL_Rect* dest,SDL_Rect* src = NULL,double angle = 0) {
        SDL_RenderCopyEx(mRend,mTexture,src,dest,angle,NULL,SDL_FLIP_NONE);
    }
};

#endif // cTextureGuard

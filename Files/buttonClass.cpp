#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>
#include <windows.h>
#include "textureClass.cpp"
#include "cardEnums.cpp"

using namespace std;

#ifndef buttonClassGuard
#define buttonClassGuard

class cButton {
    private:
    int mx,my,mWidth,mHeight;
    cTexture onTexture;
    cTexture offTexture;

    public:
    cButton() {

    }
    void setParam(int _mx,int _my,int _mWidth,int _mHeight) {
        mx = _mx;
        my = _my;
        mWidth = _mWidth;
        mHeight = _mHeight;
    }
    void initTexture(SDL_Renderer *renderer,TTF_Font* font) {
        onTexture.setRendFont(renderer,font);
        offTexture.setRendFont(renderer,font);
    }
    bool loadTexture(const string& pathOn,const string& pathOff) {
        if (onTexture.loadTexture(pathOn) == false) {
            printf("Error loading on texture!\n");
            return false;
        }
        if (pathOff != "") {
            if (offTexture.loadTexture(pathOff) == false) {
                printf("Error loading off texture!\n");
                return false;
            }
        }

        return true;
    }

    int getX() {
        return mx;
    }
    int getY() {
        return my;
    }
    int getWidth() {
        return mWidth;
    }
    int getHeight() {
        return mHeight;
    }

    ~cButton() {
        free();
    }
    void free() {
        onTexture.free();
        offTexture.free();
    }

    void render(bool on) {
        SDL_Rect rect = {mx,my,mWidth,mHeight};

        if (on) {
            onTexture.render(&rect);
        }
        else {
            offTexture.render(&rect);
        }
    }

    bool isHovered() {
        int mouseX,mouseY;
        SDL_GetMouseState(&mouseX,&mouseY);

        if (mx <= mouseX && mouseX <= mx + mWidth &&
            my <= mouseY && mouseY <= my + mHeight) {
            return true;
        }

        return false;
    }

    bool isClicked(SDL_Event e) {
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX,mouseY;
            SDL_GetMouseState(&mouseX,&mouseY);

            if (mx <= mouseX && mouseX <= mx + mWidth &&
                my <= mouseY && mouseY <= my + mHeight) {
                return true;
            }
        }

        return false;
    }
};

class cardButtonClass {
    private:
    int mx,my,mWidth,mHeight,origX,origY;
    cTexture *cardTexture;
    SDL_Rect clip;

    public:
    cardButtonClass() {
        setParam(0,0,0,0);

        SDL_Rect _clip = {0,0,0,0};
        setTexture(NULL,_clip);
    }

    void setParam(int _mx,int _my,int _mWidth,int _mHeight) {
        mx = _mx;
        my = _my;
        mWidth = _mWidth;
        mHeight = _mHeight;
    }

    void setTexture(cTexture* _cardTexture,SDL_Rect _clip) {
        cardTexture = _cardTexture;
        clip = _clip;
    }

    ~cardButtonClass() {

    }

    int getX() {
        return mx;
    }
    int getY() {
        return my;
    }
    int getWidth() {
        return mWidth;
    }
    int getHeight() {
        return mHeight;
    }

    bool isHovered() {
        int mouseX,mouseY;
        SDL_GetMouseState(&mouseX,&mouseY);

        if (mx <= mouseX && mouseX <= mx + mWidth &&
            my <= mouseY && mouseY <= my + mHeight) {
            return true;
        }

        return false;
    }

    void render() {
        SDL_Rect rect = {mx,my,mWidth,mHeight};

        cardTexture->render(&rect,&clip);
    }
};

#endif // buttonClassGuard

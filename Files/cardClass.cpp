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
#include "buttonClass.cpp"
#include "cardEnums.cpp"

using namespace std;

#ifndef cardClassGuard
#define cardClassGuard

class cardClass {
    public:
    cardRankEnum cardRank;
    cardSuitEnum cardSuit;
    cardButtonClass button;

    void setCard(cardRankEnum _cardRank,cardSuitEnum _cardSuit) {
        cardRank = _cardRank;
        cardSuit = _cardSuit;
    }
    void setParam(int _mx,int _my,int _mWidth,int _mHeight) {
        button.setParam(_mx,_my,_mWidth,_mHeight);
    }
    void setTexture(cTexture* _cardTexture,SDL_Rect _clip) {
        button.setTexture(_cardTexture,_clip);
    }
    void render() {
        button.render();
    }
};

#endif // cardClassGuard

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
#include "cardClass.cpp"

using namespace std;

#ifndef gameStateClassGuard
#define gameStateClassGuard

struct gameStateClass {
    static const int cardsInDeck = 32;
    static const int cardsInHand = 4;

    cardClass deck[cardsInDeck + 5],cpuHand[cardsInHand + 5],playerHand[cardsInHand + 5],pile[2 * cardsInHand + 5],takenByPlayer[cardsInDeck + 5],takenByCPU[cardsInDeck + 5];
    int nrDeck,nrHandCPU,nrHandPlayer,nrPile,nrTakenPlayer,nrTakenCPU;

    gameStateClass() {

    }
    void init(cTexture* _cardTexture,const int textureCardWidth,const int textureCardHeight) {
        nrDeck = 32,nrHandCPU = 0,nrHandPlayer = 0, nrTakenPlayer = 0,nrTakenCPU = 0,nrPile = 0;

        SDL_Rect clip;
        cardRankEnum currentRank;
        cardSuitEnum currentSuit;

        // first row
        clip = {0,0,textureCardWidth,textureCardHeight};
        currentRank = cardRankEnum::ACE;
        currentSuit = cardSuitEnum::CLUBS;

        for (int i=1;i <= 8;++i) {
            deck[i].setCard(currentRank,currentSuit);
            deck[i].setTexture(_cardTexture,clip);

            currentRank = static_cast<cardRankEnum>(static_cast<int>(currentRank)+1);

            clip.x += textureCardWidth;
        }

        // second row
        clip = {0,textureCardHeight,textureCardWidth,textureCardHeight};
        currentRank = cardRankEnum::ACE;
        currentSuit = cardSuitEnum::SPADES;

        for (int i=9;i <= 16;++i) {
            deck[i].setCard(currentRank,currentSuit);
            deck[i].setTexture(_cardTexture,clip);

            currentRank = static_cast<cardRankEnum>(static_cast<int>(currentRank)+1);

            clip.x += textureCardWidth;
        }

        // third row
        clip = {0,2*textureCardHeight,textureCardWidth,textureCardHeight};
        currentRank = cardRankEnum::ACE;
        currentSuit = cardSuitEnum::HEARTS;

        for (int i=17;i <= 24;++i) {
            deck[i].setCard(currentRank,currentSuit);
            deck[i].setTexture(_cardTexture,clip);

            currentRank = static_cast<cardRankEnum>(static_cast<int>(currentRank)+1);

            clip.x += textureCardWidth;
        }

        // fourth row
        clip = {0,3*textureCardHeight,textureCardWidth,textureCardHeight};
        currentRank = cardRankEnum::ACE;
        currentSuit = cardSuitEnum::DIAMONDS;

        for (int i=25;i <= 32;++i) {
            deck[i].setCard(currentRank,currentSuit);
            deck[i].setTexture(_cardTexture,clip);

            currentRank = static_cast<cardRankEnum>(static_cast<int>(currentRank)+1);

            clip.x += textureCardWidth;
        }
    }
};

#endif // gameStateClassGuard

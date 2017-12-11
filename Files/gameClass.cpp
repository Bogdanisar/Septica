#include <iostream>
#include <cstdlib>
#include <time.h>
#include <cstdio>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>
#include <windows.h>
#include "textureClass.cpp"
#include "buttonClass.cpp"
#include "cardClass.cpp"
#include "gamestateClass.cpp"


using namespace std;

#ifndef gameClassGuard
#define gameClassGuard

#if 1
#define pv(x) cout<<#x<<" = "<<x<<"; ";cout.flush()
#define pn cout<<endl
#else
#define pv(x)
#define pn
#endif

class gameClass {

    enum renderStateEnum {
        stateMenu,
        stateGame,
        stateStats,
        stateCards
    };

    enum playerEnum {
        PLAYER,
        CPU,
        NA
    };

    SDL_Window *mWindow;
    SDL_Renderer *mRend;
    TTF_Font *mFont;

    static const int screenWidth = 800;
    static const int screenHeight = 500;
    static const int squareButtonSize = 100;
    static const int mainMenuButtonWidth = 288;
    static const int mainMenuButtonHeight = 60;
    static const int soundButtonWidth = squareButtonSize / 2;
    static const int soundButtonHeight = squareButtonSize / 2;
    static const int toolTipWidth = screenWidth, toolTipHeight = 35;
    static const int cardWidth = 72, cardHeight = 96;
    static const int gearSize = 100;
    static const int pileX = 100, pileY = 165, pileWidth = 600, pileHeight = 135;
    static const int soundVolume = MIX_MAX_VOLUME / 8;
    const double angleAdd;

    gameStateClass gameState;
    renderStateEnum renderState;

    cTexture cardsTexture,backOfCardTexture,gearTexture,xTexture,textTexture,blackTexture,frameTexture;
    cTexture gameBgTexture,cardsBgTexture;

    cButton musicButton,soundButton;
    cButton startGameButton,resumeGameButton,statsButton;
    cButton cardsButton,goBackFromGameButton,dealButton;
    cButton goBackFromCardsButton;
    cButton resetButton,goBackFromStatsButton;

    bool gameInProgress,gameIsOver,playerTurn,musicIsOn,soundIsOn;
    playerEnum gameWinner,roundStarter;
    int wins,losses,draws;
    double gearAngle;

    bool isLinked;
    cardClass linkedCard;
    int offX,offY;

    Mix_Chunk *clickSound,*cardSound;
    Mix_Music *mMusic;

    const string defaultText;
    string currentText;

    public:
    gameClass(): angleAdd(0.5),defaultText("Hover over a button to see tool tips!") {
        renderState = stateMenu;
        mWindow = NULL;
        mRend = NULL;
        mFont = NULL;
        clickSound = cardSound = NULL;
        mMusic = NULL;

        wins = losses = draws = 0;

        musicIsOn = false;
        soundIsOn = true;

        gameInProgress = gameIsOver = false;
        isLinked = false;

        currentText = defaultText;

        srand(time(0));
    }

    void play() {
        if (init() == false) {
            printf("Failed to initialize!\n");
        }
        else if (loadMedia() == false) {
            printf("Failed to load media!\n");
        }
        else {
            bool quit = false;
            SDL_Event e;

            Mix_PlayMusic(mMusic,-1);
            if (musicIsOn) {
                Mix_VolumeMusic(soundVolume);
            }
            else {
                Mix_VolumeMusic(0);
            }
            while (!quit) {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                        break;
                    }

                    switch(renderState) {
                        case renderStateEnum::stateMenu: {
                            handleEventMainMenu(e);

                            break;
                        }
                        case renderStateEnum::stateGame: {
                            handleEventGame(e);

                            break;
                        }
                        case renderStateEnum::stateCards: {
                            handleEventCards(e);

                            break;
                        }
                        case renderStateEnum::stateStats: {
                            handleEventStats(e);

                            break;
                        }
                    }
                }

                switch(renderState) {
                    case renderStateEnum::stateMenu: {
                        renderMenu();

                        break;
                    }
                    case renderStateEnum::stateGame: {
                        if (playerTurn) {
                            renderGame(true);
                        }
                        else {
                            doCPUTurn();
                        }

                        break;
                    }
                    case renderStateEnum::stateStats: {
                        renderStats();

                        break;
                    }
                    case renderStateEnum::stateCards: {
                        renderCards();

                        break;
                    }
                }

            }
        }

        quitSys();
    }

    private:
    void renderSoundButtons(bool canClick) {
        SDL_Rect clip;

        musicButton.render(canClick);
        if (!musicIsOn) {
            clip = {musicButton.getX(),musicButton.getY(),musicButton.getWidth(),musicButton.getHeight()};
            xTexture.render(&clip);
        }

        soundButton.render(canClick);
        if (!soundIsOn) {
            clip = {soundButton.getX(),soundButton.getY(),soundButton.getWidth(),soundButton.getHeight()};
            xTexture.render(&clip);
        }
    }

    void renderToolTip() {
        SDL_Color color = {0xFF,0xFF,0xFF,0xFF};
        if (textTexture.loadTexture(currentText,color) == false) {
            printf("Error loading text into textTexture!\n");
        }

        SDL_Rect viewport = {0,465,toolTipWidth,toolTipHeight};
        SDL_RenderSetViewport(mRend,&viewport);

        SDL_Rect clip;
        clip = {0,0,toolTipWidth,toolTipHeight};
        blackTexture.render(&clip);

        const int textWidth = textTexture.getWidth(), textHeight = 20;
        clip = {(toolTipWidth - textWidth)/2,(toolTipHeight - textHeight)/2,textWidth,textHeight};
        textTexture.render(&clip);

        SDL_RenderSetViewport(mRend,NULL);
    }

    void renderMenu() {
        SDL_SetRenderDrawColor(mRend, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(mRend);

        resumeGameButton.render(gameInProgress);
        startGameButton.render(true);
        statsButton.render(true);

        renderSoundButtons(true);
        renderToolTip();

        SDL_RenderPresent(mRend);
    }

    void renderGame(bool canClick) {
        SDL_SetRenderDrawColor(mRend,0xFF,0xFF,0xFF,0xFF);
        SDL_RenderClear(mRend);

        gameBgTexture.render(0,0);

        SDL_Rect clip;
        clip = {20,10,gearSize,gearSize};
        gearTexture.render(&clip,NULL,gearAngle);

        gearAngle += angleAdd;
        if (gearAngle > 360) {
            gearAngle -= 360;
        }

        cardsButton.render(canClick);
        goBackFromGameButton.render(canClick);

        {
            const int boxWidth = 330,boxHeight = 120,boxX = 235,boxY = 345;

            int x = boxX + (boxWidth - gameState.nrHandPlayer * cardWidth) / 2, y = boxY + 15;
            for (int i=1;i <= gameState.nrHandPlayer;++i) {
                gameState.playerHand[i].setParam(x,y,cardWidth,cardHeight);
                gameState.playerHand[i].render();
                x += cardWidth;
            }

            SDL_RenderSetViewport(mRend,NULL);
        }

        {
            const int portWidth = 330,portHeight = 120;
            clip = {235,0,portWidth,portHeight};
            SDL_RenderSetViewport(mRend,&clip);

            int x = (portWidth - gameState.nrHandCPU * cardWidth) / 2, y = 5;
            for (int i=1;i <= gameState.nrHandCPU;++i) {
                clip = {x,y,cardWidth,cardHeight};
                backOfCardTexture.render(&clip);
                x += cardWidth;
            }

            SDL_RenderSetViewport(mRend,NULL);
        }

        {
            clip = {pileX,pileY,pileWidth,pileHeight};
            SDL_RenderSetViewport(mRend,&clip);

            int x = (pileWidth - 2 * gameState.cardsInHand * cardWidth) / 2, y = (pileHeight - cardHeight) / 2;
            for (int i=1;i <= gameState.nrPile;++i) {
                gameState.pile[i].setParam(x,y,cardWidth,cardHeight);
                gameState.pile[i].render();
                x += cardWidth;
            }

            SDL_RenderSetViewport(mRend,NULL);
        }

        dealButton.render(canDeal() & canClick);

        renderSoundButtons(canClick);

        if (isLinked) {
            int mouseX,mouseY;
            SDL_GetMouseState(&mouseX,&mouseY);
            linkedCard.setParam(mouseX - offX,mouseY - offY,cardWidth,cardHeight);
            linkedCard.render();
        }

        if (!canClick) {
            currentText = "CPU turn!";
        }
        renderToolTip();

        SDL_RenderPresent(mRend);
    }

    void renderCards() {
        SDL_Rect clip;
        SDL_SetRenderDrawColor(mRend,0xFF,0xFF,0xFF,0xFF);
        SDL_RenderClear(mRend);

        clip = {0,0,screenWidth,screenHeight};
        cardsBgTexture.render(&clip);

        goBackFromCardsButton.render(true);

        {
            const int portX = 50,portY = 90,portWidth = 700,portHeight = 110;
            clip = {portX,portY,portWidth,portHeight};
            SDL_RenderSetViewport(mRend,&clip);

            int x = 19, y = (portHeight - cardHeight) / 2;
            for (int i=1;i <= gameState.nrTakenCPU;++i) {
                gameState.takenByCPU[i].setParam(x,y,cardWidth,cardHeight);
                gameState.takenByCPU[i].render();
                x += 19;
            }

            SDL_RenderSetViewport(mRend,NULL);
        }
        {
            const int portX = 50,portY = 240,portWidth = 700,portHeight = 110;
            clip = {portX,portY,portWidth,portHeight};
            SDL_RenderSetViewport(mRend,&clip);

            int x = 19, y = (portHeight - cardHeight) / 2;
            for (int i=1;i <= gameState.nrTakenPlayer;++i) {
                gameState.takenByPlayer[i].setParam(x,y,cardWidth,cardHeight);
                gameState.takenByPlayer[i].render();
                x += 19;
            }

            SDL_RenderSetViewport(mRend,NULL);
        }

        renderSoundButtons(true);
        renderToolTip();

        SDL_RenderPresent(mRend);
    }

    void renderStats() {
        SDL_SetRenderDrawColor(mRend,22,106,0,0xFF);
        SDL_RenderClear(mRend);

        const int frameWidth = 400, frameHeight = 250;
        SDL_Rect frameClip = {(screenWidth - frameWidth)/2,65,frameWidth,frameHeight};
        frameTexture.render(&frameClip);

        SDL_RenderSetViewport(mRend,&frameClip);
        SDL_Color color = {0xFF,0xFF,0xFF,0xFF};
        SDL_Rect textClip;
        int y = 55,currentHeight = 35,currentWidth;

        if (textTexture.loadTexture(concat("Wins: ",wins).c_str(),color) == false) {
            printf("Error loading text texture in stats menu!\n");
            return;
        }
        currentWidth = textTexture.getWidth() * currentHeight / textTexture.getHeight();
        textClip = {(frameWidth - currentWidth)/2, y, currentWidth, currentHeight};
        textTexture.render(&textClip);

        if (textTexture.loadTexture(concat("Losses: ",losses).c_str(),color) == false) {
            printf("Error loading text texture in stats menu!\n");
            return;
        }
        currentWidth = textTexture.getWidth() * currentHeight / textTexture.getHeight();
        textClip = {(frameWidth - currentWidth)/2, y + textTexture.getHeight(), currentWidth, currentHeight};
        textTexture.render(&textClip);

        if (textTexture.loadTexture(concat("Draws: ",draws).c_str(),color) == false) {
            printf("Error loading text texture in stats menu!\n");
            return;
        }
        currentWidth = textTexture.getWidth() * currentHeight / textTexture.getHeight();
        textClip = {(frameWidth - currentWidth)/2, y + 2 * textTexture.getHeight(), currentWidth, currentHeight};
        textTexture.render(&textClip);

        if (textTexture.loadTexture(concat("Winrate: ", wins, (wins + losses + draws) ).c_str(),color) == false) {
            printf("Error loading text texture in stats menu!\n");
            return;
        }
        currentWidth = textTexture.getWidth() * currentHeight / textTexture.getHeight();
        textClip = {(frameWidth - currentWidth)/2, y + 3 * textTexture.getHeight(), currentWidth, currentHeight};
        textTexture.render(&textClip);

        SDL_RenderSetViewport(mRend,NULL);

        resetButton.render(true);
        goBackFromStatsButton.render(true);

        renderSoundButtons(true);
        renderToolTip();

        SDL_RenderPresent(mRend);
    }

    string concat(const string& str,int nr) {
        stringstream ss;
        ss << str << nr;

        return ss.str();
    }
    string concat(const string& str,int num,int den) {
        if (den == 0) {
            return str + "N/A";
        }

        stringstream ss;
        ss.precision(5);
        ss << str << 100 * (double)num/den << "%";

        return ss.str();
    }

    void changeState(renderStateEnum newState) {
        renderState = newState;
        if (soundIsOn) {
            Mix_PlayChannel(-1,clickSound,0);
        }
    }

    bool canDeal() {
        if (gameState.nrPile == 0) {
            return false;
        }

        if (roundStarter == playerEnum::CPU && gameState.nrPile % 2 == 1) {
            return false;
        }

        return true;
    }

    void doCPUTurn() {
        if (getHandWinner() == playerEnum::CPU || gameState.nrHandCPU == 0) {
            playerTurn = true;
            return;
        }

        renderGame(false);

        const int maxFrames = 200, minFrames = 100;
        int timesToRender = rand() % (maxFrames - minFrames + 1) + minFrames;
        for (int i=1;i <= timesToRender;++i) {
            renderGame(false);
        }

        if (roundStarter == playerEnum::PLAYER) {
            int cardIndex = chooseBestCardCPU();
            placeCPUCard(cardIndex);
        }
        else {
            if (gameState.nrPile == 0) {
                int cardIndex = rand() % (gameState.nrHandCPU) + 1;
                placeCPUCard(cardIndex);
            }
            else {
                int cardIndex = getCuttingCardFromCPU();
                if (cardIndex == -1) {
                    // can't do anything; end turn;
                }
                else {
                    placeCPUCard(cardIndex);
                }
            }
        }

        playerTurn = true;
    }

    void goToNextTurn() {
        playerEnum handWinner = getHandWinner();
        if (handWinner == playerEnum::PLAYER) {
            for (int i=1;i <= gameState.nrPile;++i) {
                gameState.takenByPlayer[++gameState.nrTakenPlayer] = gameState.pile[i];
            }
        }
        else {
            for (int i=1;i <= gameState.nrPile;++i) {
                gameState.takenByCPU[++gameState.nrTakenCPU] = gameState.pile[i];
            }
        }
        gameState.nrPile = 0;

        if (gameState.nrDeck == 0 && gameState.nrHandPlayer + gameState.nrHandCPU == 0) {
            gameIsOver = true;
            gameWinner = getGameWinner();
            playerTurn = true;
        }
        else {
            dealCards();
            roundStarter = handWinner;
            if (roundStarter == playerEnum::PLAYER) {
                playerTurn = true;
            }
            else {
                playerTurn = false;
            }
        }
    }

    playerEnum getGameWinner() {
        int nrPointsPlayer = 0;
        for (int i=1;i <= gameState.nrTakenPlayer;++i) {
            if (isPoint(gameState.takenByPlayer[i].cardRank)) {
                ++nrPointsPlayer;
            }
        }

        int nrPointsCPU = 0;
        for (int i=1;i <= gameState.nrTakenCPU;++i) {
            if (isPoint(gameState.takenByCPU[i].cardRank)) {
                ++nrPointsCPU;
            }
        }

        if (nrPointsPlayer > nrPointsCPU) {
            return playerEnum::PLAYER;
        }
        if (nrPointsPlayer < nrPointsCPU) {
            return playerEnum::CPU;
        }

        return playerEnum::NA;
    }

    playerEnum getHandWinner() {
        if (gameState.nrPile == 0) {
            return playerEnum::NA;
        }

        for (int i=gameState.nrPile;i > 0;--i) {
            if (isBottomCard(gameState.pile[i].cardRank) || isCuttingCard(gameState.pile[i].cardRank)) {
                if (roundStarter == playerEnum::PLAYER) {
                    if (i % 2 == 1) {
                        return playerEnum::PLAYER;
                    }
                    return playerEnum::CPU;
                }
                else {
                    if (i % 2 == 1) {
                        return playerEnum::CPU;
                    }
                    return playerEnum::PLAYER;
                }
            }
        }

        return playerEnum::NA;
    }

    void placeCPUCard(int cardIndex) {
        gameState.pile[++gameState.nrPile] = gameState.cpuHand[cardIndex];
        swap(gameState.cpuHand[cardIndex],gameState.cpuHand[gameState.nrHandCPU--]);
    }

    int getCuttingCardFromCPU() {
        for (int i=1;i <= gameState.nrHandCPU;++i) {
            if (isBottomCard(gameState.cpuHand[i].cardRank)) {
                return i;
            }
        }

        for (int i=1;i <= gameState.nrHandCPU;++i) {
            if (isCuttingCard(gameState.cpuHand[i].cardRank)) {
                return i;
            }
        }

        return -1;
    }

    int chooseBestCardCPU() {
        for (int i=1;i <= gameState.nrHandCPU;++i) {
            if (isBottomCard(gameState.cpuHand[i].cardRank)) {
                return i;
            }
        }

        bool pointInPile = false;
        for (int i=1;i <= gameState.nrPile;++i) {
            if (isPoint(gameState.pile[i].cardRank)) {
                pointInPile = true;
            }
        }

        if (pointInPile) {
            for (int i=1;i <= gameState.nrHandCPU;++i) {
                if (isCuttingCard(gameState.cpuHand[i].cardRank)) {
                    return i;
                }
            }

            for (int i=1;i <= gameState.nrHandCPU;++i) {
                if (!isPoint(gameState.cpuHand[i].cardRank)) {
                    return i;
                }
            }
        }
        else {
            for (int i=1;i <= gameState.nrHandCPU;++i) {
                if (!isPoint(gameState.cpuHand[i].cardRank) && !isCuttingCard(gameState.cpuHand[i].cardRank)) {
                    return i;
                }
            }

            for (int i=1;i <= gameState.nrHandCPU;++i) {
                if (isPoint(gameState.cpuHand[i].cardRank)) {
                    return i;
                }
            }
        }

        return 1;
    }

    bool isPoint(cardRankEnum cardRank) {
        if (cardRank == cardRankEnum::ACE || cardRank == cardRankEnum::TEN) {
            return true;
        }

        return false;
    }

    bool isCuttingCard(cardRankEnum cardRank) {
        if (cardRank == cardRankEnum::SEVEN) {
            return true;
        }

        return false;
    }

    bool isBottomCard(cardRankEnum cardRank) {
        if (cardRank == gameState.pile[1].cardRank) {
            return true;
        }
        return false;
    }

    void handleEventSound(SDL_Event e) {

        if (musicButton.isClicked(e)) {
            if (musicIsOn) {
                musicIsOn = false;
                Mix_VolumeMusic(0);
            }
            else {
                musicIsOn = true;
                Mix_VolumeMusic(soundVolume);
            }
        }


        if (soundButton.isClicked(e)) {
            if (soundIsOn) {
                soundIsOn = false;
            }
            else {
                soundIsOn = true;
            }
        }
    }

    void handleEventMainMenu(SDL_Event e) {
        handleEventSound(e);

        if (musicButton.isHovered()) {
            currentText = "Click the music button to mute/unmute the music!";
        }
        else if (soundButton.isHovered()) {
            currentText = "Click the sound button to mute/unmute the sound effects!";
        }
        else if (resumeGameButton.isHovered()) {
            currentText = "Click the resume button to continue an unfinished game!";
        }
        else if (startGameButton.isHovered()) {
            currentText = "Click the start button to start a new game!";
        }
        else if (statsButton.isHovered()) {
            currentText = "Click the stats button to see this session's stats!";
        }
        else {
            currentText = defaultText;
        }

        if (gameInProgress && resumeGameButton.isClicked(e)) {
            changeState(renderStateEnum::stateGame);

        }
        else if (startGameButton.isClicked(e)) {
            changeState(renderStateEnum::stateGame);

            gameState = gameStateClass();
            gameState.init(&cardsTexture,cardWidth,cardHeight);

            shuffleCards();
            dealCards();

            gameInProgress = true;
            gameIsOver = false;
            gameWinner = playerEnum::NA;
            roundStarter = playerEnum::PLAYER;
            playerTurn = true;
            isLinked = false;
            gearAngle = 0;
        }
        else if (statsButton.isClicked(e)) {
            changeState(renderStateEnum::stateStats);
        }
    }

    void shuffleCards() {
        random_shuffle(gameState.deck + 1,gameState.deck + gameState.nrDeck + 1);
    }

    bool randomCompare(cardClass a,cardClass b) {
        return rand()%2;
    }

    void dealCards() {
        while (gameState.nrHandPlayer < 4 && gameState.nrDeck > 0) {
            gameState.playerHand[++gameState.nrHandPlayer] = gameState.deck[gameState.nrDeck--];
            gameState.cpuHand[++gameState.nrHandCPU] = gameState.deck[gameState.nrDeck--];
        }
    }

    void handleEventStats(SDL_Event e) {
        handleEventSound(e);

        if (musicButton.isHovered()) {
            currentText = "Click the music button to mute/unmute the music!";
        }
        else if (soundButton.isHovered()) {
            currentText = "Click the sound button to mute/unmute the sound effects!";
        }
        else if (resetButton.isHovered()) {
            currentText = "Click the reset button to reset this session's stats!";
        }
        else if (goBackFromStatsButton.isHovered()) {
            currentText = "Click the back button to go back to the main menu!";
        }
        else {
            currentText = defaultText;
        }

        if (resetButton.isClicked(e)) {
            if (soundIsOn) {
                Mix_PlayChannel(-1,clickSound,0);
            }

            wins = losses = draws = 0;
        }
        else if (goBackFromStatsButton.isClicked(e)) {
            changeState(renderStateEnum::stateMenu);
        }

    }

    void handleEventGame(SDL_Event e) {
        handleEventSound(e);

        currentText = "Player turn: ";

        bool hover = true;
        if (musicButton.isHovered()) {
            currentText += "Click the music button to mute/unmute the music!";
        }
        else if (soundButton.isHovered()) {
            currentText += "Click the sound button to mute/unmute the sound effects!";
        }
        else if (cardsButton.isHovered()) {
            currentText += "Click the cards button to see the cards taken by each player!";
        }
        else if (goBackFromGameButton.isHovered()) {
            currentText += "Click the back button to go back to the main menu!";
        }
        else if (dealButton.isHovered()) {
            currentText += "Click the button to deal a new hand!";
        }
        else {
            bool hoverCard = false;
            for (int i=1;i <= gameState.nrHandPlayer;++i) {
                if (gameState.playerHand[i].button.isHovered()) {
                    hoverCard = true;
                    if (canPlayRank(gameState.playerHand[i].cardRank)) {
                        currentText += "You can play that card. Drag it into the pile!";
                    }
                    else {
                        currentText += "You can't play that card!";
                    }
                    break;
                }
            }

            if (!hoverCard) {
                hover = false;
                currentText += defaultText;
            }
        }

        currentText += " Cards in deck: ";
        currentText = concat(currentText,gameState.nrDeck);

        if (hover == false && gameIsOver) {
            if (gameWinner == playerEnum::PLAYER) {
                currentText = "The player wins!";
            }
            else if (gameWinner == playerEnum::CPU) {
                currentText = "The CPU wins!";
            }
            else {
                currentText = "It's a draw!";
            }
        }

        if (goBackFromGameButton.isClicked(e)) {
            if (gameIsOver) {
                gameInProgress = false;

                switch (gameWinner) {
                    case playerEnum::PLAYER: {
                        ++wins;
                        break;
                    }
                    case playerEnum::CPU: {
                        ++losses;
                        break;
                    }
                    case playerEnum::NA: {
                        ++draws;
                        break;
                    }
                }
            }

            changeState(renderStateEnum::stateMenu);
        }
        else if (cardsButton.isClicked(e)) {
            changeState(renderStateEnum::stateCards);
        }
        else if (canDeal() && dealButton.isClicked(e)) {
            if (soundIsOn) {
                Mix_PlayChannel(-1,clickSound,0);
            }

            goToNextTurn();
        }
        else {
            if (e.type == SDL_MOUSEBUTTONDOWN && !isLinked) {
                for (int i=1;i <= gameState.nrHandPlayer;++i) {
                    int mouseX,mouseY;
                    SDL_GetMouseState(&mouseX,&mouseY);
                    cardButtonClass button = gameState.playerHand[i].button;

                    if (button.getX() <= mouseX && mouseX <= button.getX() + button.getWidth() &&
                        button.getY() <= mouseY && mouseY <= button.getY() + button.getHeight() &&
                        canPlayRank(gameState.playerHand[i].cardRank) ) {

                        isLinked = true;
                        offX = mouseX - button.getX();
                        offY = mouseY - button.getY();
                        linkedCard = gameState.playerHand[i];

                        for (int j=i+1;j <= gameState.nrHandPlayer;++j) {
                            gameState.playerHand[j-1] = gameState.playerHand[j];
                        }

                        --gameState.nrHandPlayer;
                        break;
                    }
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP && isLinked) {
                int mouseX,mouseY;
                SDL_GetMouseState(&mouseX,&mouseY);

                if (pileX <= mouseX && mouseX <= pileX + pileWidth &&
                    pileY <= mouseY && mouseY <= pileY + pileHeight) {

                    gameState.pile[++gameState.nrPile] = linkedCard;
                    isLinked = false;
                    playerTurn = false;
                }

                else {
                    gameState.playerHand[++gameState.nrHandPlayer] = linkedCard;
                    isLinked = false;
                }

                if (soundIsOn) {
                    Mix_PlayChannel(-1,cardSound,0);
                }
            }
        }
    }

    bool canPlayRank(cardRankEnum cardRank) {
        if (gameState.nrPile == 0) {
            return true;
        }

        if (roundStarter == playerEnum::PLAYER) {
            if (getHandWinner() == playerEnum::PLAYER) {
                return false;
            }
            else {
                if (isCuttingCard(cardRank) || isBottomCard(cardRank)) {
                    return true;
                }
                return false;
            }
        }
        else {
            if (gameState.nrPile % 2 == 1) {
                return true;
            }
            return false;
        }
    }

    void handleEventCards(SDL_Event e) {
        handleEventSound(e);

        if (musicButton.isHovered()) {
            currentText = "Click the music button to mute/unmute the music!";
        }
        else if (soundButton.isHovered()) {
            currentText = "Click the sound button to mute/unmute the sound effects!";
        }
        else if (goBackFromCardsButton.isHovered()) {
            currentText = "Click the back button to go back to the game!";
        }
        else {
            currentText = defaultText;
        }

        if (goBackFromCardsButton.isClicked(e)) {
            changeState(renderStateEnum::stateGame);
        }
    }


    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            printf("Error initializing SDL VIDEO: %s\n",SDL_GetError());
            return false;
        }

        int imgFlags = IMG_INIT_PNG;
        if (IMG_Init(imgFlags) != imgFlags) {
            printf("Error initializing SDL IMG: %s\n",IMG_GetError());
            return false;
        }

        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,2048) < 0) {
            printf("Error initializing SDL Mixer: %s\n",Mix_GetError());
            return false;
        }

        if (TTF_Init() == -1) {
            printf("Error initializing TTF: %s\n",TTF_GetError());
            return false;
        }

        if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1") == SDL_FALSE) {
            printf("Error setting hint: %s\n",SDL_GetError());
            return false;
        }

        mWindow = SDL_CreateWindow("Septica", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        if (mWindow == NULL) {
            printf("Error creating window: %s\n",SDL_GetError());
            return false;
        }

        mRend = SDL_CreateRenderer(mWindow,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (mRend == NULL) {
            printf("Error creating renderer: %s\n",SDL_GetError());
            return false;
        }

        SDL_SetRenderDrawColor(mRend,0xFF,0xFF,0xff,0xff);
        SDL_RenderClear(mRend);

        return true;
    }

    bool loadMedia() {
        string path;

        path = "sounds/Button-SoundBible.com-1420500901.wav";
        clickSound = Mix_LoadWAV(path.c_str());
        if (clickSound == NULL) {
            printf("Error loading click sound at %s: %s\n",path.c_str(),Mix_GetError());
            return false;
        }
        path = "sounds/240777__f4ngy__dealing-card.wav";
        cardSound = Mix_LoadWAV(path.c_str());
        if (cardSound == NULL) {
            printf("Error loading click sound at %s: %s\n",path.c_str(),Mix_GetError());
            return false;
        }
        path = "sounds/Blue_Dot_Sessions_-_01_-_Lord_Weasel.mp3";
        mMusic = Mix_LoadMUS(path.c_str());
        if (mMusic == NULL) {
            printf("Error loading music at %s: %s\n",path.c_str(),Mix_GetError());
            return false;
        }
        Mix_VolumeChunk(clickSound,soundVolume);
        Mix_VolumeChunk(cardSound,soundVolume);

        path = "font/OpenSans-Bold.ttf";
        mFont = TTF_OpenFont(path.c_str(),16);
        if (mFont == NULL) {
            printf("Error loading font at %s: %s\n",path.c_str(),TTF_GetError());
            return false;
        }

        gameBgTexture.setRendFont(mRend,mFont);
        cardsBgTexture.setRendFont(mRend,mFont);

        cardsTexture.setRendFont(mRend,mFont);
        backOfCardTexture.setRendFont(mRend,mFont);
        gearTexture.setRendFont(mRend,mFont);
        xTexture.setRendFont(mRend,mFont);
        textTexture.setRendFont(mRend,mFont);
        blackTexture.setRendFont(mRend,mFont);
        frameTexture.setRendFont(mRend,mFont);

        startGameButton.initTexture(mRend,mFont);
        resumeGameButton.initTexture(mRend,mFont);
        statsButton.initTexture(mRend,mFont);
        musicButton.initTexture(mRend,mFont);
        soundButton.initTexture(mRend,mFont);
        cardsButton.initTexture(mRend,mFont);
        goBackFromGameButton.initTexture(mRend,mFont);
        dealButton.initTexture(mRend,mFont);
        goBackFromCardsButton.initTexture(mRend,mFont);
        resetButton.initTexture(mRend,mFont);
        goBackFromStatsButton.initTexture(mRend,mFont);


        if (gameBgTexture.loadTexture("images/gameBg.png") == false) {
            printf("Error loading game background texture!\n");
            return false;
        }
        if (cardsBgTexture.loadTexture("images/cardsBg.png") == false) {
            printf("Error loading cards background texture!\n");
            return false;
        }

        if (cardsTexture.loadTexture("images/cards2.png") == false) {
            printf("Error loading cards sheet texture!\n");
            return false;
        }
        if (backOfCardTexture.loadTexture("images/cardBack.png") == false) {
            printf("Error loading back of card texture!\n");
            return false;
        }
        if (gearTexture.loadTexture("images/gear.png") == false) {
            printf("Error loading gear texture!\n");
            return false;
        }
        if (xTexture.loadTexture("images/X.png") == false) {
            printf("Error loading x texture!\n");
            return false;
        }
        if (blackTexture.loadTexture("images/black.png") == false) {
            printf("Error loading black texture!\n");
            return false;
        }
        if (frameTexture.loadTexture("images/blackFrame.png") == false) {
            printf("Error loading stats frame texture!\n");
            return false;
        }


        if (startGameButton.loadTexture("images/startOn.png","") == false) {
            printf("Error loading start game button texture!\n");
            return false;
        }
        if (resumeGameButton.loadTexture("images/resumeOn.png","images/resumeOff.png") == false) {
            printf("Error loading resume game button texture!\n");
            return false;
        }
        if (statsButton.loadTexture("images/statsOn.png","") == false) {
            printf("Error loading stats game button texture!\n");
            return false;
        }
        if (musicButton.loadTexture("images/musicOn.png","images/musicOff.png") == false) {
            printf("Error loading music button texture!\n");
            return false;
        }
        if (soundButton.loadTexture("images/speakerOn.png","images/speakerOff.png") == false) {
            printf("Error loading sound button texture!\n");
            return false;
        }
        if (cardsButton.loadTexture("images/cardOn.png","images/cardOff.png") == false) {
            printf("Error loading cards button texture!\n");
            return false;
        }
        if (goBackFromGameButton.loadTexture("images/backOn.png","images/backOff.png") == false) {
            printf("Error loading go back from game button texture!\n");
            return false;
        }
        if (dealButton.loadTexture("images/dealOn.png","images/dealOff.png") == false) {
            printf("Error loading deal new cards button texture!\n");
            return false;
        }
        if (goBackFromCardsButton.loadTexture("images/backFromCardsOn.png","") == false) {
            printf("Error loading go back from cards button texture!\n");
            return false;
        }
        if (resetButton.loadTexture("images/reset.png","") == false) {
            printf("Error loading reset button texture!\n");
            return false;
        }
        if (goBackFromStatsButton.loadTexture("images/backOn.png","") == false) {
            printf("Error loading go back from stats button texture!\n");
            return false;
        }

        {
            int y = (screenHeight - 3*mainMenuButtonHeight) / 2;
            resumeGameButton.setParam((screenWidth - mainMenuButtonWidth) / 2, y, mainMenuButtonWidth, mainMenuButtonHeight);
            startGameButton.setParam((screenWidth - mainMenuButtonWidth) / 2, y + mainMenuButtonHeight, mainMenuButtonWidth, mainMenuButtonHeight);
            statsButton.setParam((screenWidth - mainMenuButtonWidth) / 2, y + 2 * mainMenuButtonHeight, mainMenuButtonWidth, mainMenuButtonHeight);
        }

        {
            int x = 680,y = 20,dif = 0;
            musicButton.setParam(x,y,soundButtonWidth,soundButtonHeight);
            soundButton.setParam(x + soundButtonWidth + dif,y,soundButtonWidth,soundButtonHeight);
        }

        {
            int x = 300,y = 340;
            resetButton.setParam(x,y,squareButtonSize,squareButtonSize);
            goBackFromStatsButton.setParam(x + 100,y,squareButtonSize,squareButtonSize);
        }

        {
            int x = 20,y = 355;
            cardsButton.setParam(x,y,squareButtonSize,squareButtonSize);
        }
        {
            int x = 680,y = 355;
            goBackFromGameButton.setParam(x,y,squareButtonSize,squareButtonSize);
        }
        {
            int x = 455,y = 320, width = 110,height = 25;
            dealButton.setParam(x,y,width,height);
        }

        {
            int width = 340, height = 70;
            int x = (screenWidth - width) / 2, y = 372;
            goBackFromCardsButton.setParam(x,y,width,height);
        }

        return true;
    }

    void quitSys() {
        cardsTexture.free();
        backOfCardTexture.free();
        gearTexture.free();
        xTexture.free();
        textTexture.free();
        blackTexture.free();
        frameTexture.free();

        gameBgTexture.free();
        cardsBgTexture.free();

        musicButton.free();
        soundButton.free();

        startGameButton.free();
        resumeGameButton.free();
        statsButton.free();

        cardsButton.free();
        goBackFromGameButton.free();
        dealButton.free();

        goBackFromCardsButton.free();

        resetButton.free();
        goBackFromStatsButton.free();


        Mix_FreeChunk(clickSound);
        Mix_FreeChunk(cardSound);
        Mix_FreeMusic(mMusic);
        clickSound = NULL;
        cardSound = NULL;
        mMusic = NULL;


        SDL_DestroyRenderer(mRend);
        SDL_DestroyWindow(mWindow);
        TTF_CloseFont(mFont);
        mRend = NULL;
        mWindow = NULL;
        mFont = NULL;

        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
};

#endif // gameClassGuard

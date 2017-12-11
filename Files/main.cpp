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
#include "gamestateClass.cpp"
#include "gameClass.cpp"

using namespace std;

int main(int argc,char** argv) {
    gameClass game;
    game.play();

    return 0;
}

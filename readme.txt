Septica is a romanian card game that needs at least 2 players.
Using this program, you can play a 1v1 game against an AI.
The game is implemented in C++ using the SDL 2.0 API in Windows.

In order to run the game follow the next steps:
1) Download the Files folder.
2) Run the compile.bat file or navigate to the folder using CMD and run the following commands only for the first time:

mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c gameClass.cpp -o obj\gameClass.o
mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c main.cpp -o obj\main.o
mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c gameStateClass.cpp -o obj\gameStateClass.o
mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c cardClass.cpp -o obj\cardClass.o
mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c buttonClass.cpp -o obj\buttonClass.o
mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c cardClass.cpp -o obj\cardClass.o
mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c cardEnums.cpp -o obj\cardEnums.o
mingw32-g++.exe -g -std=c++11 -ISDL\Mingw_dev_lib\include\SDL2 -c textureClass.cpp -o obj\textureClass.o
mingw32-g++.exe -LSDL\Mingw_dev_lib\lib -o septica.exe obj\buttonClass.o obj\cardClass.o obj\cardEnums.o obj\gameClass.o obj\gameStateClass.o obj\main.o obj\textureClass.o  -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

3) Run septica.exe

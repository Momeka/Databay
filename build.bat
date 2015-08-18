@echo off

pushd \code 

lcc -Wa-l -c -o splash.o splash.c
lcc -Wa-l -c -o main.o main.c
lcc -Wa-l -c -o gameData.o gameData.s
lcc -Wa-l -c -o game.o game.c
lcc -Wa-l -c -o gameOver.o gameOver.c
lcc -Wa-l -c -o titleData.o titleData.s
lcc -Wa-l -c -o title.o title.c
lcc -Wa-l -c -o utilities.o utilities.c

lcc -Wl-m -Wl-yt1 -Wl-yo2 -o ..\databay.gb main.o gameData.o game.o utilities.o splash.o gameOver.o title.o titleData.o

del *.o
del *.lst
del *.map

popd

echo.
echo Build Complete
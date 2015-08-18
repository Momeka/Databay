#include <gb/gb.h>
#include "Game.h"
#include "Title.h"
#include "Splash.h"
#include "GameOver.h"

#define STATE_SPLASH 	1
#define STATE_MENU 		2
#define STATE_GAME 		3
#define STATE_GAMEOVER 	4

void main(void)
{
	unsigned UBYTE gameState;

	gameState = STATE_SPLASH;
	while(1)
	{
		switch(gameState)
		{
			case STATE_SPLASH: 	 gameState = Run_splash(); break;
			case STATE_MENU: 	 gameState = Run_title(); break;
			case STATE_GAME: 	 gameState = Run_game(); break;
			case STATE_GAMEOVER: gameState = Run_gameover(); break;
		}
	}
}
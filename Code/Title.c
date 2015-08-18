#include <gb/gb.h>
#include "Title.h"
#include "Utilities.h"

extern unsigned char titleData[];
extern unsigned char titleTiles[];
extern unsigned char pressStart[];
extern unsigned char pressStartBlank[];
extern unsigned char colorEffect[];

UBYTE cnt = 0;
UBYTE pressStartCount = 0;

void Fade_in_title(void);
void Fade_out_title(void);

void lcd(void)
{
	cnt++;
	BGP_REG = colorEffect[cnt>>3];
	if(cnt > 99) cnt = 0;
}

void Initialize_title(void)
{
	HIDE_SPRITES;
	HIDE_BKG;
	SPRITES_8x16;

	STAT_REG |= 1 << 3;
	BGP_REG = 0xC4;
 	SCX_REG = 0;
 	cnt = 80;

	set_bkg_data(0, 103, &titleData);
	set_bkg_tiles(0, 0, 20, 18, &titleTiles);

	SHOW_BKG;
	SHOW_SPRITES;
}

UBYTE Run_title(void)
{
	Initialize_title();
	Fade_in_title();
	Assign_VBLANK(lcd);

	while(1)
	{
		Poll_joypad();

		pressStartCount++;
		if(pressStartCount == 35)
		{
			set_bkg_tiles(6, 13, 8, 1, &pressStartBlank);
		}
		else if(pressStartCount == 50)
		{
			set_bkg_tiles(6, 13, 8, 1, &pressStart);
			pressStartCount = 0;
		}

		if(Is_new_key_down(J_START | J_A | J_B))
		{
			Disable_VBLANK();
			Fade_out_title();
			return 3;
		}

		wait_vbl_done();

	}

	return 3;
}

void Fade_in_title(void)
{
	UBYTE count = 0;
	UBYTE stage = 0;
	while(1)
	{
		switch(stage)
		{
			case 0: BGP_REG = 0x00; break;
			case 1: BGP_REG = 0x40; break;
			case 2: BGP_REG = 0x88; break;
			case 3: BGP_REG = 0xC4; break;
		}
		stage++;

		if(stage == 4)
		{
			break;
		}

		delay(150);
		wait_vbl_done();
	}
}

void Fade_out_title(void)
{
	UBYTE count = 0;
	UBYTE stage = 0;
	while(1)
	{
		switch(stage)
		{
			case 0: BGP_REG = 0xC4; break;
			case 1: BGP_REG = 0x88; break;
			case 2: BGP_REG = 0x40; break;
			case 3: BGP_REG = 0x00; break;
		}
		stage++;

		if(stage == 4)
		{
			break;
		}

		delay(150);
		wait_vbl_done();
	}
}
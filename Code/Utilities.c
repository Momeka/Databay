#include <gb/gb.h>
#include "Utilities.h"

UBYTE fistTimeAddingVBLANK = 0;
void (*VBLANKListener)();

void Reset_sprites(void)
{
 	UBYTE i = 0;
 	for(i = 0; i != 40; i++)
	{
		move_sprite(i, 0, 0);
		set_sprite_prop(i, 0x00U);
	}
}

void Assign_VBLANK(void (*listener)())
{
	disable_interrupts();
	VBLANKListener = listener;
	if(fistTimeAddingVBLANK == 0)
	{
		add_LCD(VBLANKListener);
		fistTimeAddingVBLANK = 1;
	}
	enable_interrupts();
	set_interrupts(VBL_IFLAG | LCD_IFLAG);

}

void EmptyLcd(void) {}
void Disable_VBLANK(void)
{
	disable_interrupts();
	VBLANKListener = EmptyLcd;
	enable_interrupts();
	set_interrupts(VBL_IFLAG);
}

void Poll_joypad(void)
{
	previousJoy = currentJoy;
	currentJoy = joypad();
}

UBYTE Is_key_down(UBYTE key)
{
	return currentJoy & key;
}

UBYTE Is_key_up(UBYTE key)
{
	return !(currentJoy & key);	
}

UBYTE Is_new_key_down(UBYTE key)
{
	return (currentJoy & key) && !(previousJoy & key);	
}

UBYTE Is_new_key_up(UBYTE key)
{
	return !(currentJoy & key) && (previousJoy & key);		
}

void Fade_out(void)
{
	UBYTE count = 0;
	UBYTE stage = 0;
	while(1)
	{
		switch(stage)
		{
			case 0: BGP_REG = 0xE4; break;
			case 1: BGP_REG = 0x90; break;
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

void Fade_in(void)
{
	UBYTE count = 0;
	UBYTE stage = 0;
	while(1)
	{
		switch(stage)
		{
			case 0: BGP_REG = 0x00; break;
			case 1: BGP_REG = 0x40; break;
			case 2: BGP_REG = 0x90; break;
			case 3: BGP_REG = 0xE4; break;
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
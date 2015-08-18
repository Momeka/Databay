////////////////////
// functions
////////////////////
void Reset_sprites(void);
void Assign_VBLANK(void (*listener)());
void Disable_VBLANK(void);
void Fade_out(void);
void Fade_in(void);
void Poll_joypad(void);
UBYTE Is_key_down(UBYTE key);
UBYTE Is_key_up(UBYTE key);
UBYTE Is_new_key_down(UBYTE key);
UBYTE Is_new_key_up(UBYTE key);

////////////////////
// variables
////////////////////
UBYTE currentJoy 		= 0;
UBYTE previousJoy 		= 0;
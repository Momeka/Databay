#include <gb/gb.h>
#include "Game.h"
#include "Utilities.h"
#include "Level.h"

#define MAX_X               140
#define MIN_X               20
#define OBSTACLE_LIMIT_ONE  112
#define OBSTACLE_LIMIT_TWO  124
#define MAX_OBSTACLES       9
#define LEVEL_UPDATE_RATE   40
#define SCORE_TICK          30

extern unsigned char mapTiles[];
extern unsigned char gameData[];
extern unsigned char data_road_1[];
extern unsigned char data_road_2[];
extern unsigned char spriteData[];

const unsigned char mapScroll[] = {
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
};

UBYTE animationRate = 0;
UBYTE animationCount = 0;
UBYTE playerRate = 0;
UBYTE playerCount = 0;
UBYTE paused = 0;
UWORD levelIndex = 0;
UBYTE levelRate = 0;
UBYTE scoreRate = 0;
Obstacle obstacles[MAX_OBSTACLES];
Score score = {0, 0, 0, 0};
Player player;

void Update_animation(void);
void Update_player(void);
void Update_obstacles(void);
void Update_level(void);
void Add_obstacles(UBYTE type, UBYTE x, UBYTE y);
void Add_score(UBYTE thousandth, UBYTE hundredth, UBYTE tenth, UBYTE single);

void Initialize_game(void)
{
  UBYTE index;

	HIDE_SPRITES;
	HIDE_BKG;
	SPRITES_8x16;

	OBP1_REG = OBP0_REG = 0xC6;
 	SCX_REG = 24;

	set_bkg_data(0, 48, &gameData);
	set_bkg_tiles(0, 0, 26, 18, &mapTiles);
	set_sprite_data(0, 48, &spriteData);

	//Set up player
  player.x = 80;
  player.y = 127;
  player.life = 3;
  player.hitbox.x = player.x - 10;
  player.hitbox.y = player.y;
  player.hitbox.xMax = player.x + 10;
  player.hitbox.yMax = player.y + 3;
  player.isInvulnerable = 0;
  player.invulnerableCount = 0;
  player.invAnimRate = 0;
	set_sprite_tile(0, 2);
	set_sprite_tile(1, 4);
  set_sprite_tile(2, 6);
	set_sprite_tile(3, 8);
	set_sprite_tile(4, 4);
	set_sprite_tile(5, 2);
	set_sprite_tile(6, 8);
	set_sprite_tile(7, 6);
	set_sprite_prop(4, S_FLIPX);
	set_sprite_prop(5, S_FLIPX);
	set_sprite_prop(6, S_FLIPX);
	set_sprite_prop(7, S_FLIPX);

  //set up obstacle array
  for(index = 0; index != MAX_OBSTACLES; index++)
  {
    obstacles[index].isActive     = 0;
    obstacles[index].type         = 0;
    obstacles[index].x            = 0;
    obstacles[index].y            = 0;
    obstacles[index].speed        = 0;
    obstacles[index].moveRate     = 0;
    obstacles[index].hitbox.x     = 0;
    obstacles[index].hitbox.y     = 0;
    obstacles[index].hitbox.xMax  = 0;
    obstacles[index].hitbox.yMax  = 0;
  }

  //Set up UI
  set_sprite_tile(39, 20);
  move_sprite(39, 11, 19);
  scoreRate = 0;
  score.single = 0;
  score.tenth = 0;
  score.hundredth = 0;
  score.thousandth = 0;
  set_sprite_tile(38, 28);
  set_sprite_tile(37, 28);
  set_sprite_tile(36, 28);
  set_sprite_tile(35, 28);
  move_sprite(38, 21, 19);
  move_sprite(37, 28, 19);
  move_sprite(36, 35, 19);
  move_sprite(35, 42, 19);

  paused = 0;
  levelIndex = 0;
  levelRate = 0;

	SHOW_BKG;
	SHOW_SPRITES;
}

UBYTE Run_game(void)
{
	Initialize_game();
	Fade_in();

	while(1)
	{
    Poll_joypad();

    //Handle pause
    if(Is_new_key_down(J_START))
    {
      paused ^= 1 << 0;
    }

    if(paused == 1)
    {
      if(Is_new_key_down(J_SELECT))
      {
        Reset_sprites();
        Fade_out();
        return 2;
      }

      wait_vbl_done();
      continue;
    }

    //Check if it's game over
    if(player.life == 0)
    {
      Reset_sprites();
      Fade_out();
      return 4;
    }

    //Update game
  	animationRate++;
  	if(animationRate == 5)
 	 	{
    		Update_animation();
    		animationRate = 0;
  	}

    scoreRate++;
    if(scoreRate == SCORE_TICK)
    {
      Add_score(0, 0, 0, 1);
      scoreRate = 0;
    }

    Update_level();
  	Update_player();
  	Update_obstacles();

  	wait_vbl_done();
	}

  Reset_sprites();
  Fade_out();
	return 2;
}

void Update_level(void)
{
  UBYTE index = 0;
  levelRate++;
  if(levelRate == LEVEL_UPDATE_RATE)
  {
    for(index = 0; index != 5; index++)
    {
      if(level[levelIndex + index] == 0x00)
      {
        continue;
      }

      if(level[levelIndex + index] == 0x01)
      {
        switch(index)
        {
          case 0: Add_obstacles(1, 15, 104); break;
          case 1: Add_obstacles(1, 47, 104); break;
          case 2: Add_obstacles(1, 80, 104); break;
          case 3: Add_obstacles(1, 112, 104); break;
          case 4: Add_obstacles(1, 143, 104); break;
        }
      }
    }

    levelIndex += 5;
    if(levelIndex == 310)
    {
      Add_score(0, 0, 2, 5);
      levelIndex = 0;
    }
    levelRate = 0;
  }
}

void Add_obstacles(UBYTE type, UBYTE x, UBYTE y)
{
  UBYTE index;
  for(index = 0; index != MAX_OBSTACLES; index++)
  {
    if(obstacles[index].isActive == 1)
    {
      continue;
    }

    if(type == 1)
    {
      obstacles[index].isActive     = 1;
      obstacles[index].type         = 1;
      obstacles[index].x            = x;
      obstacles[index].y            = y;
      obstacles[index].speed        = 1;
      obstacles[index].moveRate     = 0;
      obstacles[index].hitbox.x     =  obstacles[index].x - 4;
      obstacles[index].hitbox.xMax  =  obstacles[index].x + 4;
      obstacles[index].hitbox.y     =  obstacles[index].y - 4;
      obstacles[index].hitbox.yMax  =  obstacles[index].y;
      set_sprite_tile(8 + (index << 2), 14);
      set_sprite_tile(9 + (index << 2), 14);
      set_sprite_prop(9 + (index << 2), S_FLIPX);
      move_sprite(8 + (index << 2), obstacles[index].x,     obstacles[index].y);
      move_sprite(9 + (index << 2), obstacles[index].x + 8, obstacles[index].y);
      return;
    }
  }
}

void Update_obstacles(void)
{
  UBYTE index;
  for(index = 0; index != MAX_OBSTACLES; index++)
  {
    if(obstacles[index].isActive == 0)
    {
      continue;
    }  

    if(obstacles[index].y > 160)
    {
      obstacles[index].isActive = 0;
      set_sprite_tile(8 + (index << 2), 0);
      set_sprite_tile(9 + (index << 2), 0);
    }

    if(obstacles[index].y < OBSTACLE_LIMIT_ONE)
    {
      obstacles[index].moveRate++;
      if(obstacles[index].moveRate == 2)
      {
        obstacles[index].y += obstacles[index].speed;
        obstacles[index].moveRate = 0;
      }
    }
    else
    {
      obstacles[index].y += obstacles[index].speed;
    }

    move_sprite(8 + (index << 2), obstacles[index].x, obstacles[index].y);
    move_sprite(9 + (index << 2), obstacles[index].x + 8, obstacles[index].y);


    obstacles[index].hitbox.x    =  obstacles[index].x - 4;
    obstacles[index].hitbox.xMax =  obstacles[index].x + 4;
    obstacles[index].hitbox.y    =  obstacles[index].y - 4;
    obstacles[index].hitbox.yMax =  obstacles[index].y;

    
    if(obstacles[index].y == OBSTACLE_LIMIT_ONE)
    {
      set_sprite_tile(8 + (index << 2), 16);
      set_sprite_tile(9 + (index << 2), 16);
    }
    else if(obstacles[index].y == OBSTACLE_LIMIT_TWO)
    {
      set_sprite_tile(8 + (index << 2), 18);
      set_sprite_tile(9 + (index << 2), 18);
    }

    if(Hitbox_intersects(&player.hitbox, &obstacles[index].hitbox) == 1)
    {
      obstacles[index].isActive = 0;    
      set_sprite_tile(8 + (index << 2), 0);
      set_sprite_tile(9 + (index << 2), 0);

      if(player.isInvulnerable == 0)
      {
        player.isInvulnerable = 1;
        player.life -= 1;
        switch(player.life)
        {
          case 3: set_sprite_tile(39, 20); break;
          case 2: set_sprite_tile(39, 22); break;
          case 1: set_sprite_tile(39, 24); break;
          case 0: set_sprite_tile(39, 26); break;
        }
      }
    }
  }
}

void Update_player(void)
{
  UBYTE mapscroll = 0;

	//Update animations
	playerRate++;

  if(playerRate % 3 == 0)
  {
    if(get_sprite_tile(2) == 6)
    {
      set_sprite_tile(2, 10);
      set_sprite_tile(3, 12);
      set_sprite_tile(6, 12);
      set_sprite_tile(7, 10);
    }
    else
    {
      set_sprite_tile(2, 6);
      set_sprite_tile(3, 8);
      set_sprite_tile(6, 8);
      set_sprite_tile(7, 6);
    }
  }

  if(playerRate == 10)
  {
  	if(playerCount == 1)
  	{
  		player.y += 1;
  		playerCount = 0;
  	}
  	else if(playerCount == 0)
  	{
  		player.y -= 1;
  		playerCount = 1;
  	}

   	playerRate = 0;
  }

  //Update Invulnerable animation
  if(player.isInvulnerable == 1)
  {
    player.invAnimRate++;
    if(player.invAnimRate == 3)
    {
      player.invulnerableCount++;
      if(player.invulnerableCount % 2 == 0)
      {
        set_sprite_tile(0, 0);
        set_sprite_tile(1, 0);
        set_sprite_tile(2, 0);
        set_sprite_tile(3, 0);
        set_sprite_tile(4, 0);
        set_sprite_tile(5, 0);
        set_sprite_tile(6, 0);
        set_sprite_tile(7, 0);
      }
      else
      {
        set_sprite_tile(0, 2);
        set_sprite_tile(1, 4);
        set_sprite_tile(2, 6);
        set_sprite_tile(3, 8);
        set_sprite_tile(4, 4);
        set_sprite_tile(5, 2);
        set_sprite_tile(6, 8);
        set_sprite_tile(7, 6);
      }
      player.invAnimRate = 0;
    }

    if(player.invulnerableCount == 30)
    {
      set_sprite_tile(0, 2);
      set_sprite_tile(1, 4);
      set_sprite_tile(2, 6);
      set_sprite_tile(3, 8);
      set_sprite_tile(4, 4);
      set_sprite_tile(5, 2);
      set_sprite_tile(6, 8);
      set_sprite_tile(7, 6);

      player.isInvulnerable = 0;
      player.invAnimRate = 0;
      player.invulnerableCount = 0;
    }
  }

  //Handle input
  if(Is_key_down(J_RIGHT))
  {
    player.x += 2;

    if(player.x > MAX_X)
    {
      player.x = MAX_X;
    }
  }
  if(Is_key_down(J_LEFT))
  {
    player.x -= 2;
    if(player.x < MIN_X)
    {
      player.x = MIN_X;
    }
  }

  //Update background parallax
  mapscroll = (player.x / 24) + 10;
  SCX_REG = mapScroll[mapscroll];

  //Update players hitbox
  player.hitbox.x = player.x - 14;
  player.hitbox.xMax = player.x + 14;

  //Update sprite positions
  move_sprite(0, player.x - 8,    player.y);
  move_sprite(1, player.x,        player.y);
  move_sprite(2, player.x - 8,    player.y + 16);
  move_sprite(3, player.x,        player.y + 16);
  move_sprite(4, player.x + 8,    player.y);
  move_sprite(5, player.x + 16,   player.y);
  move_sprite(6, player.x + 8,    player.y + 16);
  move_sprite(7, player.x + 16,   player.y + 16);
}

void Update_animation(void)
{ 
  if(animationCount == 0)
  {
    set_bkg_data(41, 5, &data_road_2);
    animationCount = 1;
  }
  else
  {
    set_bkg_data(41, 5, &data_road_1);
    animationCount = 0;
  }
}

void Add_score(UBYTE thousandth, UBYTE hundredth, UBYTE tenth, UBYTE single)
{
  score.single += single;
  while(score.single >= 10)
  {
    score.single -= 10;
    score.tenth += 1;
  }

  score.tenth += tenth;
  while(score.tenth >= 10)
  {
    score.tenth -= 10;
    score.hundredth += 1;
  }

  score.hundredth += hundredth;
  while(score.hundredth >= 10)
  {
    hundredth -= 10;
    thousandth += 1;
  }

  score.thousandth += thousandth;
  if(score.thousandth >= 10)
  {
    score.single = 9;
    score.tenth = 9;
    score.hundredth = 9;
    score.thousandth = 9;
  }

  //Update score sprites
  switch(score.single)
  {
    case 0: set_sprite_tile(35, 28); break;
    case 1: set_sprite_tile(35, 30); break;
    case 2: set_sprite_tile(35, 32); break;
    case 3: set_sprite_tile(35, 34); break;
    case 4: set_sprite_tile(35, 36); break;
    case 5: set_sprite_tile(35, 38); break;
    case 6: set_sprite_tile(35, 40); break;
    case 7: set_sprite_tile(35, 42); break;
    case 8: set_sprite_tile(35, 44); break;
    case 9: set_sprite_tile(35, 46); break;
  }

  switch(score.tenth)
  {
    case 0: set_sprite_tile(36, 28); break;
    case 1: set_sprite_tile(36, 30); break;
    case 2: set_sprite_tile(36, 32); break;
    case 3: set_sprite_tile(36, 34); break;
    case 4: set_sprite_tile(36, 36); break;
    case 5: set_sprite_tile(36, 38); break;
    case 6: set_sprite_tile(36, 40); break;
    case 7: set_sprite_tile(36, 42); break;
    case 8: set_sprite_tile(36, 44); break;
    case 9: set_sprite_tile(36, 46); break;
  }

  switch(score.hundredth)
  {
    case 0: set_sprite_tile(37, 28); break;
    case 1: set_sprite_tile(37, 30); break;
    case 2: set_sprite_tile(37, 32); break;
    case 3: set_sprite_tile(37, 34); break;
    case 4: set_sprite_tile(37, 36); break;
    case 5: set_sprite_tile(37, 38); break;
    case 6: set_sprite_tile(37, 40); break;
    case 7: set_sprite_tile(37, 42); break;
    case 8: set_sprite_tile(37, 44); break;
    case 9: set_sprite_tile(37, 46); break;
  }

  switch(score.thousandth)
  {
    case 0: set_sprite_tile(38, 28); break;
    case 1: set_sprite_tile(38, 30); break;
    case 2: set_sprite_tile(38, 32); break;
    case 3: set_sprite_tile(38, 34); break;
    case 4: set_sprite_tile(38, 36); break;
    case 5: set_sprite_tile(38, 38); break;
    case 6: set_sprite_tile(38, 40); break;
    case 7: set_sprite_tile(38, 42); break;
    case 8: set_sprite_tile(38, 44); break;
    case 9: set_sprite_tile(38, 46); break;
  }
}

Score *Get_score()
{
  return &score;
}

UBYTE Hitbox_intersects(Hitbox *h1, Hitbox *h2)
{
  if(h1->x > h2->xMax)
  {
    return 0;
  }

  if(h1->xMax < h2->x)
  {
    return 0;
  }

  if(h1->y > h2->yMax)
  {
    return 0;
  }

  if(h1->yMax < h2->y)
  {
    return 0;
  }

  return 1;
}

UBYTE Hitbox_contains(Hitbox *h, UBYTE x, UBYTE y)
{
  return h->x >= x && h->xMax <= x && h->y >= y && h->yMax <= y;
}
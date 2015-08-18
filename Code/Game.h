////////////////////
// structs
////////////////////
typedef struct Hitbox
{
	UBYTE x;
	UBYTE y;
	UBYTE xMax;
	UBYTE yMax;
} Hitbox;

typedef struct Player
{
	UBYTE x;
	UBYTE y;
	Hitbox hitbox;
	UBYTE isInvulnerable;
	UBYTE invulnerableCount;
	UBYTE invAnimRate;
	UBYTE life;
} Player;

typedef struct Obstacle
{
	UBYTE isActive;
	UBYTE type;
	UBYTE x;
	UBYTE y;
	UBYTE speed;
	UBYTE moveRate;
	Hitbox hitbox;
} Obstacle;

typedef struct Score
{
	UBYTE single;
	UBYTE tenth;
	UBYTE hundredth;
	UBYTE thousandth;
} Score;

////////////////////
// functions
////////////////////
UBYTE Run_game(void);
UBYTE Hitbox_intersects(Hitbox *h1, Hitbox *h2);
UBYTE Hitbox_contains(Hitbox *h1, UBYTE x, UBYTE y);
Score *Get_score();
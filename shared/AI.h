/* AI — C port of shared/AI.pas */

#ifndef AI_H
#define AI_H

#include "Sprites.h"
#include "Vector.h"
#include "Constants.h"
#include <stdbool.h>

#define DIST_AWAY 731
#define DIST_TOO_FAR 730
#define DIST_VERY_FAR 500
#define DIST_FAR 350
#define DIST_ROCK_THROW 180
#define DIST_CLOSE 95
#define DIST_VERY_CLOSE 55
#define DIST_TOO_CLOSE 35

#define DIST_COLLIDE 20
#define DIST_STOP_PRONE 25

int AI_check_distance(float pos_a, float pos_b);
void AI_simple_decision(TSprite *sprite_c);
void AI_go_to_thing(int s_num, int t_num);
void AI_control_bot(TSprite *sprite_c);

#endif /* AI_H */
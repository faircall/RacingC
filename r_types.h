#ifndef _R_TYPES_H_


typedef unsigned char byte;
typedef unsigned int uint;


#define SCREENWIDTH 800
#define SCREENHEIGHT 600

#define TRACKGAP 2 // do I still need this?
#define TRACKCOLS 20
#define TRACKROWS 15 

#define TRACKWIDTH 40
#define TRACKHEIGHT 40 //temp doubled

#define TRACK_TILE 0
#define WALL_TILE 1
#define PLAYER_START_TILE 2
#define FINISH_TILE 3

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    int exists;
    int width;
    int height;
    int x;
    int y;
} Track;

typedef struct {
    Vec2 position;
    Vec2 heading;
    float angle;
    int radius;
    float speed;
    int finished;
} Car;

typedef enum {
    GAME_START,
    GAME_PLAYING,
    GAME_OVER
} GAMESTATE;


#define _R_TYPES_H_
#endif

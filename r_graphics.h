
#include "r_types.h"


#include "SDL.h"

typedef struct {
    byte r;
    byte g;
    byte b;
    byte a;
} Color;

void draw_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height);

void draw_rotated_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height, float angle);

void draw_rect(SDL_Renderer *renderer, int x, int y, int w, int h, Color colo);

Color init_color(byte red, byte green, byte blue, byte alpha);

void draw_scores(SDL_Renderer *renderer, int score_p1, int score_p2);

void draw_tracks(SDL_Renderer *renderer, Track *tracks);



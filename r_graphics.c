#include "r_graphics.h"

void draw_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height)
{
    SDL_Rect dest = {x - width/2.0f, y - width/2.0f, width, height};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

void draw_rotated_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height, float angle)
{
    SDL_Rect dest = {x - width/2.0f, y - width/2.0f, width, height};
    SDL_RenderCopyEx(renderer, texture, NULL, &dest, angle + 270.0f, NULL, SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
}

void draw_rect(SDL_Renderer *renderer, int x, int y, int w, int h, Color color)
{
    SDL_Rect temp_rect;
    Color old_color;
    SDL_GetRenderDrawColor(renderer, &old_color.r, &old_color.g, &old_color.b, &old_color.a);
    temp_rect.x = x;
    temp_rect.y = y;
    temp_rect.w = w;
    temp_rect.h = h;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &temp_rect);
    SDL_SetRenderDrawColor(renderer, old_color.r, old_color.g, old_color.b, old_color.a);
    
}

Color init_color(byte red, byte green, byte blue, byte alpha)
{
    Color result;
    result.r = red;
    result.g = green;
    result.b = blue;
    result.a = alpha;

    return result;
}

void draw_scores(SDL_Renderer *renderer, int score_p1, int score_p2)
{
    int score_width = 5;
    int score_height = 5;
    Color score_color = {255,255,255,1};
    for (int i = 0; i < score_p1; i++) {
	draw_rect(renderer, (float)(SCREENWIDTH/4 + i*score_width*2), (float)30, score_width, score_height, score_color );
    }

}

void draw_tracks(SDL_Renderer *renderer, Track *tracks)
{
    Color blue = {0x00, 0x00, 0xff, 0x00};
    Color red = {0xff, 0x00, 0x00, 0x00};
    Color yellow = {0xff, 0xff, 0x00, 0x00};
    
    for (int i = 0; i < TRACKROWS; i++) {
	for (int j = 0; j < TRACKCOLS; j++) {
	    Track track = tracks[i*TRACKCOLS + j];
	    // semantics are a bit wrong here
	    if (track.exists == WALL_TILE) {
		draw_rect(renderer, track.x , track.y, track.width-TRACKGAP, track.height-TRACKGAP, blue);
	    } else if (track.exists == FINISH_TILE) {
		draw_rect(renderer, track.x , track.y, track.width-TRACKGAP, track.height-TRACKGAP, yellow);
	    }
	}
    }
}

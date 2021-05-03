#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"

#define SCREENWIDTH 800
#define SCREENHEIGHT 600



//TODO:
//normalalzing acutally seems to make it less fun?
//fix the issue of collisions to be more robust
//have a size on the tracks
//have a gameover state
//remove the cruft about scores from pong



#define TRACKGAP 2
#define TRACKCOLS 20
#define TRACKROWS 15 

#define TRACKWIDTH 40
#define TRACKHEIGHT 40 //temp doubled

typedef unsigned char byte;

//this could be packed into an int, it's worth noting

typedef struct {
    int exists;
    int width;
    int height;
    int x;
    int y;
} Track;

typedef struct {
    byte r;
    byte g;
    byte b;
    byte a;
} Color;

typedef struct {
    float x;
    float y;
} Vec2;



typedef struct {
    Vec2 position;
    Vec2 heading;
    int radius;
} Car;

typedef enum {
    GAME_START,
    GAME_PLAYING,
    GAME_OVER
} GAMESTATE;

Vec2 vec_normalize(Vec2 in);

void draw_rect(SDL_Renderer *renderer, int x, int y, int w, int h, Color color);

Color init_color(byte red, byte green, byte blue, byte alpha);

Vec2 init_vec2(float x, float y);



Car init_car(float x, float y, float x_heading, float y_heading);


void move_car(Track *tracks, Car *car, float car_speed, float dt, int *score_p1, int *tracks_destroye);

void reset_car(Car *car);



float distance(float x, float y);

float float_min(float x, float y);

float float_max(float x, float y);

void draw_scores(SDL_Renderer *renderer, int score_p1);

void draw_tracks(SDL_Renderer *renderer, Track *tracks);

Track init_track(int x, int y);

void reset_tracks(Track *tracks, int track_design[])
{
    
    for (int i = 0; i < TRACKROWS; i++) {
	for (int j = 0; j < TRACKCOLS; j++) {
	    Track track = init_track(j*(TRACKWIDTH) , i*(TRACKHEIGHT), track_design[i*TRACKCOLS + j]);
	    tracks[i*TRACKCOLS + j] = track;
	  
	}
    }
}

int main(int argc, char **argv)
{

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
	printf("didn't init SDL\n");
	return 1;
    }

    SDL_Window *window;
    SDL_Renderer *renderer;
    if (SDL_CreateWindowAndRenderer(SCREENWIDTH, SCREENHEIGHT, 0, &window, &renderer) != 0) {
	printf("didn't create window/renderer \n");
	return 1;
    }
    
    
    int game_running = 1;

    /////////////Game specific values/////////////
    Color white = {0xff, 0xff, 0xff, 0x00};

    
    Car car = init_car(SCREENWIDTH/2, SCREENHEIGHT/2, -1.0f, 0.5f);

    int track[TRACKROWS * TRACKCOLS] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
	1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    float car_speed = 300.0f;
    float dt;
    unsigned int current_time, time_elapsed, last_time = 0;
    int score_p1 = 0;
    int score_p2 = 0;
    GAMESTATE gamestate = GAME_START;

    int tracks_destroyed = 0;
    int empty_rows = 3;
    int total_tracks = TRACKROWS * TRACKCOLS - (empty_rows * TRACKCOLS);
    
    Track *tracks = (Track*)malloc(sizeof(Track) * TRACKROWS * TRACKCOLS);
    
    reset_tracks(tracks, track);



    
    while (game_running) {
	current_time = SDL_GetTicks();
	time_elapsed = current_time - last_time;
	dt = ((float)time_elapsed)/1000.0f;
	last_time = current_time;
	
	SDL_Event event;
	SDL_PollEvent(&event);
	
	
	if (event.type == SDL_QUIT) {
	    game_running = 0;
	}
	if (gamestate == GAME_START) {
	    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0x00);
	    SDL_RenderClear(renderer);
	    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);
	    

	
	    draw_rect(renderer, car.position.x, car.position.y, 5.0f, 5.0f, white);
	    draw_scores(renderer, score_p1, score_p2);
	    if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
		    gamestate = GAME_PLAYING;
		}
	    }
	} else if (gamestate == GAME_PLAYING) {
	    
	
	    move_car(tracks, &car, car_speed, dt, &score_p1, &tracks_destroyed);
	    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0x00);
	    SDL_RenderClear(renderer);
	    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);


	    draw_tracks(renderer, tracks);
	
	    draw_rect(renderer, car.position.x, car.position.y, 5.0f, 5.0f, white);
	    draw_scores(renderer, score_p1, score_p2);
	    if (score_p1 == 5 || score_p2 == 5) {
		gamestate = GAME_OVER;
	    }
	    if (tracks_destroyed == total_tracks) {
		tracks_destroyed = 0;
		reset_tracks(tracks, empty_rows);
		reset_car(&car);
		gamestate = GAME_START;
		score_p1 = 0;
	    }
	} else if (gamestate == GAME_OVER) {
	    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0x00);
	    SDL_RenderClear(renderer);
	    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);
	    

	
	    draw_rect(renderer, car.position.x, car.position.y, 5.0f, 5.0f, white);
	    draw_scores(renderer, score_p1);
	    if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
		    score_p1 = 0;
		    score_p2 = 0;
		    gamestate = GAME_PLAYING;
		}
	    }
	}



	SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
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

Vec2 init_vec2(float x, float y)
{
    Vec2 result;
    result.x = x;
    result.y = y;
    return result;
}


Car init_car(float x, float y, float x_heading, float y_heading)
{

    int car_radius = 5;
    Vec2 position = init_vec2(x,y);
    Vec2 heading = init_vec2(x_heading, y_heading);
    Car result;
    result.position = position;
    result.heading = heading;
    result.radius = car_radius;
    return result;
}

void move_car(Track *tracks, Car *car, float car_speed, float dt, int *score_p1, int *tracks_destroyed)
{
    Vec2 previous_position = {car->position.x, car->position.y};

    car->position.x += car_speed*car->heading.x*dt;
    car->position.y += car_speed*car->heading.y*dt;

    //reflect along y axis
    //we should really make sure we *bounce back straight away* rather than waiting to update next frame, because it might not move us enough
    //if less time passes
    if (car->position.x <=0 || car->position.x >= (SCREENWIDTH - car->radius)) {
	car->heading.x *= -1.0f;
	car->position.x += car_speed * car->heading.x * dt;
    }



    if (car->position.y <= 0 || car->position.y >= (SCREENHEIGHT - car->radius)) {
	car->heading.y *= -1.0f;
	car->position.y += car_speed * car->heading.y * dt;

    }
    


    //is there a way to quickly identify which track to check?
    //based on car x/ car y?

    //yeah....

    //it's the car X position divided by the trackwidth
    //and the car Y position...um...
    //divided by the trackheight?

    int prev_index_x = (int)((int)(previous_position.x)/(int)TRACKWIDTH);//col;
    int prev_index_y = (int)((int)(previous_position.y)/(int)TRACKHEIGHT);//row;
    
    int track_index_x = (int)((int)(car->position.x)/(int)TRACKWIDTH);//col
    int track_index_y = (int)((int)(car->position.y)/(int)TRACKHEIGHT);//row
    
    if (track_index_y < TRACKROWS && track_index_x < TRACKCOLS) {
	Track track = tracks[track_index_y*TRACKCOLS + track_index_x];
	if (track.exists) {
	    //use our algorithm here
	    
	    int both_failed = 1;
	    Track neighbour_track_col = tracks[track_index_y*TRACKCOLS + prev_index_x];
	    Track neighbour_track_row = tracks[prev_index_y*TRACKCOLS + track_index_x];

	    if (prev_index_x != track_index_x) {
		//came in horizontally
		if (neighbour_track_row.exists) {
		    car->heading.x *= -1.0f;
		    both_failed = 0;
		}
	    }
	    if (prev_index_y != track_index_y) {
		//cam in vertically
		if (neighbour_track_col.exists) {
		    
		    car->heading.y *= -1.0f;
		    both_failed = 0;
		}
	    }
	    if (both_failed) {
		car->heading.x *= -1.0f;
		car->heading.y *= -1.0f;
	    }
	    //car->heading.x *= -1.0f;
	    track.exists = 0;
	    (*tracks_destroyed) += 1;
	    tracks[track_index_y*TRACKCOLS + track_index_x] = track;
	}
    }
    
#if 0
    for (int i = 0; i < TRACKROWS; i++) {
	for (int j = 0; j < TRACKCOLS; j++) {
	    Track track = tracks[i*TRACKCOLS + j];
	    if (track.exists) {
		if (car->position.x >= track.x && (car->position.x <= track.x + track.width)
		    && car->position.y >= track.y && (car->position.y <= track.y + track.height)) {
		    car->heading.y *= -1.0f;
		    //car->heading.x *= -1.0f;
		    track.exists = 0;
		    tracks[i*TRACKCOLS + j] = track;
		}
	    }
	}
    }
#endif
    car->heading = vec_normalize(car->heading);
    //

}

void reset_car(Car *car)
{
    car->position.x = SCREENWIDTH/2;
    car->position.y = SCREENHEIGHT/2;
    car->heading.x *= -1.0f;
}


float float_min(float x, float y)
{
    if (x <= y) {
	return x;
    }
    return y;
}

float float_max(float x, float y)
{
    if (x >= y) {
	return x;
    }
    return y;
}

float distance(float x, float y)
{
    float result;
    result = sqrt((x-y)*(x-y));
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

static Vec2 vec_normalize(Vec2 in)
{
    Vec2 result;
    float mag = sqrt(in.x*in.x + in.y*in.y);
    if (mag != 0.0f) {
	result.x = in.x/mag;
	result.y = in.y/mag;
    } else {
	result.x = 0.0f;
	result.y = 0.0f;
    }

    return result;
}

Track init_track(int x, int y, int exists)
{
    Track result;
    result.x = x;
    result.y = y;
    result.width = TRACKWIDTH;
    result.height = TRACKHEIGHT;
    result.exists = exists;
    return result;
}

void draw_tracks(SDL_Renderer *renderer, Track *tracks)
{
    Color blue = {0x00, 0x00, 0xff, 0x00};
    
    for (int i = 0; i < TRACKROWS; i++) {
	for (int j = 0; j < TRACKCOLS; j++) {
	    Track track = tracks[i*TRACKCOLS + j];
	    if (track.exists) {
		draw_rect(renderer, track.x , track.y, track.width-TRACKGAP, track.height-TRACKGAP, blue);
	    }
	}
    }
}

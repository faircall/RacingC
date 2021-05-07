#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

#define MPI 3.1415926535

typedef unsigned char byte;
typedef unsigned int uint;

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
    float angle;
    int radius;
    float speed;
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


void move_car(Track *tracks, Car *car, float dt, int *score_p1);

void reset_car(Car *car);



float distance(float x, float y);

float float_min(float x, float y);

float float_max(float x, float y);

void draw_scores(SDL_Renderer *renderer, int score_p1);

void draw_tracks(SDL_Renderer *renderer, Track *tracks);

Track init_track(int x, int y);

float deg_to_rad(float deg)
{
    float result = MPI*deg/180.0f;
    return result;
}

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

    
    Car car = init_car(SCREENWIDTH/2 + 50.0f, SCREENHEIGHT/2, -1.0f, 0.5f);

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

    int imx, imy, imn; 
    unsigned char *car_image = stbi_load("car_top.png", &imx, &imy, &imn, 0);

    //load file using SDL here
    unsigned int rmask, gmask, bmask, amask;
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    SDL_Surface *car_surface = SDL_CreateRGBSurfaceFrom((void*)car_image, imx, imy, 32, 4*imx, rmask, gmask, bmask, amask);
    SDL_Texture *car_texture = SDL_CreateTextureFromSurface(renderer, car_surface);

    if (car_texture == NULL) {
	fprintf(stderr, "Failed to create texture\n", SDL_GetError());
	SDL_Quit();
    }

    SDL_FreeSurface(car_surface);

    
    while (game_running) {
	current_time = SDL_GetTicks();
	time_elapsed = current_time - last_time;
	dt = ((float)time_elapsed)/1000.0f;
	last_time = current_time;
	
	SDL_Event event;
	SDL_PollEvent(&event);

	if (event.type == SDL_KEYDOWN) {
	    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		SDL_Quit();
	    }
	}
	
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
	    
	    //get keys pressed, later
	    //just rotate for now
	    //car.angle += (100.0f*dt);
	    const Uint8 *keys = SDL_GetKeyboardState(NULL);
	    //car.speed = 0.0f;
	    if (keys[SDL_SCANCODE_A]) {
		car.angle -= (100.0f*dt);
	    }
	    if (keys[SDL_SCANCODE_D]) {
		car.angle += (100.0f*dt);
	    }
	    if (keys[SDL_SCANCODE_W]) {
		car.speed += car_speed*dt;
	    }
	    if (keys[SDL_SCANCODE_S]) {
		car.speed -= car_speed*dt;
	    }
	    
	    move_car(tracks, &car, dt, &score_p1);
	    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0x00);
	    SDL_RenderClear(renderer);
	    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);


	    draw_tracks(renderer, tracks);
	
	    //draw_rect(renderer, car.position.x, car.position.y, 5.0f, 5.0f, white);
	    draw_rotated_texture(renderer, car_texture, car.position.x, car.position.y, imx, imy, car.angle);
	    draw_rect(renderer, car.position.x + car.heading.x*10.0f, car.position.y + car.heading.y*10.0f, 5.0f, 5.0f, white);
	    draw_scores(renderer, score_p1, score_p2);

	    
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
    result.angle = 0.0f;
    result.speed = 0.0f;
    return result;
}

void move_car(Track *tracks, Car *car, float dt, int *score_p1)
{
    
    
    Vec2 previous_position = {car->position.x, car->position.y};
    float angle_rad = deg_to_rad(car->angle);
    car->heading.x = cos(angle_rad);
    car->heading.y = sin(angle_rad);
    car->position.x += car->speed*car->heading.x*dt;
    car->position.y += car->speed*car->heading.y*dt;

    //reflect along y axis
    //we should really make sure we *bounce back straight away* rather than waiting to update next frame, because it might not move us enough
    //if less time passes
    #if 0
    if (car->position.x <=0 || car->position.x >= (SCREENWIDTH - car->radius)) {
	car->heading.x *= -1.0f;
	car->position.x += car->speed * car->heading.x * dt;
    }



    if (car->position.y <= 0 || car->position.y >= (SCREENHEIGHT - car->radius)) {
	car->heading.y *= -1.0f;
	car->position.y += car->speed * car->heading.y * dt;

    }
    #endif
    


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
	    

	    //actually we should reflect,
	    //rather than negate the vector?
	    if (prev_index_x == track_index_x) {
		car->heading.y *= -1.0f;
	    }
	    if (prev_index_y == track_index_y) {
		car->heading.x *= -1.0f;
	    }
	    //car->heading.x *= -1.0f;
	    //car->heading.y *= -1.0f;

	    car->position.x += car->speed*car->heading.x*dt;
	    car->position.y += car->speed*car->heading.y*dt;

	    //car->heading.x *= -1.0f;
	    //track.exists = 0;
	    
	    //tracks[track_index_y*TRACKCOLS + track_index_x] = track;
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
    car->position.x = SCREENWIDTH/2 + 50.0f;
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

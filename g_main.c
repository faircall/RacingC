#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"

#include "r_graphics.h"
#include "r_math.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"








//TODO:
// track tiles and wall tiles
// and finish line tiles
// no turn while still
// normalalzing acutally seems to make it less fun?
// fix the issue of collisions to be more robust
// have a size on the tracks...(what did i mean by this?)
// have a gameover state
// -- this will be a finish line
// remove the cruft about scores from pong
// optimize the track drawing
// add local multiplayer





// this could be packed into an int, it's worth noting



///// Declarations



void draw_rect(SDL_Renderer *renderer, int x, int y, int w, int h, Color color);

Color init_color(byte red, byte green, byte blue, byte alpha);

Vec2 init_vec2(float x, float y);

Car init_car(float x, float y, float x_heading, float y_heading);

void move_car(Track *tracks, Car *car, float dt, int *score_p1);

void reset_car(Car *car);


void draw_scores(SDL_Renderer *renderer, int score_p1);

void draw_tracks(SDL_Renderer *renderer, Track *tracks);

Track init_track(int x, int y, int exists);

/////





void reset_tracks(Track *tracks, int track_design[], Car *car)
{
    
    for (int i = 0; i < TRACKROWS; i++) {
	for (int j = 0; j < TRACKCOLS; j++) {
	    if (track_design[i*TRACKCOLS + j] == 2) {
		// we effectively 'insert' the car here via its pointer
		car->position.x = (float)(j*(TRACKWIDTH)) + 0.5*TRACKWIDTH;
		car->position.y = (float)(i*(TRACKHEIGHT)) + 0.5*TRACKHEIGHT;
		Track track = init_track(j*TRACKWIDTH, i*TRACKHEIGHT, 0);
		tracks[i*TRACKCOLS + j] = track;
	    } else {
		Track track = init_track(j*(TRACKWIDTH) , i*(TRACKHEIGHT), track_design[i*TRACKCOLS + j]);
		tracks[i*TRACKCOLS + j] = track;
	    }
	  
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
	1, 2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,//added a 2
	1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 3, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 3, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    
    float car_speed_min_for_turn = 2.5f;
    float car_turn_speed = 200.0f;

    float car_speed = 2000.0f;
    float dt;
    unsigned int current_time, time_elapsed, last_time = 0;
    int score_p1 = 0;
    int score_p2 = 0;
    GAMESTATE gamestate = GAME_START;

    int tracks_destroyed = 0;
    int empty_rows = 3;
    int total_tracks = TRACKROWS * TRACKCOLS - (empty_rows * TRACKCOLS);
    
    Track *tracks = (Track*)malloc(sizeof(Track) * TRACKROWS * TRACKCOLS);
    
    reset_tracks(tracks, track, &car);

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

	    // only apply a turn if the car is moving
	    if (abs(car.speed) >= car_speed_min_for_turn) {
		if (keys[SDL_SCANCODE_A]) {
		    car.angle -= (car_turn_speed*dt);
		}
		if (keys[SDL_SCANCODE_D]) {
		    car.angle += (car_turn_speed*dt);
		}
	    }
	    if (keys[SDL_SCANCODE_W]) {
		car.speed += car_speed*dt;
	    }
	    if (keys[SDL_SCANCODE_S]) {
		car.speed -= car_speed*dt;
	    }

	    if (car.speed*car.speed > 0.05f) {
		// apply friction
		// this is limiting the max speed?
		car.speed -= (0.002*car.speed);
	    } else {
		car.speed = 0.0f;
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

    // use a 'next position' variable instead
    Vec2 next_position = {car->position.x + car->speed*car->heading.x*dt, car->position.y + car->speed*car->heading.y*dt};


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
    
    int track_index_x = (int)((int)(next_position.x)/(int)TRACKWIDTH);//col
    int track_index_y = (int)((int)(next_position.y)/(int)TRACKHEIGHT);//row
    
    if (track_index_y < TRACKROWS && track_index_x < TRACKCOLS &&
	track_index_y >= 0 && track_index_x >= 0) {
	Track track = tracks[track_index_y*TRACKCOLS + track_index_x];
	if (track.exists) {
	    //use our algorithm here
	    

	    //actually we should reflect,
	    //rather than negate the vector?
	    // what?
	    // instead think of 4 cases of the tile itself,
	    // make a normal vector
	    // then dot with the heading

	    //edge cases
	    
	    //if (track_index_x == 0)

	    
	    if (prev_index_x == track_index_x) {
		car->heading.y *= -1.0f;
		//car->heading.y = 0.0f;
	    }
	    if (prev_index_y == track_index_y) {
		car->heading.x *= -1.0f;
		//car->heading.x = 0.0f;
	    }

	    // what are the other cases?
	    
	    //car->heading.x *= -1.0f;
	    //car->heading.y *= -1.0f;
	    car->position.x += car->speed*car->heading.x*dt;
	    car->position.y += car->speed*car->heading.y*dt;


	    //car->heading.x *= -1.0f;
	    //track.exists = 0;
	    
	    //tracks[track_index_y*TRACKCOLS + track_index_x] = track;
	} else {
	    car->position.x += car->speed*car->heading.x*dt;
	    car->position.y += car->speed*car->heading.y*dt;
	}
    }

    car->heading = vec_normalize(car->heading);
    
    //

}

void reset_car(Car *car)
{
    car->position.x = SCREENWIDTH/2 + 50.0f;
    car->position.y = SCREENHEIGHT/2;
    car->heading.x *= -1.0f;
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



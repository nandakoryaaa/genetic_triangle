/**
 * SDL2 software rendering implementation of genetic algorithm
 * based on "Genetic Mona" program by Károly Zsolnai, zsolnai@iit.bme.hu
 * http://cg.iit.bme.hu/~zsolnai/
 * https://users.cg.tuwien.ac.at/zsolnai/gfx/mona_lisa_parallel_genetic_algorithm/
 */
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_ALPHA 38
#define CHROMO_SIZE 150
#define POP_SIZE 30
#define MAX_CHROMO_SIZE 500
#define MAX_POP_SIZE 100

int pop_size = POP_SIZE;
int chromo_size = CHROMO_SIZE;

#include "genetic_triangle.h"
#include "draw.c"
#include "population.c"

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	if (argc < 2) {
		puts("usage: genetic_triangle.exe image.bmp [chromo_size] [pop_size]");
		printf("chromo_size: 1..500, default %d\n", chromo_size);
		printf("pop_size: 4..100, default %d\n", pop_size);
		return 1;
	}

	char *filename = argv[1];
	SDL_Surface *bmpSurface = SDL_LoadBMP(argv[1]);
	if (!bmpSurface) {
		printf("Error: %s\n", SDL_GetError());
		return 1;
	}

	if (argc > 2) {
		chromo_size = atoi(argv[2]);
		if (chromo_size < 1 || chromo_size > 500) {
			chromo_size = CHROMO_SIZE;
		}
	}

	if (argc > 3) {
		pop_size = atoi(argv[3]);
		if (pop_size < 4 || pop_size > 100) {
			pop_size = POP_SIZE;
		}
	}
	
	printf("chromo_size: %d\n", chromo_size);
	printf("pop_size: %d\n", pop_size);

	int w = bmpSurface->w;
	int h = bmpSurface->h;
	SDL_Rect rect = {0, 0, w, h};

	SDL_Window *window = SDL_CreateWindow(
		"Triangles",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w,
		h,
		SDL_WINDOW_ALLOW_HIGHDPI
	);

	if (window == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
	SDL_Surface *imgSurface = SDL_ConvertSurface(bmpSurface, screenSurface->format, 0);
	SDL_Surface *chromoSurface = SDL_ConvertSurface(bmpSurface, screenSurface->format, 0);
	if (!imgSurface) {
		puts("Could not convert surface\n");
		return 1;
	}
	SDL_FreeSurface(bmpSurface);

	SDL_PixelFormat *format = imgSurface->format;
	printf("bits per pixel: %d, bytes per pixel: %d\n", format->BitsPerPixel, format->BytesPerPixel);
	printf("rmask: %d, gmask: %d, bmask: %d\n", format->Rmask, format->Gmask, format->Bmask);
	printf("w: %d, h: %d, pitch: %d\n", imgSurface->w, imgSurface->h, imgSurface->pitch);

	srand(time(0));

	SDL_Event event;

	Chromo population[pop_size];
	Chromo *pop_list[pop_size];
	init_population(population, pop_list, w, h);
	int elite_size = pop_size / 4;
	int start = 0;
	while(1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }

//		if (event.type == SDL_KEYDOWN) {
			run_population(chromoSurface, imgSurface, pop_list, start);
			sort_population(pop_list);
			Chromo *chromo = pop_list[0];
			draw_chromo(screenSurface, chromo);
			SDL_UpdateWindowSurface(window);
			for (int i = 0; i < pop_size; i++) {
				if (i >= elite_size) {
					if (rnd(1000) < 950) {
						Chromo *random_chromo1 = pop_list[rnd(pop_size)];
						Chromo *random_chromo2 = pop_list[rnd(pop_size)];
						if (rnd(1000) < 500) {
							cross_1p(random_chromo1, random_chromo2, pop_list[i]);
						} else {
							cross_rnd(random_chromo1, random_chromo2, pop_list[i]);
						}
					} else {
						if (rnd(1000) < 750) {
							mutate_chromo(pop_list[i], w / 2, 20, w, h);
						} else {
							init_chromo(pop_list[i], w, h);
						}
					}
				}
			}
			start = elite_size;
//		}
	}

	SDL_FreeSurface(screenSurface);
	SDL_FreeSurface(imgSurface);
	SDL_FreeSurface(chromoSurface);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

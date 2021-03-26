void print_population(int step, Chromo *pop_list[]) {
	printf("*** %d ***\n", step);
	for (int i = 0; i < pop_size; i++) {
		Chromo *chromo = pop_list[i];
		printf("%d. %llu\n", i, chromo->rating);
	}
}

void init_gene(Triangle *t, int w, int h) {
	t->x0 = rnd(w);
	t->y0 = rnd(h);
	t->x1 = rnd(w);
	t->y1 = rnd(h);
	t->x2 = rnd(w);
	t->y2 = rnd(h);
	t->color.r = rnd(255);
	t->color.g = rnd(255);
	t->color.b = rnd(255);
	t->color.alpha = DEFAULT_ALPHA;
	normalize_triangle(t);
}

void init_chromo(Chromo *chromo, int w, int h) {
	for (int i = 0; i < chromo_size; i++) {
		init_gene(&chromo->genes[i], w, h);
	}
	chromo->rating = 0;
} 

void init_population(Chromo pop[], Chromo *list[], int w, int h) {
	for (int i = 0; i < pop_size; i++) {
		init_chromo(&pop[i], w, h);
		list[i] = &pop[i];
	}
}

void sort_population(Chromo *list[]) {
	int swaps;
	Chromo *bubble;
	do {
		swaps = 0;
		int i = pop_size - 1;
		while(i) {
			bubble = list[i];
			if (bubble->rating < list[i - 1]->rating) {
				list[i] = list[i - 1];
				list[i - 1] = bubble;
				swaps++;
			}
			i--;
		}
	} while(swaps);
}
#define MT 500
void mutate_chromo(Chromo *chromo, int shift, int color_shift, int w, int h) {
	for (int i = 0; i < chromo_size; i++) {
		Triangle *t = &chromo->genes[i];
		if (rnd(1000) < MT) {
			t->x0 += rnds(w/2);
			if (t->x0 >= w || t->x0 < 0) {
				t->x0 = rnd(w);
			}
		}
		if (rnd(1000) < MT) {
			t->y0 += rnds(h/10);
			if (t->y0 >= h || t->y0 < 0) {
				t->y0 = rnd(h);
			}
		}
		if (rnd(1000) < MT) {
			t->x1 += rnds(w/2);
			if (t->x1 >= w || t->x1 < 0) {
				t->x1 = rnd(w);
			}
		}
		if (rnd(1000) < MT) {
			t->y1 += rnds(h/2);
			if (t->y1 >= h || t->y1 < 0) {
				t->y1 = rnd(h);
			}
		}
		if (rnd(1000) < MT) {
			t->x2 += rnds(w/2);
			if (t->x2 >= w || t->x2< 0) {
				t->x2 = rnd(w);
			}
		}
		if (rnd(1000) < MT) {
			t->y2 += rnds(h/2);
			if (t->y2 >= h || t->y2 < 0) {
				t->y2 = rnd(h);
			}
		}

		if (rnd(1000) < 500) {
			t->color.r += rnds(color_shift);
			t->color.g += rnds(color_shift);
			t->color.b += rnds(color_shift);
			if (t->color.r > 255 || t->color.r < 0) {
				t->color.r = rnd(255);
			}
			if (t->color.g > 255 || t->color.g < 0) {
				t->color.g = rnd(255);
			}
			if (t->color.b > 255 || t->color.b < 0) {
				t->color.b = rnd(255);
			}
		}

		normalize_triangle(t);
	}
}

unsigned long long calc_rating(SDL_Surface *chromoSurf, SDL_Surface *imgSurf) {
	unsigned char *chromo_pixels = (unsigned char *) chromoSurf->pixels;
	unsigned char *img_pixels = (unsigned char *) imgSurf->pixels;
	unsigned char *chromo_offset;
	unsigned char *img_offset;

	unsigned long long diff = 0;

	for (int y = 0; y < chromoSurf->h; y++) {
		chromo_offset = chromo_pixels;
		img_offset = img_pixels;
		for (int x = 0; x < chromoSurf->w; x++) {
			int b_diff = *chromo_offset - *img_offset;
			int g_diff = chromo_offset[1] - img_offset[1];
			int r_diff = chromo_offset[2] - img_offset[2];
			diff += r_diff * r_diff + g_diff * g_diff + b_diff * b_diff;
			chromo_offset += 4;
			img_offset += 4;
		}
		chromo_pixels += chromoSurf->pitch;
		img_pixels += imgSurf->pitch;
	}

	return diff;
}

void run_population(SDL_Surface *chromoSurf, SDL_Surface *imgSurf, Chromo *pop_list[], int start) {
	for (int i = start; i < pop_size; i++) {
		Chromo *chromo = pop_list[i];
		draw_chromo(chromoSurf, chromo);
		chromo->rating = calc_rating(chromoSurf, imgSurf);
	}
}

void cross_1p(Chromo *chromo1, Chromo *chromo2, Chromo *chromo3) {
	int p = rnd(chromo_size);
	Triangle *src_t, *dst_t;
	for (int i = 0; i < p; i++) {
		memcpy(&chromo3->genes[i], &chromo1->genes[i], sizeof(Triangle));
	}
	for (int i = p; i < chromo_size; i++) {
		memcpy(&chromo3->genes[i], &chromo2->genes[i], sizeof(Triangle));
	}
}

void cross_rnd(Chromo *chromo1, Chromo *chromo2, Chromo *chromo3) {
	for (int i = 0; i < chromo_size; i++) {
		if (rnd(1000) < 500) {
			memcpy(&chromo3->genes[i], &chromo1->genes[i], sizeof(Triangle));
		} else {
			memcpy(&chromo3->genes[i], &chromo2->genes[i], sizeof(Triangle));
		}
	}
}

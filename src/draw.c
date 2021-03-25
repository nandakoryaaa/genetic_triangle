void draw_alpha_line(
	unsigned char *addr, int w, alpha_pixel *p
) {
	while (w--) {
		*addr = (p->b + *addr * p->alpha) / 255;
		addr++;
		*addr = (p->g + *addr * p->alpha) / 255;
		addr++;
		*addr = (p->r + *addr * p->alpha) / 255;
		addr += 2;
	}
}

void normalize_triangle(Triangle *t) {
	int tmp;
	if (t->y0 > t->y1) {
		tmp = t->y0;
		t->y0 = t->y1;
		t->y1 = tmp;
		tmp = t->x0;
		t->x0 = t->x1;
		t->x1 = tmp;
	}

	if (t->y0 > t->y2) {
		tmp = t->y0;
		t->y0 = t->y2;
		t->y2 = tmp;
		tmp = t->x0;
		t->x0 = t->x2;
		t->x2 = tmp;
	}

	if (t->y1 > t->y2) {
		tmp = t->y1;
		t->y1 = t->y2;
		t->y2 = tmp;
		tmp = t->x1;
		t->x1 = t->x2;
		t->x2 = tmp;
	}
}


void draw_triangle(SDL_Surface *surf, Triangle *t) {

	int cross_x1;
	int cross_x2;
	int dx1 = t->x1 - t->x0;
	int dy1 = t->y1 - t->y0;
	int dx2 = t->x2 - t->x0;
	int dy2 = t->y2 - t->y0;

	alpha_pixel p = {
		t->color.r * t->color.alpha,
		t->color.g * t->color.alpha,
		t->color.b * t->color.alpha,
		255 - t->color.alpha
	};

	int top_y = t->y0;
	unsigned char *addr = (unsigned char *) surf->pixels + top_y * surf->pitch;

	while(top_y < t->y1) {
		cross_x1 = t->x0 + dx1 * (top_y - t->y0) / dy1;
		cross_x2 = t->x0 + dx2 * (top_y - t->y0) / dy2;
		if (cross_x1 > cross_x2) {
			draw_alpha_line(addr + cross_x2 * 4, cross_x1 - cross_x2, &p);
		} else {
			draw_alpha_line(addr + cross_x1 * 4, cross_x2 - cross_x1, &p);
		}

		top_y++;
		addr += surf->pitch;
	}

	dx1 = t->x2 - t->x1;
	dy1 = t->y2 - t->y1;

	while(top_y < t->y2) {
		cross_x1 = t->x1 + dx1 * (top_y - t->y1) / dy1;
		cross_x2 = t->x0 + dx2 * (top_y - t->y0) / dy2;
		if (cross_x1 > cross_x2) {
			draw_alpha_line(addr + cross_x2 * 4, cross_x1 - cross_x2, &p);
		} else {
			draw_alpha_line(addr + cross_x1 * 4, cross_x2 - cross_x1, &p);
		}

		top_y++;
		addr += surf->pitch;
	}
}

void draw_chromo(SDL_Surface *surf, Chromo *chromo) {
	SDL_Rect rect = {0, 0, surf->w, surf->h};
	SDL_FillRect(surf, &rect, 0);

	for (int i = 0; i < chromo_size; i++) {
		Triangle *t = &chromo->genes[i];
		draw_triangle(surf, t);
	}
}

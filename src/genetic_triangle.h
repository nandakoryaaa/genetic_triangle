typedef struct {
	int r, g, b, alpha;
} alpha_pixel;

typedef struct {
	int x0, y0, x1, y1, x2, y2;
	alpha_pixel color; 
} Triangle;

typedef struct {
	Triangle genes[MAX_CHROMO_SIZE];
	unsigned long rating;
} Chromo;

int rnd(int range) {
	return rand() % range;
}

int rnds(int range) {
	return (rand() % (range + range + 1)) - range;
}

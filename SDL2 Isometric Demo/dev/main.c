#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>

#define bool Uint8
#define false (bool)0
#define true (bool)1
#define WINDOW_W 1200
#define WINDOW_H 600

//////////---//////////

#define GRID_SIZE 10 //Feel free to experiment by changing this value!
#define TILE_W 100
#define TILE_H (TILE_W / 2)
#define ORIGIN_X (WINDOW_W / 2)
#define ORIGIN_Y (WINDOW_H / 2 - TILE_H * GRID_SIZE / 2)

typedef struct {
	Sint8 x;
	Sint8 y;
} Grid;
typedef struct {
	Sint32 x;
	Sint32 y;
} Vector2D;
typedef struct {
	Grid pos;
	//For the sake of the demo a tile only has a position.
} Tile;

bool init();
bool loop();
void quit();
void draw();
void drawTile(Tile*, int);
Vector2D gridToScreen(Grid);
Grid screenToGrid(Vector2D);

//////////---//////////

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* blankTileTexture;

Tile tiles[GRID_SIZE][GRID_SIZE];
Grid oldSelectedGrid = {-1, -1};
Grid selectedGrid = {-1, -1};

int main(int argc, char* argv[]) {
	if (!init()) return 1;
	draw();

	while (1) {
		if (!loop()) break;

		//Wait for next physics/animation tick
		SDL_Delay(10);
	}

	quit();
	return 0;
}

bool init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("Couldn't initialize SDL.\n%s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("Isometric Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window) {
		printf("Couldn't create window.\n%s\n", SDL_GetError());
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		printf("Couldn't create renderer.\n%s\n", SDL_GetError());
		return false;
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		printf("Couldn't initialize SDL_image.\n%s\n", SDL_GetError());
		return false;
	}

	SDL_Surface* windowIcon = IMG_Load("./resources/Cube.png");
	if (!windowIcon) {
		printf("Couldn't initialize the window's icon.\n");
		return false;
	}
	SDL_SetWindowIcon(window, windowIcon);
	SDL_FreeSurface(windowIcon);

	blankTileTexture = IMG_LoadTexture(renderer, "./resources/Blank Tile.png");
	if (!blankTileTexture) {
		printf("Couldn't initialize a texture.\n");
		return false;
	}

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			tiles[y][x] = (Tile){{x, y}};
		}
	}

	return true;
}
bool loop() {
	bool shouldRedraw = false;

	//Handle events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) return false;
		else if (event.type == SDL_MOUSEMOTION) {
			oldSelectedGrid = selectedGrid;
			selectedGrid = screenToGrid((Vector2D){event.motion.x, event.motion.y});

			if (oldSelectedGrid.x != selectedGrid.x || oldSelectedGrid.y != selectedGrid.y) {
				shouldRedraw = true;
			}
		}
	}

	//Update

	//Draw
	if (shouldRedraw) draw();

	return true;
}
void quit() {
	SDL_DestroyTexture(blankTileTexture);
	IMG_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
void draw() {
	SDL_SetRenderDrawColor(renderer, 217, 217, 217, 255);
	SDL_RenderClear(renderer);

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (x == selectedGrid.x && y == selectedGrid.y) drawTile(&tiles[y][x], -3);
			else drawTile(&tiles[y][x], 0);
		}
	}

	SDL_RenderPresent(renderer);
}

void drawTile(Tile* tile, int offsetY) {
	Vector2D screen = gridToScreen((*tile).pos);

	SDL_RenderCopy(renderer, blankTileTexture, NULL, &(SDL_Rect){screen.x, screen.y + offsetY, TILE_W, TILE_H});
}
Vector2D gridToScreen(Grid grid) {
	return (Vector2D){ORIGIN_X - TILE_W / 2 + (grid.x - grid.y) * (TILE_W / 2), ORIGIN_Y + (grid.y + grid.x) * (TILE_H / 2)};
};
Grid screenToGrid(Vector2D screen) {
	screen.x -= ORIGIN_X;
	screen.y -= ORIGIN_Y;

	int pixelX = screen.y * 2 + screen.x;
	int pixelY = screen.y * 2 - screen.x;

	if (pixelX <= 0 || pixelX >= GRID_SIZE * TILE_W || pixelY <= 0 || pixelY >= GRID_SIZE * TILE_W) return (Grid){-1, -1};
	else return (Grid){pixelX / TILE_W, pixelY / TILE_W};
};
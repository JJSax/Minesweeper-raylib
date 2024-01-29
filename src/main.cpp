#include <raylib.h>

#include "grid.hpp"

int main() {

	// Init
	const float tileSize = 30;
	const int gWidth = 18;
	const int gHeight = 14;
	const int footHeight = 50;

	InitWindow(gWidth * tileSize, gHeight * tileSize + footHeight, "Minesweeper");

	Grid field(gWidth, gHeight, tileSize);
	load();
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		//update
		//draw
		BeginDrawing();
		ClearBackground(BLACK);

		field.render();

		EndDrawing();
	}
	CloseWindow();

	unload();

	return 0;
}
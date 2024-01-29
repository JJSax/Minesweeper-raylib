#include <raylib.h>

#include "grid.hpp"

int main() {

	// Init
	const float tileSize = 30;
	const int gWidth = 18;
	const int gHeight = 14;
	const int footHeight = 50;
	const int totalMines = 40;

	InitWindow(gWidth * tileSize, gHeight * tileSize + footHeight, "Minesweeper");

	Grid field(gWidth, gHeight, tileSize, totalMines);
	load();
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		//update

		if (IsMouseButtonPressed(0) && field.hasCellAtPixel(GetMousePosition())) {
			field.placeMines(field.cellAtPixel(GetMousePosition()));
		}

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

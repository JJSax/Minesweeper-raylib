#include <raylib.h>

#include "grid.hpp"

int main() {

	// Init
	const float tileSize = 30;
	const int gWidth = 18;
	const int gHeight = 14;
	const int footHeight = 50;
	const int totalMines = 4;
	bool initialized = false; // if map has been created.

	InitWindow(gWidth * tileSize, gHeight * tileSize + footHeight, "Minesweeper");
	InitAudioDevice();

	Grid field(gWidth, gHeight, tileSize, totalMines);
	load();
	SetTargetFPS(60);
	while (!WindowShouldClose()) {

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
				field.handleDigAround(GetMousePosition());
			} else {
				field.handleLeftClick(GetMousePosition());
			}
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			field.flag(GetMousePosition());
		}

		//update
		field.update();

		//draw
		BeginDrawing();
		ClearBackground(BLACK);

		field.render();

		EndDrawing();
	}
	unload();
	CloseAudioDevice();
	CloseWindow();


	return 0;
}

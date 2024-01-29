#include <raylib.h>

#include "minesweeper.hpp"

int main() {

	// Init
	const float tileSize = 30;
	const int gWidth = 18;
	const int gHeight = 14;
	const int footHeight = 50;

	InitWindow(gWidth * tileSize, gHeight * tileSize + footHeight, "Minesweeper");

	MS::Minefield field(gWidth, gHeight, tileSize);
	MS::load();
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

	MS::unload();

	return 0;
}
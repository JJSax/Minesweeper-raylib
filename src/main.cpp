#include <raylib.h>
#include <string>

#include "grid.hpp"

int main() {

	// Init
	const float tileSize = 30;
	const int gWidth = 18;
	const int gHeight = 14;
	const int footHeight = 50;
	const int totalMines = 20;
	bool initialized = false; // if map has been created.

	InitWindow(gWidth * tileSize, gHeight * tileSize + footHeight, "Minesweeper");
	InitAudioDevice();

	Grid field(gWidth, gHeight, tileSize, totalMines);
	Texture2D texture = LoadTexture("assets/sprite.png");
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
		DrawTexturePro(
			texture, {140, 40, 40, 40},
			{40, gHeight * tileSize, 40, 40},
			{0, 0}, 0, WHITE
		);
		int flagsRemaining = totalMines - field.getTotalFlags();
		DrawText(
			TextFormat("%i", flagsRemaining),
			60, gHeight * tileSize + 5, 30, WHITE
		);

		std::string gameMessage = "";
		if (field.stateLose()) {gameMessage = "Gameover";}
		if (field.stateWin() ) {gameMessage = "WINNER";}
		DrawText(
			gameMessage.c_str(),
			gWidth * tileSize / 2 - MeasureText(gameMessage.c_str(), 30) / 2,
			gHeight * tileSize + 10,
			30,
			field.stateLose() ? PINK : WHITE
		);

		EndDrawing();
	}
	unload();
	UnloadTexture(texture);
	CloseAudioDevice();
	CloseWindow();


	return 0;
}

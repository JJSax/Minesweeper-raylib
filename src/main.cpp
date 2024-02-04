#include <raylib.h>
#include <string>
#include <iostream>

#include "grid.hpp"

int main() {

	// Init
	const float tileSize = 40;
	const int gWidth = 18;
	const int gHeight = 14;
	const int footHeight = 50;
	const int totalMines = 40;

	const Vector2 fieldSize = {gWidth * tileSize, gHeight * tileSize};

	InitWindow(fieldSize.x, fieldSize.y + footHeight, "Minesweeper");
	InitAudioDevice();

	bool newGamePressed = false;
	static const int newDimension = footHeight - 10;
	Rectangle newGameRect = {
		fieldSize.x - 10 - newDimension, fieldSize.y + 5,
		newDimension, newDimension
	};

	Grid field(gWidth, gHeight, tileSize, totalMines);
	Texture2D texture = LoadTexture("assets/tilemap.png");
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

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (CheckCollisionPointRec(GetMousePosition(), newGameRect)) {
				newGamePressed = true;
			}
		}
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			if (newGamePressed && CheckCollisionPointRec(GetMousePosition(), newGameRect)) {
				field.reset();
			}
			newGamePressed = false;
		}

		//update
		field.update();

		//draw
		BeginDrawing();
		ClearBackground(BLACK);

		field.render();

		DrawTexturePro(
			texture, {71, 428, 40, 40},
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

		// newDimension
		DrawRectanglePro(newGameRect, {0,0}, 0, LIGHTGRAY);
		bool alteredSides = false;
		if  (CheckCollisionPointRec(GetMousePosition(), newGameRect)) {
			bool active = IsMouseButtonDown(MOUSE_BUTTON_LEFT) && newGamePressed;
			float alpha = IsMouseButtonDown(active) ? 0.4 : 0.2;
			DrawRectanglePro(newGameRect, {0,0}, 0, Fade(GRAY, alpha));

			if (active) {
				DrawRectangle(newGameRect.x, newGameRect.y, 4, newGameRect.height, Fade(BLACK, 0.3));
				DrawRectangle(newGameRect.x, newGameRect.y, newGameRect.width, 4, Fade(BLACK, 0.3));
				alteredSides = true;
			}
		}
		if (!alteredSides) {
			Vector2 s = {newGameRect.x + newGameRect.width,
						newGameRect.y + newGameRect.height
			};
			DrawRectangle(s.x, s.y, -4, -newGameRect.height, WHITE);
			DrawRectangle(s.x, s.y, -newGameRect.width, -4, WHITE);
		}
		DrawTexturePro(texture, {180, 300, 60, 60}, newGameRect, {0, 0}, 0, WHITE);


		EndDrawing();
	}
	unload();
	UnloadTexture(texture);
	CloseAudioDevice();
	CloseWindow();


	return 0;
}

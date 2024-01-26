
#include <raylib.h>


int main() {
	InitWindow(500, 500, "Minesweeper");
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		//update
		//draw
		BeginDrawing();
		ClearBackground(BLACK);

		EndDrawing();
	}
	CloseWindow();

	return 0;
}
#pragma once

#include <vector>
#include <queue>
#include <raylib.h>
#include <random>
// #include <memory>

class Grid;
void load();
void unload();

enum class GAMESTATE {
	INIT,
	PLAYING,
	GAMEOVER,
	WIN,
};

class Cell {
protected:
	short int adjacentMines;
	Grid& grid;

public:
	Cell(int x, int y);
	~Cell();
	bool operator==(const Cell& other);

	const int x;
	const int y;
	bool hidden;
	bool mine;
	int spriteVal;
	bool flagged;
	void render(float tileSize);
	bool isMine();
	void dig();
	void toggleFlagged();
};

class Grid {
protected:
	int gHeight;
	int gWidth;
	float tileSize;
	std::vector<std::vector<Cell>> tiles;

private:
	int totalMines;
	const float REVEALTIMER = 0.05f;
	float revealTimer = REVEALTIMER;
	std::queue<std::vector<std::reference_wrapper<Cell>>> revealQueue;
	GAMESTATE state = GAMESTATE::GAMEOVER;
public:
	Grid(int gWidth, int gHeight, float tileSize, int totalMines);
	~Grid();


	bool hasFailed();
	bool isValid(int x, int y);
	Cell& getCell(int x, int y);
	void handleLeftClick(Vector2 pos);
	void update();
	void render();

	bool hasCellAtPixel(Vector2 pos);
	Cell& cellAtPixel(Vector2 pos);
	Cell& randomCell();

	void dig(Cell& cell);
	void dig(Vector2 position);

	void flag(Vector2 position);

	void createMap(Vector2 pos);
	void placeMines(Cell& clicked);

};

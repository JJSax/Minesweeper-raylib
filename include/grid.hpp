#pragma once

#include <vector>
#include <queue>
#include <raylib.h>
#include <random>
#include <set>
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

	/// @brief Toggle the cell's flag
	/// @return false if not hidden, true if hidden.
	bool toggleFlagged();
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
	// std::vector<std::reference_wrapper<Cell>> flaggedCells;
	std::set<std::pair<int, int>> flaggedCells;
	GAMESTATE state = GAMESTATE::GAMEOVER;
	short int totalFlags;
public:
	Grid(int gWidth, int gHeight, float tileSize, int totalMines);
	~Grid();


	bool hasFailed();
	bool isValid(int x, int y);
	Cell& getCell(int x, int y);
	int flagsAround(Cell& cell);
	void handleDigAround(Cell& cell);
	void handleDigAround(Vector2 pos);
	void handleLeftClick(Vector2 pos);
	void update();
	void render();

	bool hasCellAtPixel(Vector2 pos);
	Cell& cellAtPixel(Vector2 pos);
	Cell& randomCell();

	void dig(Cell& cell);
	void dig(Vector2 position);
	void digAdjacent(Cell& cell);

	void flag(Vector2 position);
	void placeMines(Cell& clicked);

	int getTotalFlags();
	bool stateWin();
	bool stateLose();

};

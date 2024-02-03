#pragma once

#include <vector>
#include <queue>
#include <raylib.h>
#include <random>
#include <set>
#include <unordered_set>
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
private:
	bool exploded;
	Rectangle quadOverride;
	int variant;
protected:
public:
	Cell(int x, int y);
	~Cell();
	void reset();
	bool operator==(const Cell& other);

	short int adjacentMines;
	const int x;
	const int y;
	bool hidden;
	bool revealed; // if not hidden or is in the revealQueue
	bool mine;
	int spriteVal;
	bool flagged;
	void render(float tileSize);
	bool isMine();
	void dig();

	/// @brief Toggle the cell's flag
	/// @return false if not revealed, true if revealed.
	bool toggleFlagged();
	void setBorders(Grid& grid);
};

class Grid {
private:
	int gHeight;
	int gWidth;
	float tileSize;
	int totalMines;
	std::vector<std::vector<Cell>> tiles;

	const float REVEALTIMER = 0.05f;
	float revealTimer = REVEALTIMER;
	int revealedCells;
	short int totalFlags;
	std::set<std::pair<int, int>> flaggedCells;
	std::queue<std::set<std::pair<int,int>>> revealQueue;
	std::vector<std::pair<int, int>> mines;
	GAMESTATE state = GAMESTATE::GAMEOVER;

	const float EXPOSETIMER = 0.1;
	float exposeTimer;
	int exposePos;
	std::set<std::pair<int,int>> getNextRevealLayer(Cell& cell);
	void updateExposingMines();
	void updateClearing();

protected:
public:

	/// @brief Digs cell if state is playing and the cell is hidden.
	/// @param cell The cell object to dig.
	void rawDig(Cell& cell);

	Grid(int gWidth, int gHeight, float tileSize, int totalMines);
	~Grid();
	void reset();
	// void debugDig(Vector2 pos);

	void dig(Cell& cell);
	void dig(Vector2 position);

	Cell& getCell(int x, int y);
	Cell& getCell(std::pair<int, int>);
	bool isValid(int x, int y);
	int flagsAround(Cell& cell);
	void handleDigAround(Cell& cell);
	void handleDigAround(Vector2 pos);
	void handleLeftClick(Vector2 pos);
	void update();
	void render();

	bool hasCellAtPixel(Vector2 pos);
	Cell& cellAtPixel(Vector2 pos);
	Cell& randomCell();

	void flag(Vector2 position);
	void placeMines(Cell& clicked);

	int getTotalFlags();
	bool stateWin();
	bool stateLose();
	void setBordersAround(Cell& cell);
	// void debugSetAllBorders();

};

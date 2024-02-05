#pragma once

#include <vector>
#include <queue>
#include <raylib.h>
#include <random>
#include <set>
#include <unordered_set>

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
	int spriteVal;
protected:
public:
	Cell(int x, int y);
	~Cell();
	void reset();
	bool operator==(const Cell& other);

	const int x;
	const int y;
	short int adjacentMines;
	bool mine;
	bool hidden;
	bool revealed; // if not hidden or is in the revealQueue
	bool flagged;
	void render(float tileSize);
	bool isMine();
	void dig();

	/**
	* @brief Toggle the cell's flag
	* @return If state changed
	*/
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

	const float REVEALTIMER = 0.05f; // Revealing blanks
	float revealTimer = REVEALTIMER;
	int revealedCells;
	short int totalFlags;
	std::unordered_set<int> flaggedCells; // hashed cell x/y
	std::queue<std::set<std::pair<int,int>>> revealQueue;
	std::vector<std::pair<int, int>> mines;
	GAMESTATE state = GAMESTATE::GAMEOVER;

	const float EXPOSETIMER = 0.1; // Exposing mines
	float exposeTimer;
	int exposePos;
	std::set<std::pair<int,int>> getNextRevealLayer(Cell& cell);
	void updateExposingMines();
	void updateClearing();
	void winGame();
	int hash(int x, int y);
	Cell& unhash(int hash);
	void placeMines(Cell& clicked);

	/// @brief Digs cell if state is playing and the cell is hidden.
	/// @param cell The cell object to dig.
	void rawDig(Cell& cell);
	void dig(Cell& cell);
	void dig(Vector2 position);

	bool hasCellAtPixel(Vector2 pos);
	Cell& cellAtPixel(Vector2 pos);
	Cell& randomCell();
	int flagsAround(Cell& cell);
	void setBordersAround(Cell& cell);
protected:
public:
	Grid(int gWidth, int gHeight, float tileSize, int totalMines);
	~Grid();
	void reset();

	Cell& getCell(int x, int y);
	Cell& getCell(std::pair<int, int>);
	bool isValid(int x, int y);

	void handleDigAround(Cell& cell);
	void handleDigAround(Vector2 pos);
	void handleLeftClick(Vector2 pos);
	void update();
	void render();

	void flag(Vector2 position);
	int getTotalFlags();

	bool stateWin();
	bool stateLose();
};

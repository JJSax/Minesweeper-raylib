#pragma once

#include <vector>
#include <queue>
#include <raylib.h>
#include <random>
// #include <memory>

class Grid;
void load();
void unload();

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
	void render(float tileSize);
	bool isMine();
	void dig();
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
public:
	Grid(int gWidth, int gHeight, float tileSize, int totalMines);
	~Grid();


	bool isValid(int x, int y);
	Cell& getCell(int x, int y);
	void update();
	void render();

	bool hasCellAtPixel(Vector2 pos);
	Cell& cellAtPixel(Vector2 pos);
	Cell& randomCell();

	void dig(Cell& cell);
	void dig(Vector2 position);


	void placeMines(Cell& clicked);

};

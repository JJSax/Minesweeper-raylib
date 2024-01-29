#pragma once

#include <vector>
#include <raylib.h>
#include <random>

class Grid;
void load();
void unload();

class Cell {
protected:
	bool hidden;
	short int adjacentMines;
	Grid& grid;

public:
	Cell(int x, int y);
	~Cell();
	bool operator==(const Cell& other);

	const int x;
	const int y;
	bool mine;
	int spriteVal;
	void render(float tileSize);
	bool isMine();
};

class Grid {
protected:
	int gHeight;
	int gWidth;
	float tileSize;
	std::vector<std::vector<Cell>> tiles;

private:
	int totalMines;
	bool hidden;
public:
	Grid(int gWidth, int gHeight, float tileSize, int totalMines);
	~Grid();


	bool isValid(int x, int y);
	Cell& getCell(int x, int y);
	void render();

	bool hasCellAtPixel(Vector2 pos);
	Cell& cellAtPixel(Vector2 pos);
	Cell& randomCell();

	void placeMines(Cell& clicked);

};

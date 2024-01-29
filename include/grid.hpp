#pragma once

#include <vector>
#include <raylib.h>

class Grid;
void load();
void unload();

class Cell {
protected:
	int x;
	int y;
	bool hidden;
	bool mine;
	short int adjacentMines;
	Grid& grid;

public:
	Cell(int x, int y);
	~Cell();
	Cell& operator=(const Cell& other);

	void setCoords(int x, int y);
	void render(float tileSize);
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
	Grid(int gWidth, int gHeight, float tileSize);
	~Grid();

	Cell getCell(int x, int y);
	void render();

};
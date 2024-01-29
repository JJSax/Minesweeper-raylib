#pragma once

#include <vector>

class Grid;

class Cell {
protected:
	int x;
	int y;
	Grid& grid;

public:
	Cell(int x, int y);
	~Cell();
	Cell& operator=(const Cell& other);

	void setCoords(int x, int y);
};

class Grid {
protected:
	int gHeight;
	int gWidth;
	std::vector<std::vector<Cell>> tiles;

private:
public:
	Grid(int gWidth, int gHeight);
	~Grid();

	Cell getCell(int x, int y);

};
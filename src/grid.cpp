#include "grid.hpp"

Cell::Cell(int x, int y) : grid(grid) {
	this->x = x;
	this->y = y;
}

Cell::~Cell() {}

void Cell::setCoords(int x, int y) {this->x = x; this->y = y;}

Cell& Cell::operator=(const Cell& other) {
	if (this != &other) {
		this->x = other.x;
		this->y = other.y;
	}
	return *this;
}

Grid::Grid(int gWidth, int gHeight) {
	this->gWidth = gWidth;
	this->gHeight = gHeight;

	for (int x = 0; x < gWidth; x++) {
		tiles.emplace_back();
		for (int y = 0; y < gHeight; y++) {
			tiles[x].emplace_back(x, y);
		}
	}
}

Grid::~Grid() {}

Cell Grid::getCell(int x, int y) {
	return tiles[x][y];
}
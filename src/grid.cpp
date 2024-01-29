#include "grid.hpp"
#include <unordered_map>
#include <string>
#include <iostream>

Texture2D spriteTex;

Rectangle spriteMap[13]; // would have loved to use unordered map with enum class keys...
std::unordered_map<std::string, int> spriteLabel;

void load() {

	spriteLabel["hidden"] = 9;
	spriteLabel["flag"] = 10;
	spriteLabel["exploded"] = 11;
	spriteLabel["mine"] = 12;
	spriteLabel["flagmine"] = 13;

	float sts = 20; // sprite tile size
	for (int i = 0; i <= 8; i++) {
		spriteMap[i] = {sts * i, sts, sts, sts};
	}
	for (int i = 9; i <= 13; i++) {
		spriteMap[i] = {sts * (i - 9), 0, sts, sts};
	}

	spriteTex = LoadTexture("assets/sprite.png");
}

void unload() {
	UnloadTexture(spriteTex);
}

Cell::Cell(int x, int y) : grid(grid) {
	this->x = x;
	this->y = y;
	this->hidden = true;
}

Cell::~Cell() {}

void Cell::setCoords(int x, int y) {this->x = x; this->y = y;}

void Cell::render(float tileSize) {
	Rectangle l = {x * tileSize, y * tileSize, tileSize, tileSize};
	if (hidden) {
		DrawTexturePro(spriteTex, spriteMap[9], l, {0, 0}, 0, WHITE);
	}
}

Cell& Cell::operator=(const Cell& other) {
	if (this != &other) {
		this->x = other.x;
		this->y = other.y;
	}
	return *this;
}

Grid::Grid(int gWidth, int gHeight, float tileSize) {
	this->gWidth = gWidth;
	this->gHeight = gHeight;
	this->tileSize = tileSize;

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

void Grid::render() {
	for (int x = 0; x < this->gWidth; x++) {
		for (int y = 0; y < this->gHeight; y++) {
			tiles[x][y].render(tileSize);
		}
	}
}
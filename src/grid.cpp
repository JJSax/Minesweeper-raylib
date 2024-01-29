#include "grid.hpp"
#include <unordered_map>
#include <string>
#include <iostream>
#include <random>
#include <optional>

enum Quad {
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	HIDDEN,
	FLAG,
	EXPLODE,
	MINE,
	FLAGMINE,
	QUAD_END
};

int randInt(int lower, int upper) {
	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(lower, upper); // define the range

	return distr(eng);
}
int randInt(int upper) {return randInt(0, upper);}

Texture2D spriteTex;
std::unordered_map<Quad, Rectangle> spriteMap;

Rectangle rectForSprite(int quad) {
	static const float sts = 20; // sprite tile size
	return {sts * quad, 0, sts, sts};
}

void load() {
	for (int q = ZERO; q < QUAD_END; q++) {
		spriteMap.emplace(static_cast<Quad>(q), rectForSprite(q));
	}
	spriteTex = LoadTexture("assets/sprite.png");
}

void unload() {
	UnloadTexture(spriteTex);
}

Cell::Cell(int x, int y) : grid(grid), x(x), y(y), mine(false) {
	this->hidden = false;
	this->spriteVal = 0;
}
Cell::~Cell() {}

void Cell::render(float tileSize) {
	Rectangle l = {x * tileSize, y * tileSize, tileSize, tileSize};
	if (hidden) {
		DrawTexturePro(spriteTex, spriteMap.at(HIDDEN), l, {0, 0}, 0, WHITE);
	} else {
		DrawTexturePro(spriteTex, spriteMap.at(static_cast<Quad>(spriteVal)), l, {0, 0}, 0, WHITE);
	}
}
bool Cell::isMine() {return mine;}

bool Cell::operator==(const Cell& other) {
	return y == other.y && x == other.x;
}





Grid::Grid(int gWidth, int gHeight, float tileSize, int totalMines)  {
	this->gWidth = gWidth;
	this->gHeight = gHeight;
	this->tileSize = tileSize;
	this->totalMines = totalMines;

	for (int x = 0; x < gWidth; x++) {
		tiles.emplace_back();
		for (int y = 0; y < gHeight; y++) {
			tiles[x].emplace_back(x, y);
		}
	}
	this->placeMines();
}
Grid::~Grid() {}

void Grid::placeMines() {
	for (int i = 0; i < totalMines; i++) {
		while (true) {
			Cell& rc = randomCell();
			if (rc.isMine()) continue;

			// Only here when it can place the mine
			rc.spriteVal = MINE;
			rc.mine = true;

			// set adjacent numbers
			for (int x = rc.x - 1; x <= rc.x + 1; x++) {
				for (int y = rc.y - 1; y <= rc.y + 1; y++) {
					if (x == rc.x && y == rc.y) continue;
					if (!isValid(x, y)) continue;
					Cell& adjacent = getCell(x, y);
					if (adjacent.spriteVal < EIGHT) {
						adjacent.spriteVal++;
					}
				}
			}

			break;
		}
	}
}

bool Grid::isValid(int x, int y) {
	return 0 <= x && x < tiles.size() && 0 <= y && y < tiles[x].size();
}

Cell& Grid::getCell(int x, int y) {
	return tiles.at(x).at(y);
}

void Grid::render() {
	std::optional<Cell> hovered;
	if (hasCellAtPixel(GetMousePosition()))
		hovered.emplace(cellAtPixel(GetMousePosition()));
	for (int x = 0; x < this->gWidth; x++) {
		for (int y = 0; y < this->gHeight; y++) {
			tiles[x][y].render(tileSize);
			if (hovered && tiles[x][y] == hovered.value()) {
				DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, Fade(WHITE, 0.1));
			}
		}
	}
}

bool Grid::hasCellAtPixel(Vector2 pos) {
	return pos.y < gHeight * tileSize;
}

Cell& Grid::cellAtPixel(Vector2 pos) {
	return tiles.at(pos.x / tileSize).at(pos.y / tileSize);
}

Cell& Grid::randomCell() {
	int w = randInt(gWidth - 1);
	int h = randInt(gHeight - 1);
	// std::cout << w << ": " << h << std::endl;
	return tiles[w][h];
}

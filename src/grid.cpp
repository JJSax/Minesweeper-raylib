#include "grid.hpp"
#include <unordered_map>
#include <unordered_set>
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
Sound kaboom;
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
	kaboom = LoadSound("assets/Big_Explosion_Cut_Off.mp3");
}
void unload() {
	UnloadTexture(spriteTex);
	UnloadSound(kaboom);
}



Cell::Cell(int x, int y) : grid(grid), x(x), y(y), mine(false) {
	hidden = true;
	spriteVal = 0;
	flagged = false;
}
Cell::~Cell() {}

void Cell::render(float tileSize) {
	Rectangle l = {x * tileSize, y * tileSize, tileSize, tileSize};
	Rectangle quad = spriteMap.at(static_cast<Quad>(spriteVal));
	//!DEBUG
	if (IsKeyDown(KEY_LEFT_ALT)) {
		DrawTexturePro(spriteTex, quad, l, {0, 0}, 0, WHITE);
		return;
	}
	if (hidden) quad = spriteMap.at(HIDDEN);
	if (flagged) quad = spriteMap.at(FLAG);
	DrawTexturePro(spriteTex, quad, l, {0, 0}, 0, WHITE);
}
bool Cell::isMine() {return mine;}

void Cell::dig() {
	hidden = false;
	if (!mine) return;
	spriteVal = EXPLODE;
	PlaySound(kaboom);
}

bool Cell::toggleFlagged() {
	if (!hidden) return false;
	flagged = !flagged;
	return true;
}

bool Cell::operator==(const Cell& other) {
	return y == other.y && x == other.x;
}





Grid::Grid(int gWidth, int gHeight, float tileSize, int totalMines)  {
	this->gWidth = gWidth;
	this->gHeight = gHeight;
	this->tileSize = tileSize;
	this->totalMines = totalMines;
	this->state = GAMESTATE::INIT;
	this->totalFlags = 0;

	for (int x = 0; x < gWidth; x++) {
		tiles.emplace_back();
		for (int y = 0; y < gHeight; y++) {
			tiles[x].emplace_back(x, y);
		}
	}
}
Grid::~Grid() {}

void Grid::placeMines(Cell& clicked) {
	for (int i = 0; i < totalMines; i++) {
		while (true) {
			Cell& rc = randomCell();
			if (rc.isMine()) continue;
			if (std::abs(clicked.x - rc.x) <= 1 && std::abs(clicked.y - rc.y) <= 1) continue;

			// The code only gets here when it can place the mine
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

bool Grid::hasFailed() {return state == GAMESTATE::GAMEOVER;}

bool Grid::isValid(int x, int y) {
	return 0 <= x && x < tiles.size() && 0 <= y && y < tiles[x].size();
}

Cell& Grid::getCell(int x, int y) {
	return tiles.at(x).at(y);
}

int Grid::flagsAround(Cell& cell) {
	int t = 0;
	for (int x = cell.x - 1; x <= cell.x + 1; x++) {
		for (int y = cell.y - 1; y <= cell.y + 1; y++) {
			if (cell.x == x && cell.y == y) continue;
			if (!isValid(x, y)) continue;
			if (getCell(x, y).flagged) t++;
		}
	}
	return t;
}

void Grid::handleDigAround(Cell& cell) {
	if (cell.hidden || flagsAround(cell) != cell.spriteVal) return;
	for (int x = cell.x - 1; x <= cell.x + 1; x++) {
		for (int y = cell.y - 1; y <= cell.y + 1; y++) {
			if (cell.x == x && cell.y == y) continue;
			if (!isValid(x, y)) continue;
			Cell& c = getCell(x, y);
			if (c.flagged) continue;
			dig(c);
		}
	}
}
void Grid::handleDigAround(Vector2 pos) {
	handleDigAround(cellAtPixel(pos));
}

void Grid::handleLeftClick(Vector2 pos) {
	if (!hasCellAtPixel(pos)) return;
	if (state == GAMESTATE::INIT) {
		placeMines(cellAtPixel(pos));
		state = GAMESTATE::PLAYING;
	}
	dig(pos);
}

void Grid::update() {
	if (revealQueue.empty()) return;
	revealTimer -= GetFrameTime();
	if (revealTimer > 0) return;

	revealTimer = REVEALTIMER;
	// reveal first layer of revealQueue.
	std::vector<std::reference_wrapper<Cell>> nextLayer;
	for (Cell& cell : revealQueue.front()) {
		if (!cell.flagged) cell.dig();
		if (cell.mine) state = GAMESTATE::GAMEOVER;
		if (cell.spriteVal != ZERO) continue;
		for (int x = cell.x - 1; x <= cell.x + 1; x++) {
			for (int y = cell.y - 1; y <= cell.y + 1; y++) {
				if (!isValid(x, y)) continue;
				if (!getCell(x, y).hidden) continue;
				if (getCell(x, y).flagged) continue;
				nextLayer.emplace_back(getCell(x, y));
			}
		}
	}
	revealQueue.pop();
	if (!nextLayer.empty()) revealQueue.push(nextLayer);
}

void Grid::render() {
	std::optional<Cell> hovered;
	if (hasCellAtPixel(GetMousePosition())) {
		hovered.emplace(cellAtPixel(GetMousePosition()));
	}
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
	return tiles.at(randInt(gWidth - 1)).at(randInt(gHeight - 1));
}

void Grid::dig(Cell& cell) {
	if (state == GAMESTATE::GAMEOVER || state == GAMESTATE::WIN) return;
	if (cell.flagged) return;

	cell.dig();
	if (cell.isMine()) state = GAMESTATE::GAMEOVER;
	if (cell.spriteVal != ZERO) return;

	std::vector<std::reference_wrapper<Cell>> neighbors;
	for (int x = cell.x - 1; x <= cell.x + 1; x++) {
		for (int y = cell.y - 1; y <= cell.y + 1; y++) {
			if (!isValid(x, y)) continue;
			if (!getCell(x, y).hidden) continue;
			if (getCell(x, y).flagged) continue;
			neighbors.emplace_back(getCell(x, y));
		}
	}
	revealQueue.push(neighbors);
}
void Grid::dig(Vector2 pos) {dig(cellAtPixel(pos));}

void Grid::flag(Vector2 pos) {
	if (state != GAMESTATE::PLAYING) return;
	if (!hasCellAtPixel(pos)) return;
	Cell& c = cellAtPixel(pos);
	if (!c.toggleFlagged()) return;

	if (c.flagged) {
		flaggedCells.insert(std::make_pair(c.x, c.y));
		totalFlags++;
	} else {
		flaggedCells.erase(std::make_pair(c.x, c.y));
		totalFlags--;
	}

	if (totalFlags != totalMines) return;
	// check if all the flags are on mines
	bool win = true;
	for (auto c : flaggedCells) {
		if (!getCell(c.first, c.second).isMine()) {
			win = false;
			break;
		}
	}
	if (win) state = GAMESTATE::WIN;
}

int Grid::getTotalFlags() {return totalFlags;}
bool Grid::stateWin() {return state == GAMESTATE::WIN;}
bool Grid::stateLose() {return state == GAMESTATE::GAMEOVER;}
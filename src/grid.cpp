#include "grid.hpp"
#include "randUtils.hpp"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <random>
#include <optional>

enum Wall {
    TOP = 1 << 0,
    RIGHT = 1 << 1,
    BOTTOM = 1 << 2,
    LEFT = 1 << 3
};

enum SpecialQuad {
	FLAG,
	EXPLODE,
	MINE
};


Texture2D tileMapTexture;
Sound kaboom;
Sound flagPlace;
Sound flagPop;
Sound digSound;
Sound clear;
Sound clear2;
Sound begin;
Sound win;
std::unordered_map<int, std::unordered_map<int, Rectangle>> spriteMap;
std::unordered_map<int, Color> adjacentIndicatorColor;
std::unordered_map<SpecialQuad, Rectangle> specialSpriteMap;

void load() {
	adjacentIndicatorColor = {
		{1, BLUE},
		{2, GREEN},
		{3, YELLOW},
		{4, ORANGE},
		{5, PINK},
		{6, RED},
		{7, PURPLE},
		{8, BLACK}
	};

	static const float sts = 60; // sprite tile size
	for (int i = 0; i <= 1; i++) {
		int v = i + 2;
		spriteMap.emplace(i, std::unordered_map<int, Rectangle>());
		spriteMap.emplace(v, std::unordered_map<int, Rectangle>());
		for (int j = 0; j < 16; j++) {
			spriteMap.at(i).emplace(j, Rectangle{sts * j, i*sts, sts, sts});
			spriteMap.at(v).emplace(j, Rectangle{sts * j, v*sts, sts, sts});
		}
	}
	specialSpriteMap.emplace(FLAG, Rectangle{0, 420, sts, sts});
	specialSpriteMap.emplace(EXPLODE, Rectangle{60, 300, sts, sts});
	specialSpriteMap.emplace(MINE, Rectangle{120, 300, sts, sts});

	tileMapTexture = LoadTexture("assets/tilemap.png");
	kaboom = LoadSound("assets/Big_Explosion_Cut_Off.mp3");
	flagPlace = LoadSound("assets/flag.mp3");
	flagPop = LoadSound("assets/flagPop.mp3");
	digSound = LoadSound("assets/shovel.mp3");
	clear = LoadSound("assets/clear.mp3");
	clear2 = LoadSound("assets/clear(2).mp3");
	begin = LoadSound("assets/begin.mp3");
	win = LoadSound("assets/win.mp3");
}
void unload() {
	UnloadTexture(tileMapTexture);
	UnloadSound(kaboom);
	UnloadSound(flagPlace);
	UnloadSound(flagPop);
	UnloadSound(clear);
	UnloadSound(clear2);
	UnloadSound(begin);
	UnloadSound(win);
}


////////////////////////////////////////////////////////////

void Cell::reset() {
	hidden = true;
	spriteVal = 0;
	flagged = false;
	mine = false;
	exploded = false;
	adjacentMines = 0;
	revealed = false;
};

Cell::Cell(int x, int y) : x(x), y(y) {
	variant = randInt(15);
	reset();
}
Cell::~Cell() {}

void drawQuad(Rectangle quad, Rectangle at) {
	DrawTexturePro(tileMapTexture, quad, at, {0, 0}, 0, WHITE);
}
void Cell::render(float tileSize) {
	Rectangle l = {x * tileSize, y * tileSize, tileSize, tileSize};
	drawQuad(spriteMap.at(!hidden).at(spriteVal), l);
	drawQuad(spriteMap.at(!hidden + 2).at(variant), l);
	if (exploded) drawQuad(specialSpriteMap.at(EXPLODE), l);
	if (mine && !hidden && !exploded) drawQuad(specialSpriteMap.at(MINE), l);
	if (flagged) drawQuad(specialSpriteMap.at(FLAG), l);

	DrawRectangleLinesEx(l, 1.0f, Fade(BLACK, 0.2));
	if (hidden || (adjacentMines == 0) || mine) return;
	const char *n = TextFormat("%i", adjacentMines);
	int font = 26;
	int w = MeasureText(n, font);
	DrawText(n,
		l.x + tileSize / 2 - w/2, l.y + tileSize / 2 - font/2,
		font, adjacentIndicatorColor.at(adjacentMines)
	);
}
bool Cell::isMine() {return mine;}

bool matchingCellNeighbor(Grid& grid, Cell& cell, int x, int y) {
	return (grid.isValid(x, y) && grid.getCell(x, y).hidden == cell.hidden);
}
void Cell::setBorders(Grid& grid) {
	spriteVal = 0b0000;
	if (!matchingCellNeighbor(grid, *this, x, y-1)) spriteVal |= TOP;
	if (!matchingCellNeighbor(grid, *this, x+1, y)) spriteVal |= RIGHT;
	if (!matchingCellNeighbor(grid, *this, x, y+1)) spriteVal |= BOTTOM;
	if (!matchingCellNeighbor(grid, *this, x-1, y)) spriteVal |= LEFT;
}

void Cell::dig() {
	hidden = false;
	if (!mine) return;
	quadOverride = specialSpriteMap.at(EXPLODE);
	exploded = true;
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
	this->revealedCells = 0;
	this->exposeTimer = 0;
	this->exposePos = 0;
	this->soundsOn = true;

	for (int x = 0; x < gWidth; x++) {
		tiles.emplace_back();
		for (int y = 0; y < gHeight; y++) {
			tiles[x].emplace_back(x, y);
		}
	}

	for (int x = 0; x < gWidth; x++) {
		for (int y = 0; y < gHeight; y++) {
			tiles.at(x).at(y).setBorders(*this);
		}
	}
}
Grid::~Grid() {}
int Grid::hash(int x, int y) { return x * 14 + y; }
Cell& Grid::unhash(int hash) { return getCell(hash/14, hash % 14); };

void Grid::reset() {
	if (state == GAMESTATE::INIT) return;

	for (int x = 0; x < gWidth; x++) {
		for (int y = 0; y < gHeight; y++) {
			getCell(x, y).reset();
		}
	}
	for (int x = 0; x < gWidth; x++) {
		for (int y = 0; y < gHeight; y++) {
			tiles.at(x).at(y).setBorders(*this);
		}
	}

	state = GAMESTATE::INIT;
	totalFlags = 0;
	flaggedCells = {};
	revealedCells = {};
	revealQueue = {};
	mines = {};
	exposePos = 0;
	playSound(begin);
}

void Grid::setBordersAround(Cell& cell) {
	for (int x = cell.x -1; x <= cell.x + 1; x++) {
		for (int y = cell.y -1; y <= cell.y + 1; y++) {
			if (!isValid(x, y)) continue;
			getCell(x, y).setBorders(*this);
		}
	}
}

void Grid::placeMines(Cell& clicked) {
	for (int i = 0; i < totalMines; i++) {
		while (true) {
			Cell& rc = randomCell();
			if (rc.isMine()) continue;
			if (std::abs(clicked.x - rc.x) <= 1 && std::abs(clicked.y - rc.y) <= 1) continue;

			// The code only gets here when it can place the mine
			rc.mine = true;
			mines.emplace_back(rc.x, rc.y);

			// set adjacent numbers
			for (int x = rc.x - 1; x <= rc.x + 1; x++) {
				for (int y = rc.y - 1; y <= rc.y + 1; y++) {
					if (x == rc.x && y == rc.y) continue;
					if (!isValid(x, y)) continue;
					Cell& adjacent = getCell(x, y);
					adjacent.adjacentMines++;
				}
			}
			break;
		}
	}
}


void Grid::playSound(Sound sound) {
	if (!soundsOn) return;
	PlaySound(sound);
}
void Grid::toggleSounds() { soundsOn = !soundsOn; }

void Grid::winGame() {
	state = GAMESTATE::WIN;
	playSound(win);
}

void Grid::rawDig(Cell& cell) {
	// if (!cell.hidden) throw std::runtime_error("Can't dig a tile that is already dug");
	if (!cell.hidden) return;
	cell.dig();
	setBordersAround(cell);
	if (cell.isMine()) {
		state = GAMESTATE::GAMEOVER;
		playSound(kaboom);
		return;
	}
	revealedCells++;
	if (revealedCells + totalMines == gWidth * gHeight) winGame();
}

std::set<std::pair<int,int>> Grid::getNextRevealLayer(Cell& cell) {
	std::set<std::pair<int,int>> nextLayer;
	for (int x = cell.x - 1; x <= cell.x + 1; x++) {
		for (int y = cell.y - 1; y <= cell.y + 1; y++) {
			if (!isValid(x, y)) continue;
			Cell& cell = getCell(x, y);
			if (!cell.hidden) continue;
			if (cell.flagged) continue;
			if (cell.revealed) continue;
			cell.revealed = true;
			nextLayer.emplace(std::pair(x, y));
		}
	}
	return nextLayer;
}

void Grid::dig(Cell& cell) {
	if (state == GAMESTATE::GAMEOVER || state == GAMESTATE::WIN) return;
	if (cell.flagged) return;
	if (!cell.hidden) return;
	cell.revealed = true;
	rawDig(cell);
	if (cell.adjacentMines > 0 || cell.isMine()) return;
	playSound(randInt(1) ? clear : clear2);
	revealQueue.push(getNextRevealLayer(cell));
}
void Grid::dig(Vector2 pos) {dig(cellAtPixel(pos));}

bool Grid::isValid(int x, int y) { return 0 <= x && x < gWidth && 0 <= y && y < gHeight; }
bool Grid::hasCellAtPixel(Vector2 pos) { return pos.y < gHeight * tileSize; }
Cell& Grid::getCell(int x, int y) { return tiles.at(x).at(y); }
Cell& Grid::getCell(std::pair<int, int> pos) {return getCell(pos.first, pos.second);}
Cell& Grid::cellAtPixel(Vector2 pos) { return getCell(pos.x / tileSize, pos.y / tileSize); }
Cell& Grid::randomCell() { return getCell(randInt(gWidth - 1), randInt(gHeight - 1)); }

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
	if (cell.hidden || flagsAround(cell) != cell.adjacentMines) return;
	bool dug = false;
	for (int x = cell.x - 1; x <= cell.x + 1; x++) {
		for (int y = cell.y - 1; y <= cell.y + 1; y++) {
			if (cell.x == x && cell.y == y) continue;
			if (!isValid(x, y)) continue;
			Cell& c = getCell(x, y);
			if (c.flagged) continue;
			if (!c.hidden) continue;
			c.revealed = true;
			dig(c);
			dug = true;
		}
	}
	if (dug) playSound(digSound);
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
	if (cellAtPixel(pos).hidden && state == GAMESTATE::PLAYING) playSound(digSound);
	dig(pos);
}

void Grid::updateClearing() {
	if (revealQueue.empty()) return;
	revealTimer -= GetFrameTime();
	if (revealTimer > 0) return;

	revealTimer = REVEALTIMER;
	// reveal first layer of revealQueue.
	std::set<std::pair<int, int>> nextLayer;
	for (const auto cPair : revealQueue.front()) {
		Cell& cell = getCell(cPair);
		if (!cell.flagged) {
			cell.revealed = true; // might already be true
			rawDig(cell);
			if (cell.adjacentMines > 0) continue;
		}
		nextLayer.merge(getNextRevealLayer(cell));
	}
	revealQueue.pop();
	if (!nextLayer.empty()) revealQueue.push(nextLayer);
}

void Grid::updateExposingMines() {
	if (state != GAMESTATE::GAMEOVER) return;
	if (exposePos == totalMines) return;

	exposeTimer += GetFrameTime();
	if (exposeTimer >= EXPOSETIMER) {
		exposeTimer = 0;
		Cell& cell = getCell(mines.at(exposePos));
		exposePos++;
		cell.hidden = false;
		setBordersAround(cell);
	}
}
void Grid::update() {
	updateClearing();
	updateExposingMines();
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
	if (revealQueue.empty()) return;
	for (const auto cPair : revealQueue.front()) {
		Cell& c = getCell(cPair);
		DrawRectangle(c.x * tileSize, c.y * tileSize, tileSize, tileSize, Fade(GREEN, 0.5));
	}
}

void Grid::flag(Vector2 pos) {
	if (state != GAMESTATE::PLAYING) return;
	if (!hasCellAtPixel(pos)) return;
	Cell& c = cellAtPixel(pos);
	if (!c.toggleFlagged()) return;

	if (c.flagged) {
		flaggedCells.insert(hash(c.x, c.y));
		totalFlags++;
		playSound(flagPlace);
	} else {
		flaggedCells.erase(hash(c.x, c.y));
		totalFlags--;
		playSound(flagPop);
	}

	//* the following code is for allowing victory by correct flag placements
	//* this took out challenge at the ends of playthoughs.
	// if (totalFlags != totalMines) return;
	// // check if all the flags are on mines
	// bool win = true;
	// for (auto c : flaggedCells) {
	// 	if (!unhash(c).isMine()) {
	// 		win = false;
	// 		break;
	// 	}
	// }
	// if (win) winGame();
}

int Grid::getTotalFlags() {return totalFlags;}
bool Grid::stateWin() {return state == GAMESTATE::WIN;}
bool Grid::stateLose() {return state == GAMESTATE::GAMEOVER;}

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


// Texture2D spriteTex;
Texture2D tileMapTexture;
Sound kaboom;
std::unordered_map<int, std::unordered_map<int, Rectangle>> spriteMap;
std::unordered_map<int, Color> adjacentIndicatorColor;
std::unordered_map<SpecialQuad, Rectangle> specialSpriteMap;

void load() {
	adjacentIndicatorColor = {
		{0, Fade(WHITE, 0)},
		{1, BLUE},
		{2, GREEN},
		{3, YELLOW},
		{4, ORANGE},
		{5, PINK},
		{6, RED},
		{7, PURPLE},
		{8, BLACK}
	};
	tileMapTexture = LoadTexture("assets/tilemap.png");
	static const float sts = 60; // sprite tile size
	for (int i = 0; i <= 1; i++) {
		spriteMap.emplace(i, std::unordered_map<int, Rectangle>());
		for (int j = 0; j < 16; j++) {
			spriteMap.at(i).emplace(j, Rectangle{sts * j, i*sts, sts, sts});
		}
	}
	specialSpriteMap.emplace(FLAG, Rectangle{0, 420, sts, sts});
	specialSpriteMap.emplace(EXPLODE, Rectangle{60, 300, sts, sts});
	specialSpriteMap.emplace(MINE, Rectangle{120, 300, sts, sts});

	kaboom = LoadSound("assets/Big_Explosion_Cut_Off.mp3");
}
void unload() {
	UnloadSound(kaboom);
	UnloadTexture(tileMapTexture);
}

////////////////////////////////////////////////////////////

Cell::Cell(int x, int y) : x(x), y(y) {
	hidden = true;
	spriteVal = 0;
	flagged = false;
	mine = false;
	exploded = false;
	adjacentMines = 0;
	revealed = false;
}
Cell::~Cell() {}

void drawQuad(Rectangle quad, Rectangle at) {
	DrawTexturePro(tileMapTexture, quad, at, {0, 0}, 0, WHITE);
}
void Cell::render(float tileSize) {
	Rectangle l = {x * tileSize, y * tileSize, tileSize, tileSize};
	Rectangle quad = spriteMap.at(!hidden).at(spriteVal);
	if (exploded) quad = quadOverride;
	drawQuad(quad, l);
	if (mine && !hidden && !exploded) drawQuad(specialSpriteMap.at(MINE), l);
	if (flagged) drawQuad(specialSpriteMap.at(FLAG), l);

	DrawRectangleLinesEx(l, 1.0f, Fade(BLACK, 0.2));
	if (hidden) return;
	if (adjacentMines > 0 && !mine) {
		const char *n = TextFormat("%i", adjacentMines);
		int font = 26;
		int w = MeasureText(n, font);
		DrawText(n,
			l.x + tileSize / 2 - w/2, l.y + tileSize / 2 - font/2,
			font, adjacentIndicatorColor.at(adjacentMines)
		);
	}
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
	// spriteVal = EXPLODE;
	quadOverride = specialSpriteMap.at(EXPLODE);
	exploded = true;
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
	this->revealedCells = 0;
	this->exposeTimer = 0;
	this->exposePos = 0;

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

void Grid::rawDig(Cell& cell) {
	// if (!cell.hidden) throw std::runtime_error("Can't dig a tile that is already dug");
	cell.dig();
	setBordersAround(cell);
	if (cell.isMine()) {
		state = GAMESTATE::GAMEOVER;
		return;
	}
	revealedCells++;
	if (revealedCells + totalMines == gWidth * gHeight) state = GAMESTATE::WIN;
	if (cell.adjacentMines == 0) return;
	return;
}

void Grid::dig(Cell& cell) {
	if (state == GAMESTATE::GAMEOVER || state == GAMESTATE::WIN) return;
	if (cell.flagged) return;
	if (!cell.hidden) return;
	rawDig(cell);
	cell.revealed = true;
	if (cell.adjacentMines > 0) return;

	std::set<std::pair<int,int>> neighbors;

	//! candidate to move to it's own function,  also used later in update clearing
	for (int x = cell.x - 1; x <= cell.x + 1; x++) {
		for (int y = cell.y - 1; y <= cell.y + 1; y++) {
			if (!isValid(x, y)) continue;
			Cell& cell = getCell(x, y);
			if (!cell.hidden) continue;
			if (cell.flagged) continue;
			if (cell.revealed) continue;
			cell.revealed = true;
			neighbors.emplace(std::pair(x, y));
		}
	}
	revealQueue.push(neighbors);
}
void Grid::dig(Vector2 pos) {dig(cellAtPixel(pos));}

bool Grid::hasFailed() {return state == GAMESTATE::GAMEOVER;}

bool Grid::isValid(int x, int y) {
	return 0 <= x && x < tiles.size() && 0 <= y && y < tiles[x].size();
}

Cell& Grid::getCell(int x, int y) { return tiles.at(x).at(y); }
Cell& Grid::getCell(std::pair<int, int> pos) {return getCell(pos.first, pos.second);}

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
	for (int x = cell.x - 1; x <= cell.x + 1; x++) {
		for (int y = cell.y - 1; y <= cell.y + 1; y++) {
			if (cell.x == x && cell.y == y) continue;
			if (!isValid(x, y)) continue;
			Cell& c = getCell(x, y);
			if (c.flagged) continue;
			c.revealed = true;
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
			rawDig(cell);
			cell.revealed = true; // might already be true
			if (cell.adjacentMines > 0) continue;
		}
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

bool Grid::hasCellAtPixel(Vector2 pos) {
	return pos.y < gHeight * tileSize;
}

Cell& Grid::cellAtPixel(Vector2 pos) {
	return tiles.at(pos.x / tileSize).at(pos.y / tileSize);
}

Cell& Grid::randomCell() {
	return tiles.at(randInt(gWidth - 1)).at(randInt(gHeight - 1));
}

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
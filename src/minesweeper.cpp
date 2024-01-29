
#include <unordered_map>
#include <string>
#include <iostream>

#include "minesweeper.hpp"

using namespace MS;

// Image spritesheet;
Texture2D spriteTex;

Rectangle spriteMap[13]; // would have loved to use unordered map with enum class keys...
std::unordered_map<std::string, int> spriteLabel;

void MS::load() {

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

void MS::unload() {
	UnloadTexture(spriteTex);
}

Minefield::Minefield(int gWidth, int gHeight, float cellSize)
	: Grid(gWidth, gHeight) {
		this->tileSize = cellSize;
}

// Minefield::Minefield(int gWidth, int gHeight, float cellSize) {
// 	this->gWidth = gWidth;
// 	this->gHeight = gHeight;

// 	for (int x = 0; x < gWidth; x++) {
// 		tiles.emplace_back();
// 		for (int y = 0; y < gHeight; y++) {
// 			tiles[x].emplace_back(*this, x, y);
// 		}
// 	}
// }

Minefield::~Minefield() {}

void Minefield::render() {
	// std::cout << spriteTex.height << std::endl;

	for (int x = 0; x < this->gWidth; x++) {
		for (int y = 0; y < this->gHeight; y++) {
			// tiles[x][y].render(this->tileSize);
			Rectangle l = {x * tileSize, y * tileSize, tileSize, tileSize};


			// DrawRectangleLines(x * tileSize, y * tileSize, tileSize, tileSize, WHITE);
			// DrawTextureRec(spriteTex, spriteMap[9], {0, 0}, WHITE);
			DrawTexturePro(spriteTex, spriteMap[9], l, {0, 0}, 0, WHITE);
		}
	}
}



void MC::render(float tileSize) {
	Rectangle l = {x * tileSize, y * tileSize, tileSize, tileSize};
	if (hidden) {
		DrawTexturePro(spriteTex, spriteMap[9], l, {0, 0}, 0, WHITE);
	}
}
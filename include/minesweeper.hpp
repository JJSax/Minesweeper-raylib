#pragma once

#include <vector>
#include <raylib.h>

#include "grid.hpp"

namespace MS {

	class Minefield;

	void load();
	void unload();

	enum class Quad {
		ZERO,
		ONE,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		COVER,
		MINE,
		FLAG,
		EXPLODE,
		FLAGMING
	};

	class MC : Cell {
	private:
		bool mine;
		short int adjacentMines;
	public:
		bool hidden;
		void render(float tileSize);
	};

	class Minefield : Grid {
	private:
		int totalMines;
		bool hidden;
		std::vector<std::vector<MC>> tiles;
	public:
		float tileSize;

		void render();
		Minefield(int width, int height, float cellSize);
		~Minefield();
	};

}

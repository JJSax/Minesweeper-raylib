#pragma once

#include <random>

int randInt(int lower, int upper) {
	static std::random_device rd; // obtain a random number from hardware
	static std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(lower, upper); // define the range

	return distr(eng);
}
int randInt(int upper) {return randInt(0, upper);}
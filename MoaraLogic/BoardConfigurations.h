#pragma once
#include <vector>

using BoardConfigMatrix = std::vector<std::vector<int>>;

const BoardConfigMatrix NORMAL_BOARD_CONFIG = {
		{0, -1, 1, 7, -1, -1},
		{1, -1, 2, 9, 0, -1},
		{2, -1, -1, 3, 1, -1},
		{3, 2, -1, 4, 11, -1},
		{4, 3, -1, -1, 5, -1},
		{5, 13, 4, -1, 6, -1},
		{6, 7, 5, -1, -1, -1},
		{7, 0, 15, 6, -1, -1},
		{8, -1, 9, 15, -1, -1},
		{9, 1, 10, 17, 8, -1},
		{10, -1, -1, 11, 9, -1},
		{11, 10, 3, 12, 19, -1},
		{12, 11, -1, -1, 13, -1},
		{13, 21, 12, 5, 14, -1},
		{14, 15, 13, -1, -1, -1},
		{15, 8, 23, 14, 7, -1},
		{16, -1, 17, 23, -1, -1},
		{17, 9, 18, -1, 16, -1},
		{18, -1, -1, 19, 17, -1},
		{19, 18, 11, 20, -1, -1},
		{20, 19, -1, -1, 21, -1},
		{21, -1, 20, 13, 22, -1},
		{22, 23, 21, -1, -1, -1},
		{23, 16, -1, 22, 15, -1}
};

const BoardConfigMatrix NORMAL_BIG_BOARD_CONFIG = {
		//TO DO
};

const BoardConfigMatrix DIAGONALS_NORMAL_BOARD_CONFIG = {
		//TO DO
};

const BoardConfigMatrix DIAGONALS_BIG_BOARD_CONFIG = {
		//TO DO
};

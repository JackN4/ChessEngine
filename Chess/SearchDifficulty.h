#pragma once
#include "Search.h"
class SearchDifficulty
{
	enum difficulties {easy, medium ,hard};
	int depths [3] = { 3,4,5 };
	Search search;

public: Move search_diff(Board& board, int diff) {
	return search.negamax_iter(board, depths[diff]);
}

};


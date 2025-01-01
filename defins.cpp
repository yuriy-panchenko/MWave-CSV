#include "pch.h"

void MWINFO::operator+=(const MWINFO& oth)
{
	PProfit += oth.PProfit;
	maxDD += oth.maxDD;
	Profit += oth.Profit;
	Loss += oth.Loss;
	iWin += oth.iWin;
	iLose += oth.iLose;
}

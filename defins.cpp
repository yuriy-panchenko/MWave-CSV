#include "pch.h"

MWINFO::MWINFO(mwave::Pattern pat, const MWAVE& mw)
{
	*this = {};

	Profit = pat.is_m() ? mw.next_leg.value - mw.leg.value : mw.leg.value - mw.next_leg.value;

	if (Profit > .0)
		iWin = 1;
	else
	{
		Loss = -Profit;
		iLose = 1;
		Profit = .0;
	}
}

void MWINFO::operator+=(const MWINFO& oth)
{
	PProfit += oth.PProfit;
	maxDD += oth.maxDD;
	
	Profit += oth.Profit;
	Loss += oth.Loss;
	
	iWin += oth.iWin;
	iLose += oth.iLose;
}

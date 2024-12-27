#pragma once
#include "seq.h"

struct PNT
{
	double value;
	int index;
};

struct MWAVE
{
	PNT mw[5], leg, next_leg;
	double PProfit, maxDD;
};

struct QUOTE_REC
{
	CTime time;
	double open, high, low, close;
	int volTick, volume, spread;
};

struct MWInfo { double PProfit, maxDD, Profit, Loss; };

struct MWaveTrade
{
	seq::leaf l;
	MWInfo i;
};

//using TradableTree = std::vector<MWaveTrade>;

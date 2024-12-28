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
	double Open, high, low, Close;
	int volTick, volume, spread;
};

struct MWInfo { double PProfit, maxDD, Profit, Loss; };

template<typename T>
CArchive& operator<<(CArchive& ar, const std::vector<T>& v)
{
	const uint64_t size{ v.size() };
	ar.Write(&size, sizeof size);
	ar.Write(v.data(), UINT(size * sizeof(T)));
	return ar;
}

template<typename T>
CArchive& operator>>(CArchive& ar, std::vector<T>& v)
{
	uint64_t size;
	ar.Read(&size, sizeof size);
	v.resize(size);
	ar.Read(v.data(), UINT(size * sizeof(T)));
	return ar;
}


#pragma once

class Trader
{
public:
	using TradePoint = std::pair<CTime, double>;

public:

	void Open(BOOL isBuy, const TradePoint&);
	void Close(const TradePoint&);
};

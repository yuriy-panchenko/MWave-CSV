#include "pch.h"
#include "fwd_leaf.h"

namespace fwd
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	leaf::leaf()
		:itPat{}
	{
	}

	leaf::leaf(chRevIter iter)
		:info{}
		, pPrev{ nullptr }
		, itPat{ iter }
	{
	}

	const leaf* leaf::is_tradable(const chRevIter itBegin, const chRevIter itEnd) const
	{
		if (itBegin == itEnd)
			return nullptr;

		if (*itPat != *itBegin)
			return nullptr;

		//	is threshold reached?

		if (leaves.empty())
			return this;

		auto nxt{ itBegin + 1 };

		if (auto pLeaf{ find_child(*nxt) })
			return pLeaf->is_tradable(nxt, itEnd);

		return this;
	}

	const leaf* leaf::find_child(mwave::Pattern p)const
	{
		for (auto& l : leaves)
			if (l->id() == p)
				return l.get();

		return nullptr;
	}

	mwave::Pattern leaf::id()const
	{
		return *itPat;
	}

	chRevIter leaf::get_iter() const
	{
		return itPat;
	}

	bool leaf::is_buy()const
	{
		return info.Net() >= .0 ? itPat->is_m() : itPat->is_w();
	}

	size_t leaf::depth()const
	{
		auto ret{ 0ull };
		auto p{ this };
		do ++ret;
		while (p = p->pPrev);
		return ret;
	}

	seq::chain leaf::get_chain()const
	{
		seq::chain ret;
		ret.reserve(depth());

		auto p{ this };

		do ret.push_back(p->id());
		while (p = p->pPrev);

		std::reverse(ret.begin(), ret.end());

		return ret;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	const leaf* tree::is_tradable(const chRevIter itFrom, const chRevIter itTo)
	{
		if (itFrom == itTo)
			return nullptr;

		auto ind{ (char)*itFrom };

		auto& l{ m_Root[ind] };

		if (l.get_iter() == fwd::chRevIter{})
			l = itFrom;

		return l.is_tradable(itFrom, itTo);
	}

	const leaf* tree::add(chRevIter, chRevIter, const MWINFO&)
	{
		return nullptr;
	}

}
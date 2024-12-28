#include "pch.h"
#include "fwd_leaf.h"

namespace fwd
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*leaf::leaf()
		:itPat{}
	{
	}*/

	leaf::leaf(chRevIter iter)
		:info{}
		, pPrev{ nullptr }
		, itPat{ iter }
	{
	}

	leaf::leaf(chRevIter it, const MWINFO& i, const leaf* p)
		:info{ i }
		, pPrev{ p }
		, itPat{ it }
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

	leaf* leaf::find_child(mwave::Pattern p)const
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

	const leaf* leaf::add(const chRevIter itBeg, const chRevIter itEnd, const MWINFO& i, const leaf* pParent)
	{
		if (itPat == fwd::chRevIter{})
		{
			info = i;
			pPrev = pParent;
			itPat = itBeg;
			return this;
		}

		ASSERT(id() == *itBeg);

		if (auto pLeaf{ find_child((char)*(itBeg + 1)) })
			return pLeaf->add(itBeg + 1, itEnd, i, this);

		leaves.push_back(std::make_unique<leaf>(itBeg, info, this));

		return leaves.back().get();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	const leaf* tree::is_tradable(const chRevIter itFrom, const chRevIter itTo)const
	{
		if (itFrom == itTo)
			return nullptr;

		auto ind{ (char)*itFrom };

		auto& l{ m_Root[ind] };

		if (!l)
			return nullptr;

		return l->is_tradable(itFrom, itTo);
	}

	const leaf* tree::add(const chRevIter itBeg, const chRevIter itEnd, const MWINFO& i)
	{
		auto pRoot{ get_root(*itBeg) };
		if (!pRoot)
			m_Root[(char)*itBeg] = std::make_unique<leaf>(itBeg, i, nullptr);

		return pRoot->add(itBeg, itEnd, i);
	}

	leaf* tree::get_root(mwave::Pattern p)
	{
		auto val{ (char)p };

		if (val < 0 || val>31)
			return nullptr;

		return m_Root[val].get();
	}
}
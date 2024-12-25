#include "pch.h"
#include "seq.h"

namespace seq
{
	leaf::leaf(mwave::Pattern mw)
		:leaf{ mw, nullptr }
	{
	}

	leaf::leaf(mwave::Pattern mw, const leaf* pParent)
		:pat{ mw }
		, hItem{ NULL }
		, pPrev{ pParent }
		//, isSelected{ false }
	{
	}

	leaf::~leaf()
	{
		for (auto pL : leaves)
			delete pL;
	}

	void leaf::set_indexes(std::vector<INT_PTR>&& v)
	{
		indexes = std::move(v);
	}

	leaf& leaf::operator=(leaf&& oth)
	{
		pat = oth.pat;
		hItem = oth.hItem;
		pPrev = oth.pPrev;
		for (auto pL : leaves)
			delete pL;
		leaves = std::move(oth.leaves);
		indexes = std::move(oth.indexes);
		isSelected = oth.isSelected;

		oth.leaves.clear();

		return *this;
	}

	mwave::Pattern leaf::get_pattern()const
	{
		return pat;
	}

	bool leaf::grow(const chain& mws)
	{
		indexes.clear();
		leaves.clear();

		const auto id{ get_chain() };

		if (pPrev)
			indexes.reserve(pPrev->get_indexes().size());
		else
			indexes.reserve(mws.size() / 32);

		if (pPrev)
			for (auto ind : pPrev->get_indexes())
			{
				if (ind
					&& mws[ind - 1] == pat
					//&& is_same_chain((ind - 1) + id.size() - 1)
					)
					indexes.push_back(ind - 1);
			}
		else
			for (size_t i = 0; i < mws.size(); ++i)
				if (mws[i] == id.front()/* && is_same_chain(i)*/)
					indexes.push_back(i);

		indexes.shrink_to_fit();

		if (indexes.size() < 2)
			return false;

		for (char ch = 0; ch < 32; ++ch)
		{
			const mwave::Pattern oth{ ch };
			if (pat.is_m() ^ oth.is_m())
			{
				auto pL{ new leaf{ oth, this } };
				if (pL->grow(mws))
					leaves.push_back(pL);
				else
					delete pL;
			}
		}

		return true;
	}

	std::vector<leaf*>& leaf::get_leaves()
	{
		return leaves;
	}

	std::vector<INT_PTR>& leaf::get_indexes()
	{
		return indexes;
	}

	void leaf::set_handle(HTREEITEM h)
	{
		hItem = h;
	}

	void leaf::select(bool b)
	{
		isSelected = b;
		for (auto& l : leaves)
			l->select(b);
	}

	leaf* leaf::find(HTREEITEM h)
	{
		if (hItem == h)
			return this;

		for (auto pL : leaves)
			if (auto pLeaf{ pL->find(h) })
				return pLeaf;

		return nullptr;
	}

	chain leaf::get_chain()const
	{
		if (pPrev)
		{
			auto ch{ pPrev->get_chain() };
			ch.push_back(pat);
			return ch;
		}
		else
			return { pat };
	}

	int leaf::get_level() const
	{
		if (pPrev)
			return pPrev->get_level() + 1;
		else
			return 0;
	}

	const leaf* leaf::head() const
	{
		const leaf* pRet{ this };

		while (pRet)
			if (pRet->pPrev)
				pRet = pRet->pPrev;
			else
				break;

		return pRet;
	}

	const leaf* leaf::parent() const
	{
		return pPrev;
	}

	const std::vector<leaf*>& leaf::get_leaves() const
	{
		return leaves;
	}

	const std::vector<INT_PTR>& leaf::get_indexes() const
	{
		return indexes;
	}

	size_t leaf::get_max_depth() const
	{
		size_t count{};

		for (auto pL : leaves)
			count = max(count, pL->get_max_depth());

		return count + 1;
	}

	HTREEITEM leaf::get_handle() const
	{
		return hItem;
	}

	const leaf* leaf::find(HTREEITEM h) const
	{
		if (hItem == h)
			return this;

		for (auto pL : leaves)
			if (auto pLeaf{ pL->find(h) })
				return pLeaf;

		return nullptr;
	}

	bool leaf::is_selected() const
	{
		return isSelected;
	}

	//	0	unticked
	//	1	undeter
	//	2	undeter

	leaf::cbstate leaf::get_icon_state() const
	{
		if (leaves.empty())
			return isSelected ? cbstate::selected : cbstate::unselected;

		size_t iSelected{ 0 }, iUnselected{ 0 };

		for (auto& l : leaves)
			switch (l->get_icon_state())
			{
			case cbstate::selected:++iSelected; break;
			case cbstate::unselected:++iUnselected; break;
			}

		const bool
			all_selected{ iSelected == leaves.size() },
			all_unselected{ iUnselected == leaves.size() };

		assert(!(all_selected && all_unselected));

		if (all_selected)
			return cbstate::selected;

		if (all_unselected)
			return cbstate::unselected;

		return cbstate::mix;
	}
}
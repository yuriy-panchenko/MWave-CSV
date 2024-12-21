#include "pch.h"
#include "seq.h"

namespace seq
{
	leaf::leaf(mwave::Pattern mw)
		:pat{ mw }
		, hItem{ NULL }
		, pPrev{ nullptr }
	{
	}

	leaf::leaf(mwave::Pattern mw, const leaf* pParent)
		:pat{ mw }
		, hItem{ NULL }
		, pPrev{ pParent }
	{
	}

	void leaf::set_indexes(std::vector<INT_PTR>&& v)
	{
		indexes = std::move(v);
	}

	mwave::Pattern leaf::get_pattern()const
	{
		return pat;
	}

	bool leaf::grow(const CArray<mwave::Pattern>& mws)
	{
		indexes.clear();
		leaves.clear();

		const auto id{ get_chain() };
		//const auto depth{ get_level() };
		//ASSERT(id.size() == (size_t)depth + 1);
		//if (id.size() >= max_depth)
			//return false;

		auto is_same_chain = [&](INT_PTR index)->bool
			{
				auto const from{ index + 1 - (INT_PTR)id.size() };
				if (from < 0)
					return false;

				chain ch(id.size());
				auto iter{ ch.begin() };

				for (INT_PTR i = from; i <= index; ++i)
					*iter++ = mws[i];

				std::reverse(ch.begin(), ch.end());

				return ch == id;
			};

		for (INT_PTR i = 0; i < mws.GetSize(); ++i)
			if (mws[i] == id.front() && is_same_chain(i))
				indexes.push_back(i);

		if (indexes.size() < 2)
			return false;

		for (char ch = 0; ch < 32; ++ch)
		{
			const mwave::Pattern oth{ ch };
			if (pat.is_m() ^ oth.is_m())
			{
				leaf l{ oth, this };
				if (l.grow(mws))
					leaves.push_back(std::move(l));
			}
		}

		return true;
	}

	std::vector<leaf>& leaf::get_leaves()
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

	const std::vector<INT_PTR>& leaf::get_indexes() const
	{
		return indexes;
	}

	size_t leaf::get_max_depth() const
	{
		size_t count{};

		for (auto& l : leaves)
			count = max(count, l.get_max_depth());

		return count + 1;
	}
}
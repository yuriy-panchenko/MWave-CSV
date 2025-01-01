#pragma once

namespace fwd
{
	using chRevIter = seq::chain::const_reverse_iterator;

	class leaf
	{
	public:
		leaf() = delete;
		leaf(const leaf&) = delete;
		leaf(leaf&&) = default;
		leaf(chRevIter);
		leaf(chRevIter, const MWINFO&, leaf*);
		//leaf(mwave::Pattern p);
		~leaf() = default;

		leaf& operator=(const leaf&) = delete;
		leaf& operator=(leaf&&) = default;

		const leaf* is_tradable(chRevIter, chRevIter)const;
		bool is_buy()const;
		seq::chain get_chain()const;
		size_t depth()const;
		leaf* find_child(mwave::Pattern p)const;
		mwave::Pattern id()const;
		chRevIter get_iter()const;
		const leaf& head()const;

		const leaf* add(const chRevIter itBeg, const chRevIter itEnd, const MWINFO& i);
		const MWINFO& update_info();
		leaf& head();


	private:
		//mwave::Pattern pat;
		MWINFO info;
		leaf* pPrev;

		chRevIter itPat;
		std::vector<std::unique_ptr<leaf>> leaves;
	};

	class tree
	{
		std::unique_ptr<leaf> m_Root[32];

	public:
		tree() = default;

		const leaf* is_tradable(chRevIter, chRevIter)const;
		const leaf* add(chRevIter, chRevIter, const MWINFO&);
		leaf* get_root(mwave::Pattern);
	};
}
#pragma once
#include "seq.h"

namespace trd
{
	class leaf
	{
		mwave::Pattern pat;
		const leaf* pPrev;
		bool isSelected{ false };
		std::vector<leaf*> leaves;

		MWINFO info;

	public:
		leaf();
		leaf(const leaf&) = delete;
		leaf(leaf&&) = default;
		leaf(mwave::Pattern);
		leaf(const seq::leaf&, const MWINFO&,const leaf* parent=nullptr);
		~leaf();

		leaf& operator=(const leaf&) = delete;
		leaf& operator=(leaf&&) = default;

		mwave::Pattern id()const;
		bool is_valid() const;
		bool is_buy()const;
		double diff()const;
		bool is_selected()const;
		const leaf* is_tradable(const seq::chain::const_reverse_iterator itFrom, const seq::chain::const_reverse_iterator itTo) const;
		seq::chain chain()const;
		size_t depth()const;
		const leaf* head()const;
		const leaf* add(const seq::chain::const_reverse_iterator itFrom, const seq::chain::const_reverse_iterator itTo, const MWINFO& info);

		void add(leaf*);

	protected:
		const leaf* find_child(mwave::Pattern)const;
	};

	class tree
	{
		std::unique_ptr<leaf> m_Root[32];

	public:
		tree() = default;
		tree(const tree&) = delete;
		tree(tree&&) = default;
		~tree() = default;

		tree& operator=(const tree&) = delete;
		tree& operator=(tree&&) = default;

		const leaf* is_tradable(seq::chain::const_reverse_iterator, seq::chain::const_reverse_iterator)const;
		const leaf* add(seq::chain::const_reverse_iterator, seq::chain::const_reverse_iterator, const MWINFO&);

		void set(std::unique_ptr<leaf>&&);
	};
}


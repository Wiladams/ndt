#pragma once

#include "pstoken.h"
#include <memory>
#include <deque>

// An unbounded stack
class PSStack
{
	std::deque<std::shared_ptr<PSToken> > fContainer;

public:
	size_t length() const
	{
		return fContainer.size();
	}

	size_t countToMark()
	{
		size_t pos = fContainer.size();
		while (pos > 0)
		{
			if (fContainer.at(pos-1)->fType == PSTokenType::MARK)
				return pos;
			pos--;
		}

		return 0;
	}

	PSStack& clear()
	{
		fContainer.clear();
		return *this;
	}

	PSStack& dup()
	{
		if (fContainer.size() > 0) {
			push(this->top());
		}

		return *this;
	}

	PSStack& exch()
	{
		auto a = pop();
		auto b = pop();
		push(a);
		push(b);

		return *this;
	}

	PSStack& mark()
	{
		//fContainer.push_back(std::make_shared<PSToken>(PSTokenType::MARK, false));
		return *this;
	}

	PSStack& push(std::shared_ptr<PSToken> a)
	{
		fContainer.push_back(a);
		return *this;
	}

	std::shared_ptr<PSToken> pop()
	{
		auto a = fContainer.back();
		fContainer.pop_back();

		return a;

	}

	// n is the number of items to consider
	// j is the number of positions to exchange
	// this is a brute force implementation which simply
	// does a single rotation as many times as is needed
	// a more direct approach would be to calculate the
	// new position of each elementand use swaps to put
	// them in place
	bool roll(int n, int j)
	{	
	}

	std::shared_ptr<PSToken> top()
	{
		return fContainer.back();
	}

	// get an item 'n' positions from the top
	std::shared_ptr<PSToken> nth(const size_t n)
	{
		auto last = fContainer.size()-1;
		auto idx = last - n;
		if (idx < 0)
			return nullptr;

		return fContainer.at(idx);
	}
	
};
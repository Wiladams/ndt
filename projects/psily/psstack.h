#pragma once

#include "pstypes.h"

#include <memory>
#include <deque>

// An unbounded stack
//teamplate <typename T>
class PSStack
{


public:
	// BUGBUG - this should not be public
	// but, it is for now
	std::deque<std::shared_ptr<PSToken> > fContainer;

	// Return the number of items that are currently 
	// on the stack
	size_t length() const
	{
		return fContainer.size();
	}

	// Clear all entries from the stack
	PSStack& clear()
	{
		fContainer.clear();

		return *this;
	}

	// Count the number of items from the top of the
	// stack down to a mark, or end of the stack
	size_t countToMark()
	{
		size_t pos = fContainer.size();
		while (pos > 0)
		{
			if (fContainer.at(pos - 1)->fType == PSTokenType::MARK)
				return pos;
			pos--;
		}

		return 0;
	}

	PSStack& clearToMark()
	{
		while (length() > 0) {
			auto item = pop();
			if (item->fType == PSTokenType::MARK)
				break;
		}

		return *this;
	}

	// Duplicate the item at the top of the stack
	PSStack& dup()
	{
		if (fContainer.size() > 0) {
			push(this->top());
		}

		return *this;
	}

	PSStack& copy(size_t n)
	{
		auto tok = top();
		for (size_t i = 0; i < n; i++)
		{
			// BUGBUG
			// need to consider deep copy depending on the thing?
			push(tok);
		}

		return *this;
	}

	// Exchange the two items that are atop
	// the stack
	PSStack& exch()
	{
		auto a = pop();
		auto b = pop();
		push(a);
		push(b);

		return *this;
	}

	// Place a marker on the stack
	PSStack& mark()
	{
		fContainer.push_back(std::make_shared<PSToken>(PSTokenType::MARK));
		return *this;
	}

	// Push a token onto the stack
	PSStack& push(std::shared_ptr<PSToken> a)
	{
		fContainer.push_back(a);
		return *this;
	}

	// Pop a token off the stack.
	// if the stack is empty, then return
	// a nullptr
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
	// new position of each element and use swaps to put
	// them in place
	PSStack & roll(int n, int j)
	{	
		if (j > 0){
			// Roll the stack 'up' counter clockwise
			for (size_t outer = 1; outer <= j; outer++)
			{
				auto tmp = top();

				for (size_t inner = 1; inner < n; inner++) {
					
				}
			}
		}
		else if (j < 0) {

		}
		return *this;
	}

	// Peek the top of the stack
	// do NOT take the item off the stack
	std::shared_ptr<PSToken> top()
	{
		if (fContainer.size() < 1)
			return nullptr;

		return fContainer.back();
	}

	// get an item 'n' positions from the top
	// The item at the 'top' of the stack is item '0'
	// the item at the bottom of the stack is item 'n'
	std::shared_ptr<PSToken> nth(const size_t n)
	{
		auto last = fContainer.size()-1;
		auto idx = last - n;
		if (idx < 0)
			return nullptr;

		return fContainer.at(idx);
	}
	
};
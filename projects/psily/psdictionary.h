#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "pstypes.h"
#include "psstack.h"

using std::make_shared;
using std::shared_ptr;

class PSVM;



class PSDictionaryStack : public PSStack
{
public:
	void pushDictionary(shared_ptr<PSDictionary> d)
	{
		auto tok = make_shared<PSToken>(d);
		push(tok);
	}


	// BUGBUG - need some error checking here
	shared_ptr<PSDictionary> popDictionary()
	{
		auto tok = pop();
		if (nullptr == tok)
			return nullptr;

		if (tok->fType != PSTokenType::DICTIONARY)
			return nullptr;

		return std::get<shared_ptr<PSDictionary> >(tok->fData);
	}

	shared_ptr<PSDictionary> currentdict()
	{
		auto topItem = top();
		if (nullptr == topItem)
			return nullptr;

		if (top()->fType == PSTokenType::MARK) {
			auto tok = nth(1);
			if (nullptr == tok)
				return nullptr;

			return std::get<shared_ptr<PSDictionary> >(tok->fData);
		}

		return std::get<shared_ptr<PSDictionary> >(top()->fData);
	}

	// Associate key and value in current dictionary
	bool def(std::string key, shared_ptr<PSToken> value)
	{
		auto current = currentdict();
		
		if (nullptr == current)
			return false;

		current->insert_or_assign(key, value);
	}

	shared_ptr<PSDictionary> where(std::string key)
	{
		for (size_t idx = 0; idx < length(); idx++)
		{
			auto tok = nth(idx);
			if (tok->fType == PSTokenType::DICTIONARY) {
				if (std::get<shared_ptr<PSDictionary> >(tok->fData)->operator[](key) != nullptr)
					return std::get<shared_ptr<PSDictionary> >(tok->fData);
			}
		}

		return nullptr;
	}

	// Search for a key, traversing dictionaries 
	// in stack order
	shared_ptr<PSToken> load(std::string key)
	{
		auto d = where(key);

		if (nullptr == d)
			return nullptr;

		// We found a dictionary, so retrieve the
		// value.  Kind of duplicative of 'where'

		return d->operator[](key);
	}

	void store(std::string key, shared_ptr<PSToken> value)
	{
		auto d = where(key);

		// If we didn't find an entry in an 
		// existing dictionary, then stick the value
		// in the current dictionary
		if (nullptr == d) {
			d = currentdict();
		}

		d->operator[](key) = value;
	}
};

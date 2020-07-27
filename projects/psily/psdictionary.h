#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "psstack.h"
#include "pstoken.h"

using std::make_shared;
using std::shared_ptr;

class PSVM;

class PSDictionary : public std::unordered_map<std::string, std::shared_ptr<PSToken> >
{

public:
	PSDictionary(std::unordered_map < std::string, std::function<void(PSVM& vm)> > ops)
	{
		for (auto& it : ops)
		{
			insert({ it.first, make_shared<PSToken>(it.second) });
		}
	}
};

class PSDictionaryStack : public PSStack
{
public:
	void pushDictionary(shared_ptr<PSDictionary> d)
	{
		auto tok = make_shared<PSToken>(d);
		push(tok);
	}

	//void pushDictionary(std::shared_ptr<PSDictionary> d)
	//{
	//	auto tok = make_shared<PSToken>(d);
	//	push(tok);
	//}

	// BUGBUG - need some error checking here
	shared_ptr<PSDictionary> popDictionary()
	{
		pop();
	}

	shared_ptr<PSDictionary> currentdict()
	{
		auto topItem = top();
		if (nullptr == topItem)
			return nullptr;

		if (top()->fType == PSTokenType::MARK) {
			auto tok = nth(1);
			return tok->fData.asDictionary;
		}

		return top()->fData.asDictionary;
	}

	// Associate key and value in current dictionary
	void def(std::string key, shared_ptr<PSToken> value)
	{
		auto current = currentdict();
		current->insert_or_assign(key, value);
	}

	shared_ptr<PSDictionary> where(std::string key)
	{
		for (size_t idx = 0; idx < length(); idx++)
		{
			auto tok = nth(idx);
			if (tok->fType == PSTokenType::DICTIONARY) {
				if (tok->fData.asDictionary->operator[](key) != nullptr)
				//if (tok->fData.asDictionary->find(key) != nullptr)
					return tok->fData.asDictionary;
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

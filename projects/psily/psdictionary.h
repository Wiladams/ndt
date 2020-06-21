#pragma once

#include <unordered_map>

#include "psstack.h"

class PSDictionary : std::unordered_map<std::string, std::shared_ptr<PSToken>)
{

public:

};

class PSDictionaryStack : public PSStack
{
	void pushDictionary(std::shared_ptr<PSDictionary> d)
	{

	}

	
};

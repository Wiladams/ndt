#pragma once

#include <vector>
#include <memory>

#include "pstoken.h"


class PSArray : public std::vector<std::shared_ptr<PSToken> > 
{
public:
	PSArray() {}
};
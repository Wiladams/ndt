#pragma once

#include <vector>
#include <memory>

#include "pstypes.h"

class PSArray : public std::vector<std::shared_ptr<PSToken> > 
{
public:
	PSArray() {}
};
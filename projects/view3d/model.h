#pragma once


#include "GMesh.h"

class Model : public GMesh {
private:

public:
	static GMesh * loadModel(const char *filename);

};



#pragma once

//==============================================================================================
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

// Usings
using std::shared_ptr;
using std::make_shared;


// Constants
const double INFD = std::numeric_limits<double>::infinity();


// Common Headers
#include "grmath.h"
#include "ray.h"

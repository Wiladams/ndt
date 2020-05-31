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

#include "maths.hpp"

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double INFD = std::numeric_limits<double>::infinity();


// Utility Functions
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double_range(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int(int low, int high) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double_range(low, high + 1));
}

// Common Headers

#include "vec3.h"
#include "ray.h"




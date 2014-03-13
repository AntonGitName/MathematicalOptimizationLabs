#ifndef TRANSPORTATION_H
#define TRANSPORTATION_H

#include "simplex.h"

namespace transportation
{
    vvd solve(const vvd& a, const vd& b, const vd& c);
}

#endif // TRANSPORTATION_H
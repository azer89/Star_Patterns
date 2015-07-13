#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#include "ALine.h"
#include <vector>

class Triangulator
{
public:
    Triangulator();
    ~Triangulator();

static std::vector<ALine> GetTriangles(std::vector<std::pair<ALine, ALine>> shape, AVector seedPt);

};

#endif // TRIANGULATOR_H

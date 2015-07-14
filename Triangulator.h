#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#include "ALine.h"
#include <vector>

class Triangulator
{
public:
    Triangulator();
    ~Triangulator();


static std::vector<ALine> GetTriangles1(std::vector<std::pair<ALine, ALine>> shape, AVector seedPt);
static std::vector<ALine> GetTriangles2(std::vector<ALine> shape, AVector seedPt);
static std::vector<ALine> GetTriangles3(std::vector<std::pair<ALine, ALine>> shape, std::vector<ALine> poly);
static std::vector<ALine> GetTriangles4(std::vector<std::pair<ALine, ALine>> shape, std::vector<ALine> poly);

private:
// to do: brute force
static ALine SearchLine(std::vector<std::pair<ALine, ALine>> shape, int side, bool isRight);
static AVector GetIntersection(ALine line1, ALine line2);

};

#endif // TRIANGULATOR_H

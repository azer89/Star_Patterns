#ifndef TILING_H
#define TILING_H

#include <vector>

#include <AVector.h>

enum ShapeType
{
    S_REGULAR = 0,
    S_POLYGON = 1,


};

struct Tile
{
public:
    ShapeType _shapeType;

};

struct Tiling
{
public:
    std::string _name;
    std::vector<AVector> _translations;
    std::vector<Tile> _tiles;

};

#endif // TILING_H

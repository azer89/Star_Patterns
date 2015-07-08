#ifndef TILING_H
#define TILING_H

//#include <QMatrix3x3>
//#include <vector>
//#include <AVector.h>

enum ShapeType
{
    S_REGULAR = 0,
    S_POLYGON = 1,
};

struct TileData
{
public:
    ShapeType               _shapeType;
    float                   _sides;
    std::vector<QMatrix3x3> _transforms;
    std::vector<AVector>    _vertices;


    float GetRotation()
    {
        if(this->_shapeType == ShapeType::S_POLYGON)
        {
            return 0;
        }
        else if(this->_sides == 3)
        {
            return M_PI / (this->_sides * 2.0f);
        }
        else if(this->_sides == 4)
        {
            return M_PI / this->_sides;
        }
        else if(this->_sides == 5)
        {
            // to do: why?
            return M_PI / (-2.0f) ;
        }
        else if(this->_sides == 6)
        {
            return 0;
        }
        else if(this->_sides == 8)
        {
            return M_PI / this->_sides;
        }
        else if(this->_sides == 9)
        {
            return M_PI / 6.0f;
        }
        else if(this->_sides == 12)
        {
            return M_PI / this->_sides;
        }
        else if(this->_sides == 16)
        {
            return M_PI / this->_sides;
        }


        return 0;
    }
};

struct TilingData
{
public:
    std::string _name;
    AVector _translation1;
    AVector _translation2;
    std::vector<TileData> _tiles;

};

#endif // TILING_H

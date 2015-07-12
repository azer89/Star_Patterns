#ifndef SYSTEMPARAMS_H
#define SYSTEMPARAMS_H

#include <iostream>
#include <QVector3D>

class SystemParams
{
public:
    SystemParams();
    ~SystemParams();

public:
    static std::string default_tiling;

    static bool show_tiling;
    static float rad_angle;

    static float ribbon_width;

    static int w;
    static int h;

    static QVector3D ribbon_color;
    static QVector3D line_color;
};

#endif // SYSTEMPARAMS_H

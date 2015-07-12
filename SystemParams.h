#ifndef SYSTEMPARAMS_H
#define SYSTEMPARAMS_H

#include <iostream>

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
};

#endif // SYSTEMPARAMS_H

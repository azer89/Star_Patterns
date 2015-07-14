
#include "SystemParams.h"

#define _USE_MATH_DEFINES
#include <cmath>

std::string SystemParams::default_tiling = "3.4.12 RD";

bool SystemParams::show_tiling = false;

float SystemParams::rad_angle = M_PI / 4.0f;

float SystemParams::ribbon_width = 0.02;

float SystemParams::line_width = 0.006;

float SystemParams::contact_delta = 0.0;

int SystemParams::w = 1;
int SystemParams::h = 1;

QVector3D SystemParams::star_color = QVector3D(0, 0.75, 0.75);
QVector3D SystemParams::ribbon_color = QVector3D(0, 0, 1);
QVector3D SystemParams::background_color = QVector3D(1, 1, 1);
QVector3D SystemParams::interlacing_color = QVector3D(1, 1, 0);




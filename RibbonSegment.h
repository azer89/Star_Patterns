#ifndef RIBBONSEGMENT_H
#define RIBBONSEGMENT_H

#include "AVector.h"
#include "ALine.h"

#include <vector>

struct RibbonSegment
{
public:
    //AVector _startMPt;
    //AVector _endMPt;

    //AVector _anchor1;
    //AVector _anchor2;

    AVector _startRPt;
    AVector _endRPt;

    AVector _startLPt;
    AVector _endLPt;

    //LayerType _layerType;
    //std::vector<ALine> _cLines;

    //std::vector<ALine> _rLines;
    //std::vector<ALine> _lLines;

    ALine _rLine;
    ALine _lLine;
};

#endif // RIBBONSEGMENT_H

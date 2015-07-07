#ifndef PATTERNGENERATOR_H
#define PATTERNGENERATOR_H

#include "ALine.h"
#include "AVector.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix3x3>

class PatternGenerator
{
public:
    PatternGenerator();
    ~PatternGenerator();

    void InitTiling();
    void Paint();

private:
    std::vector<AVector>    GenerateNGon(float sides, float radius, float angleOffset, AVector centerPt);
    void                    ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines);
    AVector                 MultiplyVector(QMatrix3x3 mat, AVector vec);
    void                    MultiplyShape(QMatrix3x3 mat, std::vector<AVector>& shape);

    void PrepareLinesVAO(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol);

// to do: fix this
public:
    QOpenGLShaderProgram* _shaderProgram;
    int         _colorLocation;
    int         _vertexLocation;
    int         _use_color_location;
    int         _img_width;
    int         _img_height;


private:
    std::vector<ALine>          _tilingLines;
    QOpenGLBuffer               _tilingLinesVbo;
    QOpenGLVertexArrayObject    _tilingLinesVao;



};

#endif // PATTERNGENERATOR_H

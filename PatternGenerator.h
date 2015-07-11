#ifndef PATTERNGENERATOR_H
#define PATTERNGENERATOR_H



#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix3x3>

#include "ALine.h"
#include "AVector.h"
#include "TilingData.h"

class PatternGenerator
{
public:
    PatternGenerator();
    ~PatternGenerator();

    //void InitTiling1();
    void InitTiling();
    void GeneratePattern(std::string tilingName);
    void Paint();

private:
    std::vector<AVector>    GenerateNGon(float sides, float radius, float angleOffset, AVector centerPt);
    void                    ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines);
    void                    ConcatShapes(std::vector<AVector> sourcePolygon, std::vector<std::vector<ALine>>  &shapes);
    void                    InferenceAlgorithm(std::vector<std::vector<ALine>> shapes);
    AVector                 MultiplyVector(QMatrix3x3 mat, AVector vec);
    void                    MultiplyShape(QMatrix3x3 mat, std::vector<AVector>& shape);
    void                    ReadXML(std::string filename);
    TilingData              GetTiling(std::string tilingName);
    //bool                    IsCollinear(AVector pt1, AVector pt2, AVector pt3);
    bool CheckCollinearCase(ALine ray1, ALine ray2);
    bool CheckHorizontalVerticalCase(ALine ray1, ALine ray2);

    void PrepareLinesVAO1(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol);
    void PrepareLinesVAO2(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao);

// to do: fix this
public:
    QOpenGLShaderProgram* _shaderProgram;
    int         _colorLocation;
    int         _vertexLocation;
    int         _use_color_location;
    int         _img_width;
    int         _img_height;

    // to do: delete me
    //float sideDiv;


private:
    std::vector<ALine>              _rayLines;
    QOpenGLBuffer                   _rayLinesVbo;
    QOpenGLVertexArrayObject        _rayLinesVao;

    std::vector<ALine>              _tilingLines;
    QOpenGLBuffer                   _tilingLinesVbo;
    QOpenGLVertexArrayObject        _tilingLinesVao;

    std::vector<TilingData>         _tilings;

    std::vector<std::vector<ALine>> _shapes;
};

#endif // PATTERNGENERATOR_H

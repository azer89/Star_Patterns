#ifndef PATTERNGENERATOR_H
#define PATTERNGENERATOR_H



#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QMatrix3x3>

#include "ALine.h"
#include "AVector.h"
#include "TilingData.h"
#include "RibbonSegment.h"

class PatternGenerator
{
public:
    PatternGenerator();
    ~PatternGenerator();

    //void InitTiling1();
    void InitTiling();
    void GeneratePattern(std::string tilingName);
    void Paint(float zoomFactor);

    std::vector<ALine> GetTilingLines() { return _tilingLines; }
    std::vector<ALine> GetTriangleLines() { return _triangleLines; }
    std::vector<ALine> GetULines() { return _uLines; }
    std::vector<ALine> GetOLines() { return _oLines; }

private:
    std::vector<AVector>    GenerateNGon(float sides, float radius, float angleOffset, AVector centerPt);
    void                    ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines);
    void                    ConcatShapes(std::vector<AVector> sourcePolygon, std::vector<std::vector<ALine>>  &shapes);
    void                    InferenceAlgorithm(std::vector<std::vector<ALine>> shapes);
    AVector                 MultiplyVector(QMatrix3x3 mat, AVector vec);
    void                    MultiplyShape(QMatrix3x3 mat, std::vector<AVector>& shape);
    float                   AngleInBetween(AVector vec1, AVector vec2);
    void                    ReadXML(std::string filename);
    TilingData              GetTiling(std::string tilingName);

    float GetRotation(AVector pt1, AVector pt2);

    AVector GetPolygonCentroid(std::vector<ALine> shapes);

    //bool                    IsCollinear(AVector pt1, AVector pt2, AVector pt3);
    bool CheckCollinearCase(ALine ray1, ALine ray2);
    bool CheckHorizontalVerticalCase(ALine ray1, ALine ray2);
    void CalculateInterlace(std::pair<ALine, ALine> segment, std::vector<ALine> aShape, std::vector<ALine> &uLines, std::vector<ALine> &oLines);

    void PreparePointsVAO(std::vector<AVector> points, QOpenGLBuffer* ptsVbo, QOpenGLVertexArrayObject* ptsVao, QVector3D vecCol);
    void PrepareLinesVAO0(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol1, QVector3D vecCol2);
    void PrepareLinesVAO1(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol);
    void PrepareLinesVAO2(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao);
    void PrepareQuadsVAO(std::vector<ALine> lines, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, QVector3D vecCol);
    void PrepareTrianglesVAO(std::vector<ALine> lines, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, QVector3D vecCol);

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

    // to do: delete this
    std::vector<ALine>              _tempLines;
    QOpenGLBuffer                   _tempLinesVbo;
    QOpenGLVertexArrayObject        _tempLinesVao;

    // to do: delete this
    std::vector<AVector>            _tempPoints;
    QOpenGLBuffer                   _tempPointsVbo;
    QOpenGLVertexArrayObject        _tempPointsVao;

    // triangles
    std::vector<ALine>              _triangleLines;
    QOpenGLBuffer               _trianglesVbo;
    QOpenGLVertexArrayObject    _trianglesVao;

    // triangles
    std::vector<ALine>          _backTriangleLines;
    QOpenGLBuffer               _backTrianglesVbo;
    QOpenGLVertexArrayObject    _backTrianglesVao;

    // under
    //std::vector<RibbonSegment>  _uSegments;
    std::vector<ALine>  _uLines;

    // over
    //std::vector<RibbonSegment>  _oSegments;
    std::vector<ALine>  _oLines;

    // lines under
    QOpenGLBuffer               _uLinesVbo;
    QOpenGLVertexArrayObject    _uLinesVao;

    // lines over
    QOpenGLBuffer               _oLinesVbo;
    QOpenGLVertexArrayObject    _oLinesVao;

    // quads under
    QOpenGLBuffer               _uQuadsVbo;
    QOpenGLVertexArrayObject    _uQuadsVao;

    // quads over
    QOpenGLBuffer               _oQuadsVbo;
    QOpenGLVertexArrayObject    _oQuadsVao;

    std::vector<ALine>              _tilingLines;
    QOpenGLBuffer                   _tilingLinesVbo;
    QOpenGLVertexArrayObject        _tilingLinesVao;

    std::vector<TilingData>         _tilings;

    std::vector<std::vector<ALine>> _shapes;
};

#endif // PATTERNGENERATOR_H

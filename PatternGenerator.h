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


    /*
     * Read xml files which contain tiling specifications
     */
    void InitTiling();

    /*
     * 1. generate an islamic ornamental tiling given a name of a tiling
     * 2. Call InferenceAlgorithm()
     * 3. Create vertex data
     */
    void GeneratePattern(std::string tilingName);

    /*
     * Pain function, nothing special
     */
    void Paint(float zoomFactor);


    /*
     * These functions are called by GLWidget to render an SVG file
     */
    std::vector<ALine> GetTilingLines() { return _tilingLines; }
    std::vector<ALine> GetTriangleLines() { return _triangleLines; }
    std::vector<ALine> GetBackTriangleLines() { return _backTriangleLines; }
    std::vector<ALine> GetAddTriangleLines() { return _addTriangleLines; }
    std::vector<ALine> GetULines() { return _uLines; }
    std::vector<ALine> GetOLines() { return _oLines; }

private:
    /*
     * Generate an N-Gon, this function has a numerical problem so I use epsilon
     */
    std::vector<AVector>    GenerateNGon(float sides, float radius, float angleOffset, AVector centerPt);

    /*
     * Copy sourcePolygon to destinationLines
     */
    void                    ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines);

    /*
     * Copy sourcePolygon to shapes
     */
    void                    ConcatShapes(std::vector<AVector> sourcePolygon, std::vector<std::vector<ALine>>  &shapes);

    /*
     * The greedy inference algorithm
     */
    void                    InferenceAlgorithm(std::vector<std::vector<ALine>> shapes);

    /*
     * Affine transformation
     */
    AVector                 MultiplyVector(QMatrix3x3 mat, AVector vec);

    /*
     * Affine transformation
     */
    void                    MultiplyShape(QMatrix3x3 mat, std::vector<AVector>& shape);

    /*
     * Unsigned angle in radian
     */
    float                   AngleInBetween(AVector vec1, AVector vec2);

    /*
     * Read an XML file with TinyXML
     */
    void                    ReadXML(std::string filename);

    /*
     *
     */
    TilingData              GetTiling(std::string tilingName);

    /*
     * Signed angle in radian
     */
    float GetRotation(AVector pt1, AVector pt2);

    /*
     * Get a center of mass of a polygon
     */
    AVector GetPolygonCentroid(std::vector<ALine> shapes);

    /*
     * Do both rays are collinear ?
     */
    bool CheckCollinearCase(ALine ray1, ALine ray2);

    /*
     * Do both rays create a 90 degree corner
     */
    bool CheckHorizontalVerticalCase(ALine ray1, ALine ray2);

    /*
     *
     */
    void CalculateInterlace(std::pair<ALine, ALine> segment, std::vector<ALine> aShape, std::vector<ALine> &uLines, std::vector<ALine> &oLines);

    void BuildPointsVertexData(std::vector<AVector> points, QOpenGLBuffer* ptsVbo, QOpenGLVertexArrayObject* ptsVao, QVector3D vecCol);
    void BuildLinesVertexData0(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol1, QVector3D vecCol2);
    void BuildLinesVertexData1(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol);
    void BuildLinesVertexData2(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao);
    void BuildQuadsVertexData(std::vector<ALine> lines, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, QVector3D vecCol);
    void BuildTrianglesVertexData(std::vector<ALine> lines, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, QVector3D vecCol);

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

    // triangles
    std::vector<ALine>          _addTriangleLines;
    QOpenGLBuffer               _addTrianglesVbo;
    QOpenGLVertexArrayObject    _addTrianglesVao;

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

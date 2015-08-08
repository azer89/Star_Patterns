#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <iostream>

#include <QMatrix4x4>
#include <QtOpenGL/QGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "AVector.h"
#include "ALine.h"
#include "PatternGenerator.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

private:
    bool    _isMouseDown;
    float   _zoomFactor;
    QPoint  _scrollOffset;

    // image size
    int _img_width;
    int _img_height;

    // shader
    QOpenGLShaderProgram* _shaderProgram;

    PatternGenerator* _patternGenerator;

    // points
    std::vector<AVector>        _points;
    QOpenGLBuffer               _pointsVbo;
    QOpenGLVertexArrayObject    _pointsVao;

    // lines
    //std::vector<ALine>          _tilingLines;
    //QOpenGLBuffer               _tilingLinesVbo;
    //QOpenGLVertexArrayObject    _tilingLinesVao;

    // for rendering
    int         _mvpMatrixLocation;
    int         _colorLocation;
    int         _vertexLocation;
    int         _use_color_location;
    QMatrix4x4  _perspMatrix;
    QMatrix4x4  _transformMatrix;

private:
    //void InitCurve();
    //void PaintCurve();
    //void CreateCurveVAO();



    //void InitTiling();
    //std::vector<AVector> GenerateNGon(float sides, float radius, float angleOffset, AVector centerPt);
    //void ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines);
    //AVector MultiplyVector(QMatrix3x3 mat, AVector vec);
    //void MultiplyShape(QMatrix3x3 mat, std::vector<AVector>& shape);

    //void PreparePointsVAO(std::vector<AVector> points, QOpenGLBuffer* ptsVbo, QOpenGLVertexArrayObject* ptsVao, QVector3D vecCol);
    //void PrepareLinesVAO(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol);

protected:
    // qt event
    bool event( QEvent * event );
    // init opengl
    void initializeGL();
    // draw
    void paintGL();

    void resizeGL(int width, int height);

public:

    // constructor
    GLWidget( QGLFormat format, QWidget *parent = 0);
    // destructor
    ~GLWidget();

    QSize GetCanvasSize() { return QSize(_img_width, _img_height); }

    void SaveToSvg();

    // zoom in handle
    void ZoomIn();
    // zoom out handle
    void ZoomOut();
    // set zoom value
    void SetZoom(int val){this->_zoomFactor = val;}
    // get zoom value
    float GetZoomFactor() { return this->_zoomFactor; }

    // set horizontal scroll position
    void HorizontalScroll(int val);
    // set vertical scroll position
    void VerticalScroll(int val);
    // get scroll position (horizontal and vertical)
    QPoint GetScrollOffset() {return this->_scrollOffset;}

    // mouse press
    void mousePressEvent(int x, int y);
    // mouse move
    void mouseMoveEvent(int x, int y);
    // mouse release
    void mouseReleaseEvent(int x, int y);
    // mouse double click
    void mouseDoubleClick(int x, int y);

    void GeneratePattern(std::string tilingName);

    void ResizeLines(std::vector<ALine> &lines, AVector offsetVec, float scaleFactor);

    /*void IncreaseDiv()
    {
        std::cout << "increase\n";
        _patternGenerator->sideDiv += 1;
        _patternGenerator->InitTiling2();
    }*/

    /*void DecreaseDiv()
    {
        std::cout << "decrease\n";
        _patternGenerator->sideDiv -= 1;
        _patternGenerator->InitTiling2();
    }*/
};

#endif // GLWIDGET_H

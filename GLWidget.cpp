


#include "GLWidget.h"

#include <iostream>
#include <random>
#include <math.h>

#include <QGLFormat>
#include <QSvgGenerator>

#include "VertexData.h"

GLWidget::GLWidget(QGLFormat format, QWidget *parent) :
    QGLWidget(format, parent),
    _isMouseDown(false),
    _zoomFactor(10.0),
    _shaderProgram(0),
    _img_width(50),
    _img_height(50)
{
}

GLWidget::~GLWidget()
{
    if(_shaderProgram) delete _shaderProgram;
}

void GLWidget::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) { std::cerr << "Could not enable sample buffers." << std::endl; return; }

    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glEnable(GL_DEPTH_TEST);

    _shaderProgram = new QOpenGLShaderProgram();
    if (!_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "../IslamicStarPatterns/shader.vert"))
        { std::cerr << "Cannot load vertex shader." << std::endl; return; }

    if (!_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "../IslamicStarPatterns/shader.frag"))
        { std::cerr << "Cannot load fragment shader." << std::endl; return; }

    if ( !_shaderProgram->link() )
        { std::cerr << "Cannot link shaders." << std::endl; return; }

    _shaderProgram->bind();
    _mvpMatrixLocation = _shaderProgram->uniformLocation("mvpMatrix");
    _colorLocation = _shaderProgram->attributeLocation("vertexColor");
    _vertexLocation = _shaderProgram->attributeLocation("vert");
    _use_color_location = _shaderProgram->uniformLocation("use_color");

    //InitCurve();
    //CreateCurveVAO();
    InitTiling();
}

bool GLWidget::event( QEvent * event )
{
    return QGLWidget::event(event);
}


// This is an override function from Qt but I can't find its purpose
void GLWidget::resizeGL(int width, int height)
{
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, this->width(),  this->height());

    int current_width = width();
    int current_height = height();

    // Set orthographic Matrix
    QMatrix4x4 orthoMatrix;

    orthoMatrix.ortho(0.0 +  _scrollOffset.x(),
                      (float)current_width +  _scrollOffset.x(),
                      (float)current_height + _scrollOffset.y(),
                      0.0 + _scrollOffset.y(),
                      -100, 100);

    // Translate the view to the middle
    QMatrix4x4 transformMatrix;
    transformMatrix.setToIdentity();
    transformMatrix.scale(_zoomFactor);

    _shaderProgram->setUniformValue(_mvpMatrixLocation, orthoMatrix * transformMatrix);

    PaintCurve();
}

// Mouse is pressed
void GLWidget::mousePressEvent(int x, int y)
{
    _isMouseDown = true;

    double dx = x + _scrollOffset.x();
    dx /= _zoomFactor;

    double dy = y + _scrollOffset.y();
    dy /= _zoomFactor;

    this->repaint();
}

// Mouse is moved
void GLWidget::mouseMoveEvent(int x, int y)
{
    double dx = x + _scrollOffset.x();
    dx /= _zoomFactor;

    double dy = y + _scrollOffset.y();
    dy /= _zoomFactor;

    // your stuff

    this->repaint();
}


// Mouse is released
void GLWidget::mouseReleaseEvent(int x, int y)
{
    _isMouseDown = false;
    double dx = x + _scrollOffset.x();
    dx /= _zoomFactor;

    double dy = y + _scrollOffset.y();
    dy /= _zoomFactor;

    // your stuff

    this->repaint();
}

void GLWidget::mouseDoubleClick(int x, int y)
{
    double dx = x + _scrollOffset.x();
    dx /= _zoomFactor;

    double dy = y + _scrollOffset.y();
    dy /= _zoomFactor;

    // your stuff

    this->repaint();
}


void GLWidget::HorizontalScroll(int val) { _scrollOffset.setX(val); }
void GLWidget::VerticalScroll(int val) { _scrollOffset.setY(val); }
void GLWidget::ZoomIn() { this->_zoomFactor += 0.5f; }
void GLWidget::ZoomOut() { this->_zoomFactor -= 0.5f; if(this->_zoomFactor < 0.1f) _zoomFactor = 0.1f; }

void GLWidget::ConcatNGon(std::vector<AVector> sourcePolygon, std::vector<ALine> &destinationLines)
{
    for(uint a = 0; a < sourcePolygon.size(); a++)
    {
        if(a < sourcePolygon.size() - 1)
        {
            destinationLines.push_back(ALine(sourcePolygon[a], sourcePolygon[a + 1]));
        }
        else
        {
            destinationLines.push_back(ALine(sourcePolygon[a], sourcePolygon[0]));
        }
    }
}

AVector GLWidget::Multiply(QMatrix3x3 mat, AVector vec)
{
    float x = mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * 1.0;
    float y = mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * 1.0;
    //float z = mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * 1.0;
    return AVector(x, y);
}

void GLWidget::InitTiling()
{
    /*
    const float matVals [9] = {0, 11, 22, 33, 44, 55, 66, 77, 88};
    QMatrix3x3 mat(matVals);
    std::cout << mat(0, 0) << "\n"; std::cout << mat(0, 1) << "\n"; std::cout << mat(0, 2) << "\n";
    std::cout << mat(1, 0) << "\n"; std::cout << mat(1, 1) << "\n"; std::cout << mat(1, 2) << "\n";
    std::cout << mat(2, 0) << "\n"; std::cout << mat(2, 1) << "\n"; std::cout << mat(2, 2) << "\n";
    */

    // 6.6.6
    /*
    AVector trans1(2.0000000000000004, -7.740128356567047E-16);
    AVector trans2(1.0, 1.7320508075688767);
    AVector centerPt(this->_img_width / 2, this->_img_height / 2);
    for(int a = 0; a < 10; a++)
    {
        for(int b = 0; b < 10; b++)
        {
            std::vector<AVector> shape = GenerateNGon(6, centerPt + trans1 * a + trans2 * b);
            ConcatNGon(shape, _lines);
        }
    }*/

    // 4.8.8
    AVector trans1(2.8284271247461925, -5.455873044871822E-16);
    AVector trans2(1.414213562373095, -1.4142135623730971);
    AVector centerPt(this->_img_width / 2, this->_img_height / 2);
    //<transform
    //a="0.41421356237309553" b="1.9048415650791867E-17" c="1.4142135623730954"
    //d="-1.9048415650791867E-17" e="0.41421356237309553" f="-3.885780586188048E-16"/>
    const float matVals [9] = {0.41421356237309553,
                               1.9048415650791867E-17,
                               1.4142135623730954,
                               -1.9048415650791867E-17,
                               0.41421356237309553,
                               -3.885780586188048E-16,
                               0, 0, 0};
    QMatrix3x3 mat(matVals);
    for(int a = 0; a < 10; a++)
    {
        // rectangle
        for(int b = 0; b < 10; b++)
        {
            std::vector<AVector> shape = GenerateNGon(4, centerPt + trans1 * a + trans2 * b);
            ConcatNGon(shape, _lines);
        }

        // octagon
    }


    PrepareLinesVAO(_lines, &_linesVbo, &_linesVao, QVector3D(1.0, 0.0, 0.0));
}

std::vector<AVector> GLWidget::GenerateNGon(int sides, AVector centerPt)
{
    float radius = 1.0;
    //if(sides == 6)
    //{
        float radAngle = (M_PI / (float)sides);
        radius = 1.0 / cos(radAngle);
    //}

    std::vector<AVector> shape;
    float addValue = (M_PI * 2.0 / (float)sides);
    for(float a = 0.0; a < M_PI * 2.0; a += addValue)
    {
        float xPt = centerPt.x + radius * sin(a);
        float yPt = centerPt.y + radius * cos(a);
        shape.push_back(AVector(xPt, yPt));
    }

    return shape;
}

void GLWidget::InitCurve()
{
    _points.clear();

    AVector centerPt(this->_img_width / 2, this->_img_height / 2);

    float addValue = (M_PI * 2.0 / (float)6);
    for(float a = 0.0; a < M_PI * 2.0; a += addValue)
    {
        float xPt = centerPt.x + 10 * sin(a);
        float yPt = centerPt.y + 10 * cos(a);
        _points.push_back(AVector(xPt, yPt));
    }
}

void GLWidget::CreateCurveVAO()
{
    // POINTS VAO
    QVector3D vecCol = QVector3D(1.0, 0.0, 0.0);
    PreparePointsVAO(_points, &_pointsVbo, &_pointsVao, vecCol);

    // LINES VAO
    vecCol = QVector3D(0.0, 0.5, 1.0);
    std::vector<ALine> lines;
    for(uint a = 0; a < _points.size(); a++)
    {
        if(a < _points.size() - 1) { lines.push_back(ALine(_points[a], _points[a + 1])); }
        else { lines.push_back(ALine(_points[a], _points[0])); }
    }
    PrepareLinesVAO(lines, &_linesVbo, &_linesVao, vecCol);
}

void GLWidget::PreparePointsVAO(std::vector<AVector> points, QOpenGLBuffer* ptsVbo, QOpenGLVertexArrayObject* ptsVao, QVector3D vecCol)
{
    if(ptsVao->isCreated())
    {
        ptsVao->destroy();
    }

    ptsVao->create();
    ptsVao->bind();

    QVector<VertexData> data;
    for(uint a = 0; a < points.size(); a++)
    {
        data.append(VertexData(QVector3D(points[a].x, points[a].y,  0), QVector2D(), vecCol));
    }

    ptsVbo->create();
    ptsVbo->bind();
    ptsVbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    _shaderProgram->enableAttributeArray(_vertexLocation);
    _shaderProgram->setAttributeBuffer(_vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    ptsVao->release();
}

void GLWidget::PrepareLinesVAO(std::vector<ALine> lines, QOpenGLBuffer* linesVbo, QOpenGLVertexArrayObject* linesVao, QVector3D vecCol)
{
    if(linesVao->isCreated())
    {
        linesVao->destroy();
    }

    linesVao->create();
    linesVao->bind();

    QVector<VertexData> data;
    for(uint a = 0; a < lines.size(); a++)
    {
        data.append(VertexData(QVector3D(lines[a].XA, lines[a].YA,  0), QVector2D(), vecCol));
        data.append(VertexData(QVector3D(lines[a].XB, lines[a].YB,  0), QVector2D(), vecCol));
    }

    linesVbo->create();
    linesVbo->bind();
    linesVbo->allocate(data.data(), data.size() * sizeof(VertexData));

    quintptr offset = 0;

    _shaderProgram->enableAttributeArray(_vertexLocation);
    _shaderProgram->setAttributeBuffer(_vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    offset += sizeof(QVector2D);

    _shaderProgram->enableAttributeArray(_colorLocation);
    _shaderProgram->setAttributeBuffer(_colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    linesVao->release();
}

void GLWidget::SaveToSvg()
{
}

void GLWidget::PaintCurve()
{
    //if(_points.size() == 0) { return; }

    _shaderProgram->setUniformValue(_use_color_location, (GLfloat)1.0);

    if(_points.size() != 0)
    {
        glPointSize(5.0f);
        _pointsVao.bind();
        glDrawArrays(GL_POINTS, 0, _points.size());
        _pointsVao.release();
    }

    if(_lines.size() != 0)
    {
        glLineWidth(2.0f);
        _linesVao.bind();
        glDrawArrays(GL_LINES, 0, _lines.size() * 2);
        _linesVao.release();
    }
}



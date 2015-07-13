


#include "GLWidget.h"

#include <iostream>
#include <random>
#include <math.h>

#include <QGLFormat>
#include <QSvgGenerator>

#include "VertexData.h"
#include "SystemParams.h"

GLWidget::GLWidget(QGLFormat format, QWidget *parent) :
    QGLWidget(format, parent),
    _isMouseDown(false),
    _zoomFactor(50.0),
    _shaderProgram(0),
    _patternGenerator(0),
    _img_width(50),
    _img_height(50)
{
}

GLWidget::~GLWidget()
{
    if(_shaderProgram) delete _shaderProgram;
    if(_patternGenerator) delete _patternGenerator;
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

    if(_patternGenerator) { delete _patternGenerator; }
    _patternGenerator = new PatternGenerator();
    _patternGenerator->_shaderProgram = _shaderProgram;
    _patternGenerator->_colorLocation = _colorLocation;
    _patternGenerator->_vertexLocation = _vertexLocation;
    _patternGenerator->_use_color_location = _use_color_location;
    _patternGenerator->_img_width = _img_width;
    _patternGenerator->_img_height = _img_height;
    _patternGenerator->InitTiling();
    _patternGenerator->GeneratePattern(SystemParams::default_tiling);
    //_patternGenerator->GeneratePattern("test");
}

void GLWidget::GeneratePattern(std::string tilingName)
{
    _patternGenerator->GeneratePattern(tilingName);
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

    _patternGenerator->Paint(_zoomFactor);
    //PaintCurve();
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
void GLWidget::ZoomIn() { this->_zoomFactor += 10.0f; }
void GLWidget::ZoomOut() { this->_zoomFactor -= 10.0f; if(this->_zoomFactor < 1.0f) _zoomFactor = 1.0f; }

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

    // error
    //PrepareLinesVAO(lines, &_tilingLinesVbo, &_tilingLinesVao, vecCol);
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
    //std::cout << "save SVG\n";
    std::vector<ALine> tilingLines = _patternGenerator->GetTilingLines();
    std::vector<ALine> uLines = _patternGenerator->GetULines();
    std::vector<ALine> oLines = _patternGenerator->GetOLines();
    std::vector<ALine> triangleLines = _patternGenerator->GetTriangleLines();

    int maxW = 100;
    int maxH = 100;
    float scaleFactor = 10.0f;
    float offset = 250.0f;

    // rescale
    for(int a = 0; a < tilingLines.size(); a++)
    {
        tilingLines[a].XA = tilingLines[a].XA * scaleFactor - offset;
        tilingLines[a].YA = tilingLines[a].YA * scaleFactor - offset;
        tilingLines[a].XB = tilingLines[a].XB * scaleFactor - offset;
        tilingLines[a].YB = tilingLines[a].YB * scaleFactor - offset;
    }
    for(int a = 0; a < triangleLines.size(); a++)
    {
        triangleLines[a].XA = triangleLines[a].XA * scaleFactor - offset;
        triangleLines[a].YA = triangleLines[a].YA * scaleFactor - offset;
        triangleLines[a].XB = triangleLines[a].XB * scaleFactor - offset;
        triangleLines[a].YB = triangleLines[a].YB * scaleFactor - offset;
    }
    for(int a = 0; a < uLines.size(); a++)
    {
        uLines[a].XA = uLines[a].XA * scaleFactor - offset;
        uLines[a].YA = uLines[a].YA * scaleFactor - offset;
        uLines[a].XB = uLines[a].XB * scaleFactor - offset;
        uLines[a].YB =uLines[a].YB * scaleFactor - offset;
    }
    for(int a = 0; a < oLines.size(); a++)
    {
        oLines[a].XA = oLines[a].XA * scaleFactor - offset;
        oLines[a].YA = oLines[a].YA * scaleFactor - offset;
        oLines[a].XB = oLines[a].XB * scaleFactor - offset;
        oLines[a].YB = oLines[a].YB * scaleFactor - offset;
    }



    QSvgGenerator generator;
    generator.setFileName("image.svg");
    generator.setSize(QSize(maxW, maxH));
    generator.setViewBox(QRect(0, 0, maxW, maxH));
    generator.setTitle(tr("Islamic Star Pattern"));
    generator.setDescription(tr("Islamic Star Pattern"));

    QPainter painter;
    painter.begin(&generator);

    // draw
    painter.setClipRect(QRect(0, 0, maxW, maxH));

    painter.setPen(QPen(Qt::red, 0.01, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < tilingLines.size(); a++)
    {
        ALine aLine = tilingLines[a];

        painter.drawLine(QPointF(aLine.XA, aLine.YA),
                         QPointF(aLine.XB, aLine.YB));


    }

    QBrush ribbonBrush;
    QVector3D ribVec = SystemParams::ribbon_color;
    QVector3D lineVec = SystemParams::interlacing_color;
    QVector3D starVec = SystemParams::star_color;
    QColor starCol(starVec.x() * 255, starVec.y() * 255, starVec.z() * 255);
    QColor ribCol(ribVec.x() * 255, ribVec.y() * 255, ribVec.z() * 255);
    QColor lineCol(lineVec.x() * 255, lineVec.y() * 255, lineVec.z() * 255);

    ribbonBrush.setColor(starCol);
    ribbonBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(starCol, 0.04, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < triangleLines.size(); a += 3)
    {
        ALine line1 = triangleLines[a];
        ALine line2 = triangleLines[a+1];
        ALine line3 = triangleLines[a+2];

        QPolygonF poly;
        poly << QPointF(line1.XA, line1.YA)
             << QPointF(line1.XB, line1.YB)
             << QPointF(line3.XA, line3.YA);

        QPainterPath path;
        path.addPolygon(poly);
        painter.drawPolygon(poly);
        painter.fillPath(path, ribbonBrush);
    }

    ribbonBrush.setColor(ribCol);
    ribbonBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(ribCol, 0.04, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < uLines.size(); a += 2)
    {
        ALine line1 = uLines[a];
        ALine line2 = uLines[a+1];

        QPolygonF poly;
        poly << QPointF(line1.XA, line1.YA)
             << QPointF(line1.XB, line1.YB)
             << QPointF(line2.XB, line2.YB)
             << QPointF(line2.XA, line2.YA);

        QPainterPath path;
        path.addPolygon(poly);
        painter.drawPolygon(poly);
        painter.fillPath(path, ribbonBrush);
    }

    painter.setPen(QPen(lineCol, 0.08, Qt::SolidLine, Qt::SquareCap));
    for(int a = 0; a < uLines.size(); a++)
    {
        ALine aLine = uLines[a];
        painter.drawLine(QPointF(aLine.XA, aLine.YA),
                         QPointF(aLine.XB, aLine.YB));


    }


    ribbonBrush.setColor(ribCol);
    ribbonBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(ribCol, 0.04, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < oLines.size(); a += 2)
    {
        ALine line1 = oLines[a];
        ALine line2 = oLines[a+1];

        QPolygonF poly;
        poly << QPointF(line1.XA, line1.YA)
             << QPointF(line1.XB, line1.YB)
             << QPointF(line2.XB, line2.YB)
             << QPointF(line2.XA, line2.YA);

        QPainterPath path;
        path.addPolygon(poly);
        painter.drawPolygon(poly);
        painter.fillPath(path, ribbonBrush);
    }

    painter.setPen(QPen(lineCol, 0.08, Qt::SolidLine, Qt::SquareCap));
    for(int a = 0; a < oLines.size(); a++)
    {
        ALine aLine = oLines[a];

        painter.drawLine(QPointF(aLine.XA, aLine.YA),
                         QPointF(aLine.XB, aLine.YB));
    }


    painter.end();

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

//    if(_tilingLines.size() != 0)
//    {
//        glLineWidth(2.0f);
//        _tilingLinesVao.bind();
//        glDrawArrays(GL_LINES, 0, _tilingLines.size() * 2);
//        _tilingLinesVao.release();
//    }
}



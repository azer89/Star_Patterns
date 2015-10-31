


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
    QVector3D backColor = SystemParams::background_color;
    glClearColor( backColor.x(), backColor.y(), backColor.z(), 1.0 );
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
    QVector3D backColor = SystemParams::background_color;
    glClearColor( backColor.x(), backColor.y(), backColor.z(), 1.0 );
    //glClearColor( 0, 0, 0, 1.0 );

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


void GLWidget::ResizeLines(std::vector<ALine> &lines, AVector offsetVec, float scaleFactor)
{
    for(int a = 0; a < lines.size(); a++)
    {
        lines[a].XA -= offsetVec.x;
        lines[a].YA -= offsetVec.y;
        lines[a].XB -= offsetVec.x;
        lines[a].YB -= offsetVec.y;
        lines[a].XA *= scaleFactor;
        lines[a].YA *= scaleFactor;
        lines[a].XB *= scaleFactor;
        lines[a].YB *= scaleFactor;
    }
}


void GLWidget::SaveToSvg()
{
    std::vector<ALine> tilingLines = _patternGenerator->GetTilingLines();
    std::vector<ALine> uLines = _patternGenerator->GetULines();
    std::vector<ALine> oLines = _patternGenerator->GetOLines();
    std::vector<ALine> triangleLines = _patternGenerator->GetTriangleLines();
    std::vector<ALine> backTriangleLines = _patternGenerator->GetBackTriangleLines();
    std::vector<ALine> addTriangleLines = _patternGenerator->GetAddTriangleLines();

    float xLeft = 0 + _scrollOffset.x();
    float yTop = 0 + _scrollOffset.y();

    float invScale = 1.0 / this->_zoomFactor;
    xLeft   *= invScale;
    yTop    *= invScale;

    AVector offsetVec(xLeft, yTop);

    ResizeLines(tilingLines,       offsetVec, _zoomFactor);
    ResizeLines(uLines,            offsetVec, _zoomFactor);
    ResizeLines(oLines,            offsetVec, _zoomFactor);
    ResizeLines(triangleLines,     offsetVec, _zoomFactor);
    ResizeLines(backTriangleLines, offsetVec, _zoomFactor);
    ResizeLines(addTriangleLines,  offsetVec, _zoomFactor);

    QSvgGenerator generator;
    generator.setFileName("image.svg");
    generator.setSize(QSize(this->width(), this->height()));
    generator.setViewBox(QRect(0, 0, this->width(), this->height()));
    generator.setTitle(tr("Islamic Star Pattern"));
    generator.setDescription(tr("Islamic Star Pattern"));
    QPainter painter;
    painter.begin(&generator);
    painter.setClipRect(QRect(0, 0, this->width(), this->height()));

    QBrush myBrush;
    QVector3D backVec = SystemParams::background_color;
    QColor backCol(backVec.x() * 255, backVec.y() * 255, backVec.z() * 255);
    myBrush.setColor(backCol);
    myBrush.setStyle(Qt::SolidPattern);
    painter.fillRect(QRect(0, 0, this->width(), this->height()), myBrush);

    QVector3D ribVec = SystemParams::ribbon_color;
    QVector3D lineVec = SystemParams::interlacing_color;
    QVector3D starVec = SystemParams::star_color;
    QColor starCol(starVec.x() * 255, starVec.y() * 255, starVec.z() * 255);
    QColor ribCol(ribVec.x() * 255, ribVec.y() * 255, ribVec.z() * 255);
    QColor lineCol(lineVec.x() * 255, lineVec.y() * 255, lineVec.z() * 255);

    // star triangles
    myBrush.setColor(starCol);
    myBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(starCol, 0.75, Qt::SolidLine, Qt::RoundCap));
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
        painter.fillPath(path, myBrush);
    }

    // back triangles
    myBrush.setColor(backCol);
    myBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(backCol, 0.75, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < backTriangleLines.size(); a += 3)
    {
        ALine line1 = backTriangleLines[a];
        ALine line2 = backTriangleLines[a+1];
        ALine line3 = backTriangleLines[a+2];

        QPolygonF poly;
        poly << QPointF(line1.XA, line1.YA)
             << QPointF(line1.XB, line1.YB)
             << QPointF(line3.XA, line3.YA);

        QPainterPath path;
        path.addPolygon(poly);
        painter.drawPolygon(poly);
        painter.fillPath(path, myBrush);
    }

    // add triangles
    myBrush.setColor(starCol);
    myBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(starCol, 0.75, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < addTriangleLines.size(); a += 3)
    {
        ALine line1 = addTriangleLines[a];
        ALine line2 = addTriangleLines[a+1];
        ALine line3 = addTriangleLines[a+2];

        QPolygonF poly;
        poly << QPointF(line1.XA, line1.YA)
             << QPointF(line1.XB, line1.YB)
             << QPointF(line3.XA, line3.YA);

        QPainterPath path;
        path.addPolygon(poly);
        painter.drawPolygon(poly);
        painter.fillPath(path, myBrush);
    }

    // under ribbons
    myBrush.setColor(ribCol);
    myBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(ribCol, 0.25, Qt::SolidLine, Qt::RoundCap));
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
        painter.fillPath(path, myBrush);
    }

    painter.setPen(QPen(lineCol, SystemParams::line_width * _zoomFactor, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < uLines.size(); a++)
    {
        ALine aLine = uLines[a];
        painter.drawLine(QPointF(aLine.XA, aLine.YA),
                         QPointF(aLine.XB, aLine.YB));
    }


    myBrush.setColor(ribCol);
    myBrush.setStyle(Qt::SolidPattern);
    painter.setPen(QPen(ribCol, 0.25, Qt::SolidLine, Qt::RoundCap));
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
        painter.fillPath(path, myBrush);
    }

    painter.setPen(QPen(lineCol, SystemParams::line_width * _zoomFactor, Qt::SolidLine, Qt::RoundCap));
    for(int a = 0; a < oLines.size(); a++)
    {
        ALine aLine = oLines[a];

        painter.drawLine(QPointF(aLine.XA, aLine.YA),
                         QPointF(aLine.XB, aLine.YB));
    }


    painter.end();

}







//#include "stdafx.h"

#include "GLContainer.h"

#include <iostream>

#include <QApplication>
#include <QEvent>
#include <QGLFormat>
#include <QGridLayout>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

GLContainer::GLContainer(QWidget *parent) :
    QAbstractScrollArea (parent),
    _ctrlPressed(false),
    _mousePressed(false),
    _sWidth(20),
    _sHeight(20),
    _prevNum(-1),
    _scrollMoved(false)
{
    QGLFormat format;
    format.setVersion(4, 0);
    format.setProfile(QGLFormat::CompatibilityProfile);
    format.setSampleBuffers(true);

    _glWidget = new GLWidget(format);
    _glWidget->setObjectName(QStringLiteral("myGLImageDisplay"));

    setViewport(_glWidget);

    horizontalScrollBar()->setSingleStep(10);
    horizontalScrollBar()->setPageStep(100);

    verticalScrollBar()->setSingleStep(10);
    verticalScrollBar()->setPageStep(100);

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),this, SLOT(HScrollChanged(int)));
    connect(verticalScrollBar(),   SIGNAL(valueChanged(int)),this, SLOT(VScrollChanged(int)));

    setMouseTracking(true);

    _doubleClickTimer = new QTimer(this); connect(_doubleClickTimer, SIGNAL(timeout()), this, SLOT(DummyFunction()));
    _doubleClickTimeout = 100;

    this->_justInitialized = true;
}

void GLContainer::UpdateViewport(bool putInMiddle)
{
    QSize barSize = QSize(this->width() - _sWidth, this->height() - _sHeight);

    float zoomFactor = _glWidget->GetZoomFactor();
    QSize canvasSize  = _glWidget->GetCanvasSize();

    //QSize borderLimit( 10, 10);

    int img_width =	canvasSize.width()  * zoomFactor;
    int img_height = canvasSize.height() * zoomFactor;

    //std::cout << img_width << " " << img_height << "\n";

    if(img_width == 0 || img_height == 0)
    {
        img_width = 100 * zoomFactor;
        img_height  = 100 * zoomFactor;
    }

    float xSPos;
    float ySPos;
    if(!putInMiddle)
    {
        float xNormPos = _mousePos.x() + _xPrevF;
        float yNormPos = _mousePos.y() + _yPrevF;
        xNormPos /= _prevZoomFactor;
        yNormPos /= _prevZoomFactor;

        float xRev = xNormPos * zoomFactor;
        float yRev = yNormPos * zoomFactor;
        xSPos = xRev - _mousePos.x();
        ySPos = yRev - _mousePos.y();
    }

    int leftRange = 0;
    int rightRange = 0;
    int upRange = 0;
    int downRange = 0;

    float hPos = 0;
    float vPos = 0;

    int xGap = abs(barSize.width() - img_width);
    int yGap = abs(barSize.height() - img_height);

    if(img_width <= barSize.width())
    {
        if(putInMiddle)hPos = -xGap * 0.5; //
        else hPos = xSPos;

        leftRange = -img_width - xGap;
        rightRange = img_width;

    }
    else
    {
        if(putInMiddle) hPos = xGap * 0.5;
        else hPos = xSPos;
        leftRange = -img_width + xGap;
        rightRange = img_width;
    }

    if(img_height <= barSize.height())
    {
        if(putInMiddle) vPos = -yGap * 0.5;
        else vPos = ySPos;

        upRange =  -img_height -yGap;
        downRange = img_height;

    }
    else
    {
        if(putInMiddle) vPos = yGap * 0.5;
        else vPos = ySPos;
        upRange = -img_height + yGap;
        downRange = img_height;
    }

    _xPrevF = hPos;
    _yPrevF = vPos;

    horizontalScrollBar()->setRange(leftRange, rightRange);
    verticalScrollBar()->setRange(upRange, downRange);

    horizontalScrollBar()->setSliderPosition(hPos);
    verticalScrollBar()->setSliderPosition(vPos);
}

void GLContainer::paintEvent(QPaintEvent *event)
{
    if(this->_justInitialized)
    {
        UpdateViewport(true);
        this->_justInitialized = false;
    }

    // please fix me
    if(this->width() != _glWidget->width() || this->height() != _glWidget->height())
    {
        _glWidget->setFixedWidth(this->width());
        _glWidget->setFixedHeight(this->height());
    }

    QAbstractScrollArea::paintEvent(event);
    _glWidget->updateGL();
}

bool GLContainer::event(QEvent * event)
{
    //if(scrollMoved)
    //{
    //    UpdateViewport(false);
    //}
    //else
    //{

    //    UpdateViewport(true);

    //}

    if(event->type() == QEvent::Resize)
    {
        //std::cout << "resize\n";
    }

    bool evResult = QAbstractScrollArea::event(event);
    return evResult;
}

void GLContainer::SetScrolls()
{
    horizontalScrollBar()->setVisible(true);
    verticalScrollBar()->setVisible(true);

    _prevZoomFactor = 1.0f;

    std::cout << "SetScrolls\n";

    // nasty code here...
    bool shouldZoom = true;
    do
    {
        //int _w = this->width();
        //int _h = this->height();
        QSize imgSize = _glWidget->GetCanvasSize();

        if(imgSize.width() == 0 || imgSize.height() == 0)
        {
            imgSize = QSize(100, 100);
        }

        std::cout << "image size " << imgSize.width() << " - " << imgSize.height() << "\n";
        double zoomFactor = _glWidget->GetZoomFactor();

        if((double)this->width()  < (double)imgSize.width() * zoomFactor ||  (double)this->height() < (double)imgSize.height() *zoomFactor)
        { _glWidget->ZoomOut(); }
        else
        { shouldZoom = false; }
    } while (shouldZoom);
    UpdateViewport(true);
}

void GLContainer::VScrollChanged(int val)
{
    _yPrevF = val;
    _glWidget->VerticalScroll(val);
    this->_scrollMoved = true;
}
void GLContainer::HScrollChanged(int val)
{
    _xPrevF = val;
    _glWidget->HorizontalScroll(val);
    this->_scrollMoved = true;
}

void GLContainer::DummyFunction()
{
    _doubleClickTimer->stop();
}

void GLContainer::mousePressEvent(QMouseEvent *event)
{
    if(_doubleClickTimer->isActive())
    {
        _doubleClickTimer->stop();
        _glWidget->mouseDoubleClick(event->x(), event->y());
    }
    else
    {
    }

    this->_mousePressed = true;

    if(!this->_ctrlPressed)
    {
        _glWidget->mousePressEvent(event->x(), event->y());
    }

    if(this->_ctrlPressed)
    {
        this->_prevMousePos = _mousePos;
        this->_prevScrollPos.setX(horizontalScrollBar()->sliderPosition());
        this->_prevScrollPos.setY(verticalScrollBar()->sliderPosition());
    }
}

void GLContainer::mouseMoveEvent(QMouseEvent *event)
{
    _mousePos.setX(event->x());
    _mousePos.setY(event->y());

    if(this->_ctrlPressed && this->_mousePressed)
    {
        int xDelta = _mousePos.x() - _prevMousePos.x();
        int yDelta = _mousePos.y() - _prevMousePos.y();
        horizontalScrollBar()->setSliderPosition(_prevScrollPos.x() - xDelta);
        verticalScrollBar()->setSliderPosition(_prevScrollPos.y() - yDelta);
    }

    _glWidget->mouseMoveEvent(event->x(), event->y());
}

void GLContainer::mouseReleaseEvent(QMouseEvent *event)
{
    this->_mousePressed = false;
    if(!this->_ctrlPressed)
    {
        _glWidget->mouseReleaseEvent(event->x(), event->y());
    }

    _doubleClickTimer->start(_doubleClickTimeout);
}

void GLContainer::wheelEvent(QWheelEvent* event)
{
    bool scrollDir = (event->delta() > 0) ? true : false;	// negative means scroll down, positive is otherwise
    _prevZoomFactor = _glWidget->GetZoomFactor();		// for anchor zoom

    if(scrollDir) _glWidget->ZoomOut();
    else _glWidget->ZoomIn();

    float zoomFactor = _glWidget->GetZoomFactor() * 100.0;

    // update scrollbars
    UpdateViewport();
}

void GLContainer::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        this->_ctrlPressed = true;
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
    }

    /*if(event->key() == Qt::Key_Right)
    {
        //std::cout << "right\n";
        _glWidget->IncreaseDiv();
    }
    else if(event->key() == Qt::Key_Left)
    {
        //std::cout << "left\n";
        _glWidget->DecreaseDiv();
    }*/

    //if(event->key() == Qt::Key_C) { this->glWidget->DoClustering(); }


    //_glWidget->updateGL();
    _glWidget->repaint();
}

void GLContainer::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        this->_ctrlPressed = false;
        QApplication::restoreOverrideCursor();
    }
}

// get renderer
GLWidget* GLContainer::GetGLWidget()
{
    return this->_glWidget;
}

#ifndef GLCONTAINER_H
#define GLCONTAINER_H

//#include "stdafx.h"
#include <QAbstractScrollArea>

#include "GLWidget.h"

class GLContainer : public QAbstractScrollArea
{
    Q_OBJECT

public:
    // constructor
    GLContainer(QWidget *parent = 0);

    // for Qt internal use
    void setWidget(QWidget *widget);
    QWidget *widget() const;

    // get renderer
    GLWidget* GetGLWidget();

    // set up scrolls (vertical and horizontal)
    void SetScrolls();


protected:
    // global event
    bool event( QEvent * event );

    // draw
    void paintEvent(QPaintEvent *event);

    // mouse events
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    // mouse wheel
    void wheelEvent(QWheelEvent* event);

    // keyboard
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    GLWidget*   _glWidget;
    QPoint      _prevScrollPos;
    QPoint      _prevMousePos;
    bool        _ctrlPressed;
    bool        _mousePressed;

    int     _sWidth;
    int     _sHeight;
    QPoint  _mousePos;

    // for updating opengl when scrolling display
    float _xPrevF;
    float _yPrevF;
    float _prevZoomFactor;

    // previous number of strokes (not used)
    int _prevNum;

    // timer for double click
    QTimer* _doubleClickTimer;

    // timing for double click
    int     _doubleClickTimeout;

    bool _scrollMoved;
    bool _justInitialized;

private:
    // update opengl viewport
    void UpdateViewport(bool putInMiddle = false);

signals:

private slots:
    // vertical scroll
    void VScrollChanged(int val);

    // horizontal scroll
    void HScrollChanged(int val);

    // for double click effect
    void DummyFunction();
};

#endif // GLCONTAINER_H

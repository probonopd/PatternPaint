#include "patterneditor.h"

#include <iostream>
#include <cmath>
#include <QtWidgets>
#include <QDebug>

#define COLOR_CLEAR             QColor(0,0,0,0)
#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,0)
#define COLOR_GRID_LINES        QColor(30,30,30,200)
#define COLOR_GRID_EDGES        QColor(30,30,30, 60)

#define COLOR_TOOL_DEFAULT      QColor(255,255,255)

#define COLOR_PLAYBACK_EDGE     QColor(255,255,255,255)
#define COLOR_PLAYBACK_TOP      QColor(255,255,255,100)

PatternEditor::PatternEditor(QWidget *parent) :
    QWidget(parent)
{
}

void PatternEditor::resizeEvent(QResizeEvent * event)
{
    updateGridSize();
}

void PatternEditor::init(int frameCount, int stripLength)
{
    // Initialize the pattern to a blank canvass
    pattern = QImage(frameCount,
                     stripLength,
                     QImage::Format_RGB32);
    pattern.fill(COLOR_CANVAS_DEFAULT);

    toolPreview = QImage(frameCount,
                         stripLength,
                         QImage::Format_ARGB32);
    toolPreview.fill(COLOR_CLEAR);

    // TODO: Don't reset these here, they need to come from main...
    toolColor = COLOR_TOOL_DEFAULT;
    toolSize = 2;

    // Turn on mouse tracking so we can draw a preview
    setMouseTracking(true);

    updateGridSize();
    update();
}

bool PatternEditor::init(QImage newPattern, bool scaled) {
    // TODO: Implement 'save' check before overwriting?

    // If the pattern doesn't fit, scale it.
    // TODO: Display an import dialog to let the user decide what to do?
    if(scaled && newPattern.height() != pattern.height()) {
        newPattern = newPattern.scaledToHeight(pattern.height());
    }

    // Re-init the display using the new geometry
    init(newPattern.width(), newPattern.height());

    // Draw the new pattern to the display
    QPainter painter(&pattern);
    painter.drawImage(0,0,newPattern);

    // and force a screen update
    update();

    return true;
}

void PatternEditor::updateGridSize() {
    // Set the x and y scale based on the widget size
    // TODO: resize more intelligently (maybe minimum of both, or add scroll bars?)
    xScale = float(size().height() - 1)/pattern.height();
    yScale = float(size().height() - 1)/pattern.height();

    // And make a grid pattern to superimpose over the image
    //
    gridPattern = QImage(pattern.width()*xScale  +.5 + 1,
                         pattern.height()*yScale +.5 + 1,
                         QImage::Format_ARGB32);
    gridPattern.fill(COLOR_CLEAR);

    QPainter painter(&gridPattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Draw vertical lines
    painter.setPen(COLOR_GRID_LINES);
    for(int x = 0; x <= pattern.width(); x++) {
        painter.drawLine(x*xScale+.5,
                         0,
                         x*xScale+.5,
                         gridPattern.height());
    }

    // Draw horizontal lines
    for(int y = 0; y <= pattern.height(); y++) {
        painter.drawLine(0,
                         y*yScale+.5,
                         gridPattern.width(),
                         y*yScale+.5);
    }

    // Draw corners
    painter.setPen(COLOR_GRID_EDGES);
    for(int x = 0; x <= pattern.width(); x++) {
        for(int y = 0; y <= pattern.height(); y++) {
            painter.drawPoint(QPoint(x*xScale     +.5 +1,    y*yScale     +.5 +1));
            painter.drawPoint(QPoint((x+1)*xScale +.5 -1,    y*yScale     +.5 +1));
            painter.drawPoint(QPoint(x*xScale     +.5 +1,    (y+1)*yScale +.5 -1));
            painter.drawPoint(QPoint((x+1)*xScale +.5 -1,    (y+1)*yScale +.5 -1));
        }
    }
}

void PatternEditor::mousePressEvent(QMouseEvent *event){
    int x = event->x()/xScale;
    int y = event->y()/yScale;

    QPainter painter(&pattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(toolColor);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(toolColor);
    painter.setBrush(brush);

    painter.drawPoint(x,y);
    painter.drawEllipse ( QPoint(x,y), toolSize/2, toolSize/2 );

    update();
}

void PatternEditor::leaveEvent(QEvent * event) {
    toolPreview.fill(COLOR_CLEAR);

    update();
}

void PatternEditor::mouseMoveEvent(QMouseEvent *event){
    static int oldX = -1;
    static int oldY = -1;

    int x = event->x()/xScale;
    int y = event->y()/yScale;

    // If the position hasn't changed, don't do anything.
    // This is to improve responsiveness on slower computers.
    if(x == oldX && y == oldY) {
        return;
    }

    oldX = x;
    oldY = y;

    // If we aren't pressed down, just draw a preview
    QPainter painter(&toolPreview);
    toolPreview.fill(COLOR_CLEAR);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(toolColor);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(toolColor);
    painter.setBrush(brush);

    painter.drawPoint(x,y);
    painter.drawEllipse ( QPoint(x,y), toolSize/2, toolSize/2 );

    if( event->buttons() &  Qt::LeftButton ) {
        QPainter painter(&pattern);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(toolColor);


        QBrush brush = painter.brush();
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(toolColor);
        painter.setBrush(brush);

        painter.drawPoint(x,y);
        painter.drawEllipse ( QPoint(x,y), toolSize/2, toolSize/2 );
    }

    update();
}

void PatternEditor::setToolColor(QColor color) {
    toolColor = color;
}

void PatternEditor::setToolSize(int size) {
    toolSize = size;
}

void PatternEditor::setPlaybackRow(int row) {
    playbackRow = row;
    update();
}

void PatternEditor::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawRect(0,0,width()-1, height()-1);

    // Draw the image and tool preview
    painter.drawImage(QRect(0,0,pattern.width()*xScale+.5,pattern.height()*yScale), pattern);
    painter.drawImage(QRect(0,0,pattern.width()*xScale+.5,pattern.height()*yScale), toolPreview);
    painter.drawImage(0,0,gridPattern);

    // Draw the playback indicator
    // Note that we need to compute the correct width based on the rounding error of
    // the current cell, otherwise it won't line up correctly with the actual image.
    painter.setPen(COLOR_PLAYBACK_EDGE);
    painter.drawRect(playbackRow*xScale +.5,
                     0,
                     int((playbackRow+1)*xScale +.5) - int(playbackRow*xScale +.5),
                     pattern.height()*yScale);
    painter.fillRect(playbackRow*xScale +.5,
                     0,
                     int((playbackRow+1)*xScale +.5) - int(playbackRow*xScale +.5),
                     pattern.height()*yScale,
                     COLOR_PLAYBACK_TOP);

//    // And a scrubber
//    painter.setPen(COLOR_PLAYBACK_EDGE);
//    painter.drawPolygon();
}

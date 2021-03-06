#ifndef PATTERNDITOR_H
#define PATTERNDITOR_H

#include "fixture.h"
#include "pattern.h"

#include <QWidget>
#include <QPointer>

class QUndoStack;
class PatternFrameUndoCommand;
class AbstractInstrument;

class FrameEditor : public QWidget
{
    Q_OBJECT
public:
    explicit FrameEditor(QWidget *parent = 0);

    /// Re-size the display grid and selector based on new widget geometery
    void resizeEvent(QResizeEvent *resizeEvent);

    // Pass messages to the selected instrument
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

    // Handle file URLs dropped from other programs
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

public:

    /// Instrument interface

    /// Get the image data for the current pattern
    /// @return QImage containing the current pattern
    const QImage &getPatternAsImage() const;

    /// Update the pattern with the given changes.
    /// @param update RGBA QImage to draw on top of the current
    void applyInstrument(QImage &update);

    QColor getPrimaryColor() const;

    int getPenSize() const;

private:

    QImage frameData;     ///< Frame image
    int frameIndex;                 ///< Frame index

    QPointer<AbstractInstrument> instrument;

    QImage gridPattern;    ///< Holds the pre-rendered grid overlay

    float pixelScale;      ///< Number of pixels in the grid pattern per pattern pixel.

    QColor toolColor;      ///< Color of the current drawing tool (TODO: This should be a pointer to a tool)
    int toolSize;          ///< Size of the current drawing tool (TODO: This should be a pointer to a tool)

    bool showPlaybackIndicator;

    QPointer<Fixture> fixture;

    /// Redraw the gridPattern to fit the current widget size.
    void updateGridSize();

    /// Update the screen, but only if we haven't done so in a while
    void lazyUpdate();

    /// True if the editor has an image to edit
    bool hasImage();

    void paintEvent(QPaintEvent *event);

signals:
    void dataEdited(int index, const QImage data);

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
    void setInstrument(AbstractInstrument *);
    void setFixture(Fixture *);
    void setShowPlaybakIndicator(bool);

    void setFrameData(int index, const QImage data);
};

#endif // PATTERNDITOR_H

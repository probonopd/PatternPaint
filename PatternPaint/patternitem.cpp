#include "patternitem.h"
#include "undocommand.h"

#include <QPainter>
#include <QDebug>

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)

PatternItem::PatternItem(int patternLength, int ledCount, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    modified(false)
{
    undoStack.setUndoLimit(50);

    QImage newImage(patternLength, ledCount, QImage::Format_ARGB32_Premultiplied);
    newImage.fill(COLOR_CANVAS_DEFAULT);
    applyUndoState(newImage);
}

PatternItem::PatternItem(int ledCount, QImage newImage, QListWidget* parent) :
    QListWidgetItem(parent, QListWidgetItem::UserType + 1),
    modified(false)
{
    undoStack.setUndoLimit(50);

    image = newImage.scaledToHeight(ledCount);
}

void PatternItem::pushUndoState() {
    undoStack.push(new UndoCommand(image, this));
}

void PatternItem::applyUndoState(const QImage& newImage) {
    image = newImage;

    if(!notifier.isNull()) {
        notifier->signalSizeUpdated();
    }

    setModified(modified);
}

QString PatternItem::getPatternName() const
{
    if(fileInfo.baseName() == "") {
        return "Untitled";
    }
    return fileInfo.baseName();
}

bool PatternItem::load(const QFileInfo &newFileInfo)
{
    // TODO: Fail if there is unsaved data?

    // Attempt to load the iamge
    if(!replace(newFileInfo)) {
        return false;
    }

    // If successful, record the filename and clear the undo stack.
    fileInfo = newFileInfo;

    undoStack.clear();

    setModified(false);
    return true;
}

bool PatternItem::saveAs(const QFileInfo &newFileInfo) {
    // Attempt to save to this location
    if(!image.save(newFileInfo.absoluteFilePath())) {
        return false;
    }

    // If successful, update the filename
    fileInfo = newFileInfo;

    // TODO: Notify the main window that the filename was updated!
    //on_patternFilenameChanged(fileinfo);

    if(!notifier.isNull()) {
        notifier->signalNameUpdated();
    }

    setModified(false);
    return true;
}

bool PatternItem::replace(const QFileInfo &newFileInfo)
{
    pushUndoState();

    QImage newImage;

    // Attempt to load the iamge
    if(!newImage.load(newFileInfo.absoluteFilePath())) {
        return false;
    }

    // Scale the image (TODO: present a dialog for this?)
    image = newImage.scaledToHeight(image.height());

    if(!notifier.isNull()) {
        notifier->signalSizeUpdated();
    }

    setModified(true);
    return true;
}


bool PatternItem::save()
{
    if(!image.save(fileInfo.absoluteFilePath())) {
        return false;
    }

    // TODO: Set new save point here
    setModified(false);
    return true;
}

QVariant PatternItem::data(int role) const {
    switch(role) {
        case PreviewImage: return image;
        case Modified: return modified;
        case PatternSize: return image.size();
    };

    return QListWidgetItem::data(role);
}

void PatternItem::setData(int role, const QVariant& value) {
    switch(role)
    {
    case PreviewImage:
        applyUndoState(qvariant_cast<QImage>(value));
        break;
    case Modified:
        setModified(qvariant_cast<bool>(value));
        break;
    case PatternSize:
        Q_ASSERT(false);    // never set size separated from image!
        break;
    default:
        break;
    }

    QListWidgetItem::setData(role, value);
}

void PatternItem::resize(int newPatternLength, int newLedCount, bool scale) {
    pushUndoState();

    QImage originalImage = image;

    if(scale && newLedCount != originalImage.height()) {
        originalImage = originalImage.scaledToHeight(newLedCount);
    }

    // Initialize the pattern to a blank canvass
    image = QImage(newPatternLength,
                     newLedCount,
                     QImage::Format_ARGB32_Premultiplied);
    image.fill(COLOR_CANVAS_DEFAULT);

    QPainter painter(&image);
    painter.drawImage(0,0,originalImage);

    if(!notifier.isNull()) {
        notifier->signalSizeUpdated();
    }

    setModified(true);
}

void PatternItem::applyInstrument(const QImage &update)
{
    pushUndoState();
    QPainter painter(&image);
    painter.drawImage(0,0,update);
    setModified(true);
}

void PatternItem::setModified(bool newModified)  {
    bool modifiedChanged = false;

    if(modified != newModified) {
        modifiedChanged = true;
    }

    modified = newModified;

    if(!notifier.isNull()) {
        notifier->signalDataUpdated();
        if(modifiedChanged) {
            notifier->signalModifiedChange();
        }
    }
}

void PatternItem::setNotifier(QPointer<PatternUpdateNotifier> newNotifier) {
    notifier = newNotifier;
}

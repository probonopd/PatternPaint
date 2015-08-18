#ifndef TIMELINEDISPLAY_H
#define TIMELINEDISPLAY_H

#include "displaymodel.h"


class TimelineDisplay : public DisplayModel {
public:

    bool hasFixedLedCount() const { return false; }
    int getFixedLedCount() const { return 0; }

    void setSource(PatternItem* newPatternItem);
    bool hasPatternItem() const;

    bool showPlaybackIndicator() const { return true;}

    int getFrameCount() const;

    void setFrameIndex(int newFrame);
    int getFrameIndex() const;

    void deleteFrame(int newFrame);
    void addFrame(int newFrame);

    const QImage& getFrameData();
    void applyInstrument(const QImage& instrumentFrameData) ;

private:
    PatternItem* patternItem;
    int frame;
};

#endif // TIMELINEDISPLAY_H

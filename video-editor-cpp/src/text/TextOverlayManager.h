#ifndef TEXTOVERLAYMANAGER_H
#define TEXTOVERLAYMANAGER_H

#include <QObject>
#include <QList>
#include "TextOverlay.h"
#include <Mlt.h>

class Mlt::Producer;
class TimelineWidget;

class TextOverlayManager : public QObject {
    Q_OBJECT
public:
    explicit TextOverlayManager(QObject *parent = nullptr);
    
    TextOverlay* addTextOverlay();
    void removeTextOverlay(TextOverlay *overlay);
    QList<TextOverlay*> textOverlays() const;
    
    Mlt::Producer* createMltProducer(TextOverlay *overlay, Mlt::Profile *profile);
    
signals:
    void overlayAdded(TextOverlay *overlay);
    void overlayRemoved(TextOverlay *overlay);
    
private:
    QList<TextOverlay*> m_overlays;
};

#endif // TEXTOVERLAYMANAGER_H 
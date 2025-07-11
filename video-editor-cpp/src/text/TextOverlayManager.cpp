#include "TextOverlayManager.h"
#include <mlt++/MltProducer.h>
#include <mlt++/MltProfile.h>

TextOverlayManager::TextOverlayManager(QObject *parent) : QObject(parent) {}

TextOverlay* TextOverlayManager::addTextOverlay() {
    TextOverlay *overlay = new TextOverlay(this);
    m_overlays.append(overlay);
    emit overlayAdded(overlay);
    return overlay;
}

void TextOverlayManager::removeTextOverlay(TextOverlay *overlay) {
    if (m_overlays.removeOne(overlay)) {
        overlay->deleteLater();
        emit overlayRemoved(overlay);
    }
}

QList<TextOverlay*> TextOverlayManager::textOverlays() const {
    return m_overlays;
}

Mlt::Producer* TextOverlayManager::createMltProducer(TextOverlay *overlay, Mlt::Profile *profile) {
    if (!overlay || !profile) return nullptr;
    
    Mlt::Producer *producer = new Mlt::Producer(*profile, "qtext:");
    if (!producer || !producer->is_valid()) {
        delete producer;
        return nullptr;
    }
    
    producer->set("text", overlay->text().toUtf8().constData());
    producer->set("length", overlay->duration());
    producer->set("family", overlay->font().family().toUtf8().constData());
    producer->set("size", overlay->font().pointSize());
    producer->set("weight", overlay->font().bold() ? "bold" : "normal");
    producer->set("fgcolour", overlay->color().name().toUtf8().constData());
    producer->set("bgcolour", "transparent");
    
    return producer;
} 
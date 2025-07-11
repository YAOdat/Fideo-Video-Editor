#include "TextOverlay.h"

TextOverlay::TextOverlay(QObject *parent) : QObject(parent),
    m_text("Your Text Here"),
    m_color(Qt::white),
    m_duration(50),
    m_startFrame(0)
{
    m_font.setFamily("Arial");
    m_font.setPointSize(36);
    m_font.setBold(true);
}

QString TextOverlay::text() const { return m_text; }
void TextOverlay::setText(const QString &text) {
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

QColor TextOverlay::color() const { return m_color; }
void TextOverlay::setColor(const QColor &color) {
    if (m_color != color) {
        m_color = color;
        emit styleChanged();
    }
}

QFont TextOverlay::font() const { return m_font; }
void TextOverlay::setFont(const QFont &font) {
    if (m_font != font) {
        m_font = font;
        emit styleChanged();
    }
}

int TextOverlay::duration() const { return m_duration; }
void TextOverlay::setDuration(int duration) {
    if (m_duration != duration) {
        m_duration = duration;
        emit styleChanged();
    }
}

int TextOverlay::startFrame() const { return m_startFrame; }
void TextOverlay::setStartFrame(int frame) {
    if (m_startFrame != frame) {
        m_startFrame = frame;
        emit styleChanged();
    }
}

QString TextOverlay::styleString() const {
    return QString("font=%1,fontsize=%2,fontweight=%3,fgcolour=%4")
        .arg(m_font.family())
        .arg(m_font.pointSize())
        .arg(m_font.bold() ? "bold" : "normal")
        .arg(m_color.name());
}
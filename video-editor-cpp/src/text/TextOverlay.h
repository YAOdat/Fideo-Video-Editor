#ifndef TEXTOVERLAY_H
#define TEXTOVERLAY_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QFont>

class TextOverlay : public QObject {
    Q_OBJECT
public:
    explicit TextOverlay(QObject *parent = nullptr);
    
    QString text() const;
    void setText(const QString &text);
    
    QColor color() const;
    void setColor(const QColor &color);
    
    QFont font() const;
    void setFont(const QFont &font);
    
    int duration() const;
    void setDuration(int duration);
    
    int startFrame() const;
    void setStartFrame(int frame);
    
    QString styleString() const;
    
signals:
    void textChanged();
    void styleChanged();
    
private:
    QString m_text;
    QColor m_color;
    QFont m_font;
    int m_duration;
    int m_startFrame;
};

#endif // TEXTOVERLAY_H
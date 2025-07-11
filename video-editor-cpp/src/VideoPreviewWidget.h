#pragma once
#include <QWidget>
#include <QLabel>
#include <QImage>

class VideoPreviewWidget : public QLabel {
    Q_OBJECT
public:
    VideoPreviewWidget(QWidget* parent = nullptr);
    void setFrame(const QImage& image);
}; 
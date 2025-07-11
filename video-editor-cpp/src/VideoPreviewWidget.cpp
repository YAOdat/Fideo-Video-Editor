#include "VideoPreviewWidget.h"

VideoPreviewWidget::VideoPreviewWidget(QWidget* parent)
    : QLabel(parent) {
    setAlignment(Qt::AlignCenter);
    setMinimumSize(640, 360);
    setText("No video loaded");
}

void VideoPreviewWidget::setFrame(const QImage& image) {
    if (!image.isNull()) {
        setPixmap(QPixmap::fromImage(image).scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        setText("No video loaded");
        setPixmap(QPixmap());
    }
} 
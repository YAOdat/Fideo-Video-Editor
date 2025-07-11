#include "FileUtils.h"
#include <QFileInfo>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QMessageBox>

bool isImageFile(const QString &fileName) {
    QString ext = fileName.split('.').last().toLower();
    return ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" || ext == "gif";
}

bool isVideoFile(const QString &fileName) {
    QString ext = fileName.split('.').last().toLower();
    return ext == "mp4" || ext == "avi" || ext == "mov" || ext == "mkv" || ext == "wmv";
}

QPixmap extractVideoFrame(const QString &videoPath) {
    cv::VideoCapture cap(videoPath.toStdString());
    if (!cap.isOpened()) {
        return QPixmap();
    }
    cv::Mat frame;
    if (!cap.read(frame)) {
        return QPixmap();
    }
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    return QPixmap::fromImage(qimg.copy()).scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
} 
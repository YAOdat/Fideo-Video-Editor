#pragma once
#include <QString>
#include <QPixmap>

bool isImageFile(const QString &fileName);
bool isVideoFile(const QString &fileName);
QPixmap extractVideoFrame(const QString &videoPath); 
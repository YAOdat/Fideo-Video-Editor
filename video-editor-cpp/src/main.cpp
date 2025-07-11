#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QDockWidget>
#include <QListWidget>
#include <QFrame>
#include <QPainter>
#include <QSplitter>
#include <QMimeData>
#include <QDrag>
#include "timeline/TimelineWidget.h"
#include "VideoPreviewWidget.h"
#include "utils/FileUtils.h"
#include <QObject>
#include <mlt/framework/mlt.h>
#include <QShortcut>
#include <QScrollArea>
#include "text/TextOverlayManager.h"
#include "text/TextEditDialog.h" // Added for TextEditDialog

class TimelineRuler : public QFrame {
public:
    TimelineRuler(QWidget* parent = nullptr) : QFrame(parent) {
        setMinimumHeight(30);
        setMaximumHeight(30);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.fillRect(rect(), QColor(40, 40, 40));
        p.setPen(QColor(180, 180, 180));
        int w = width();
        int h = height();
        int tickSpacing = 50;
        for (int x = 0; x < w; x += tickSpacing) {
            p.drawLine(x, h-15, x, h);
            p.drawText(x+2, h-18, QString::number(x/tickSpacing));
        }
    }
};

int main(int argc, char *argv[]) {
    // Initialize MLT globally
    mlt_factory_init(NULL);
    QApplication app(argc, argv);
    try {
        QMainWindow mainWindow;
        mainWindow.setWindowTitle("Video Editor");
        mainWindow.resize(1400, 900);
        mainWindow.setMinimumSize(900, 600);
        mainWindow.setMaximumSize(1920, 1200);
        // Dark theme
        app.setStyleSheet("QMainWindow, QMenuBar, QMenu, QToolBar, QDockWidget, QWidget { background: #232323; color: #e0e0e0; } QPushButton { background: #333; color: #fff; border: 1px solid #444; padding: 4px 12px; border-radius: 4px; } QPushButton:hover { background: #444; } QLineEdit, QTextEdit, QComboBox { background: #181818; color: #fff; border: 1px solid #444; } QLabel { color: #e0e0e0; } QFrame#TimelineArea { border-top: 2px solid #444; background: #181818; }");
        // Central widget and layout
        QWidget* centralWidget = new QWidget(&mainWindow);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        // Video preview
        VideoPreviewWidget* preview = new VideoPreviewWidget(centralWidget);
        preview->setMinimumHeight(320);
        mainLayout->addWidget(preview);
        // Timeline area
        QFrame* timelineArea = new QFrame(centralWidget);
        timelineArea->setObjectName("TimelineArea");
        timelineArea->setMinimumHeight(220);
        QVBoxLayout* timelineLayout = new QVBoxLayout(timelineArea);
        timelineLayout->setContentsMargins(0, 0, 0, 0);
        timelineLayout->setSpacing(0);
        TimelineRuler* ruler = new TimelineRuler(timelineArea);
        timelineLayout->addWidget(ruler);
        TimelineWidget* timeline = new TimelineWidget(timelineArea);
        timelineLayout->addWidget(timeline);
        mainLayout->addWidget(timelineArea);
        centralWidget->setLayout(mainLayout);
        mainWindow.setCentralWidget(centralWidget);
        // Toolbar and menu
        QMenuBar* menuBar = mainWindow.menuBar();
        QMenu* fileMenu = menuBar->addMenu("File");
        QAction* importAction = fileMenu->addAction("Import Image/Video");
        QToolBar* toolBar = new QToolBar(&mainWindow);
        toolBar->addAction(importAction);
        mainWindow.addToolBar(toolBar);
        // Text overlay manager
        TextOverlayManager* textOverlayManager = new TextOverlayManager(&mainWindow);
        // Add Text button after timeline is created
        QAction* textAction = toolBar->addAction("Text");
        QObject::connect(textAction, &QAction::triggered, &mainWindow, [timeline, textOverlayManager]() {
            // Ensure a dedicated text track exists (last track)
            int textTrackIndex = timeline->getTrackCount() - 1;
            if (textTrackIndex < 0 || timeline->getClipCount(textTrackIndex) > 0) {
                timeline->addTrack();
                textTrackIndex = timeline->getTrackCount() - 1;
            }
            // Add a default text overlay to the manager
            TextOverlay* overlay = textOverlayManager->addTextOverlay();
            // Create an MLT text producer for the timeline
            auto* producer = new Mlt::Producer(*timeline->getMultitrack()->profile(), "qml:text");
            producer->set("text", overlay->text().toUtf8().data());
            producer->set("length", 50); // 2 seconds at 25fps
            // Add the text clip to the text track
            timeline->addClip(textTrackIndex, "qml:text");
            // Optionally, store the overlay/producer association for editing later
        });
        // Connect double-click on timeline clips to text edit dialog (for text overlays)
        QObject::connect(timeline, &TimelineWidget::clipDoubleClicked, &mainWindow, [timeline, textOverlayManager](int trackIndex, int clipIndex) {
            // For now, only handle the last track (text track)
            if (trackIndex == timeline->getTrackCount() - 1) {
                auto overlays = textOverlayManager->textOverlays();
                if (clipIndex >= 0 && clipIndex < overlays.size()) {
                    TextEditDialog dlg(overlays[clipIndex]);
                    dlg.exec();
                }
            }
        });
        // Playback controls
        QToolBar* playbackBar = new QToolBar(&mainWindow);
        QPushButton* playBtn = new QPushButton("Play");
        QPushButton* pauseBtn = new QPushButton("Pause");
        QPushButton* stopBtn = new QPushButton("Stop");
        playbackBar->addWidget(playBtn);
        playbackBar->addWidget(pauseBtn);
        playbackBar->addWidget(stopBtn);
        mainWindow.addToolBar(Qt::BottomToolBarArea, playbackBar);
        // Media Bin (Dock)
        QDockWidget* mediaBinDock = new QDockWidget("Media Bin", &mainWindow);
        // Restore custom MediaBinListWidget for drag-and-drop
        class MediaBinListWidget : public QListWidget {
        public:
            using QListWidget::QListWidget;
        protected:
            void startDrag(Qt::DropActions supportedActions) override {
                QListWidgetItem* item = currentItem();
                if (!item) return;
                QMimeData* mimeData = new QMimeData;
                QString filePath = item->data(Qt::UserRole).toString();
                mimeData->setText(filePath);
                QList<QUrl> urls; urls << QUrl::fromLocalFile(filePath);
                mimeData->setUrls(urls);
                QDrag* drag = new QDrag(this);
                drag->setMimeData(mimeData);
                drag->setPixmap(item->icon().pixmap(64, 64));
                drag->exec(Qt::CopyAction);
            }
        };
        MediaBinListWidget* mediaBin = new MediaBinListWidget(mediaBinDock);
        mediaBin->setSelectionMode(QAbstractItemView::SingleSelection);
        mediaBin->setViewMode(QListView::IconMode);
        mediaBin->setIconSize(QSize(64, 64));
        mediaBin->setResizeMode(QListView::Adjust);
        mediaBin->setSpacing(8);
        mediaBinDock->setWidget(mediaBin);
        mainWindow.addDockWidget(Qt::LeftDockWidgetArea, mediaBinDock);
        // Connect signals
        QObject::connect(playBtn, &QPushButton::clicked, timeline, &TimelineWidget::play);
        QObject::connect(pauseBtn, &QPushButton::clicked, timeline, &TimelineWidget::pause);
        QObject::connect(stopBtn, &QPushButton::clicked, timeline, &TimelineWidget::stop);
        QObject::connect(timeline, &TimelineWidget::frameReady, preview, &VideoPreviewWidget::setFrame);
        // Keyboard shortcuts for playback
        QShortcut* spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), &mainWindow);
        QObject::connect(spaceShortcut, &QShortcut::activated, [&]() {
            if (timeline->isPlaying()) timeline->pause();
            else timeline->play();
        });
        QShortcut* homeShortcut = new QShortcut(QKeySequence(Qt::Key_Home), &mainWindow);
        QObject::connect(homeShortcut, &QShortcut::activated, [&]() {
            timeline->stop();
        });
        QShortcut* endShortcut = new QShortcut(QKeySequence(Qt::Key_End), &mainWindow);
        QObject::connect(endShortcut, &QShortcut::activated, [&]() {
            timeline->setPlayhead(99999); // TODO: set to last frame
            timeline->pause();
            preview->setFrame(timeline->getCurrentFrame());
        });
        // Import action
        QObject::connect(importAction, &QAction::triggered, &mainWindow, [&]() {
            QString filePath = QFileDialog::getOpenFileName(&mainWindow, "Import Image or Video", "", "Media Files (*.mp4 *.avi *.mov *.mkv *.wmv *.jpg *.jpeg *.png *.bmp *.gif)");
            if (filePath.isEmpty()) return;
            QPixmap thumb;
            if (isVideoFile(filePath)) {
                thumb = extractVideoFrame(filePath);
            } else if (isImageFile(filePath)) {
                thumb = QPixmap(filePath).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            QListWidgetItem* item = new QListWidgetItem(QIcon(thumb), QFileInfo(filePath).fileName());
            item->setToolTip(filePath);
            item->setData(Qt::UserRole, filePath);
            mediaBin->addItem(item);
        });
        // Double-click media bin to add to timeline
        QObject::connect(mediaBin, &QListWidget::itemDoubleClicked, timeline, [timeline](QListWidgetItem* item) {
            if (!item) return;
            timeline->addClip(0, item->data(Qt::UserRole).toString());
        });
        mainWindow.show();
        // Add default track ONCE after the window is shown, if none exist
        QTimer::singleShot(100, [timeline]() {
            try {
                if (timeline->getTrackCount() == 0) {
                    timeline->addTrack();
                }
            } catch (const std::exception& e) {
                qDebug() << "Exception creating default tracks:" << e.what();
            }
        });
        int result = app.exec();
        mlt_factory_close();
        return result;
    } catch (const std::exception& e) {
        qDebug() << "Exception in main:" << e.what();
        QMessageBox::critical(nullptr, "Error", QString("Failed to initialize application: %1").arg(e.what()));
        mlt_factory_close();
        return 1;
    }
}
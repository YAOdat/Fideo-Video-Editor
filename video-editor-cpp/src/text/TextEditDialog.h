#ifndef TEXTEDITDIALOG_H
#define TEXTEDITDIALOG_H

#include <QDialog>
#include "TextOverlay.h"

class QLineEdit;
class QFontComboBox;
class QSpinBox;
class QPushButton;
class QColorDialog;

class TextEditDialog : public QDialog {
    Q_OBJECT
public:
    explicit TextEditDialog(TextOverlay *overlay, QWidget *parent = nullptr);
    
private slots:
    void updateText();
    void selectColor();
    void updateFont();
    void accept() override;
    
private:
    TextOverlay *m_overlay;
    QLineEdit *m_textEdit;
    QFontComboBox *m_fontCombo;
    QSpinBox *m_fontSizeSpin;
    QPushButton *m_colorButton;
    QColorDialog *m_colorDialog;
};

#endif // TEXTEDITDIALOG_H 
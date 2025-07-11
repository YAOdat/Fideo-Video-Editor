#include "TextEditDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QLabel>

TextEditDialog::TextEditDialog(TextOverlay *overlay, QWidget *parent)
    : QDialog(parent), m_overlay(overlay) {
    setWindowTitle("Edit Text");
    setMinimumSize(400, 300);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Text input
    QFormLayout *formLayout = new QFormLayout();
    m_textEdit = new QLineEdit(m_overlay->text(), this);
    formLayout->addRow("Text:", m_textEdit);
    
    // Font selection
    m_fontCombo = new QFontComboBox(this);
    m_fontCombo->setCurrentFont(m_overlay->font());
    formLayout->addRow("Font:", m_fontCombo);
    
    // Font size
    m_fontSizeSpin = new QSpinBox(this);
    m_fontSizeSpin->setRange(8, 144);
    m_fontSizeSpin->setValue(m_overlay->font().pointSize());
    formLayout->addRow("Size:", m_fontSizeSpin);
    
    // Color selection
    m_colorButton = new QPushButton("Select Color", this);
    m_colorButton->setStyleSheet(QString("background-color: %1;").arg(m_overlay->color().name()));
    m_colorDialog = new QColorDialog(m_overlay->color(), this);
    formLayout->addRow("Color:", m_colorButton);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connections
    connect(m_textEdit, &QLineEdit::textChanged, this, &TextEditDialog::updateText);
    connect(m_fontCombo, &QFontComboBox::currentFontChanged, this, &TextEditDialog::updateFont);
    connect(m_fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TextEditDialog::updateFont);
    connect(m_colorButton, &QPushButton::clicked, this, &TextEditDialog::selectColor);
    connect(okButton, &QPushButton::clicked, this, &TextEditDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void TextEditDialog::updateText() {
    m_overlay->setText(m_textEdit->text());
}

void TextEditDialog::selectColor() {
    if (m_colorDialog->exec() == QDialog::Accepted) {
        QColor color = m_colorDialog->currentColor();
        m_overlay->setColor(color);
        m_colorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
    }
}

void TextEditDialog::updateFont() {
    QFont font = m_fontCombo->currentFont();
    font.setPointSize(m_fontSizeSpin->value());
    m_overlay->setFont(font);
}

void TextEditDialog::accept() {
    updateText();
    updateFont();
    QDialog::accept();
} 
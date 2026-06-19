/**
 * @file SettingsDialog.cpp
 * @brief 文件管理器设置对话框实现
 */

#include "SettingsDialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

static const char *kDlgStyle = R"(
QDialog {
    background-color: #1e1e1e;
}
QLabel {
    color: #999;
    font-size: 12px;
}
QCheckBox {
    color: #ccc;
    font-size: 12px;
    spacing: 8px;
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    background-color: #252525;
    border: 1px solid #444;
}
QCheckBox::indicator:checked {
    background-color: #555;
    border: 1px solid #888;
}
QPushButton {
    background-color: #333;
    color: #ccc;
    border: none;
    padding: 6px 18px;
    font-size: 12px;
}
QPushButton:hover {
    background-color: #3a3a3a;
    color: #fff;
}
)";

namespace FileManager {

SettingsDialog::SettingsDialog(bool showIcons, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("文件管理器 — 设置");
    setFixedSize(320, 130);
    setStyleSheet(kDlgStyle);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    auto *hint = new QLabel("调整以下选项以减少性能开销：");
    layout->addWidget(hint);

    m_checkIcons = new QCheckBox("显示文件图标");
    m_checkIcons->setChecked(showIcons);
    layout->addWidget(m_checkIcons);

    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

} // namespace FileManager

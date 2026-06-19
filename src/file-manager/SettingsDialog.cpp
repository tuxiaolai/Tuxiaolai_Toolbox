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
QRadioButton {
    color: #ccc;
    font-size: 12px;
    spacing: 8px;
    padding: 2px 0;
}
QRadioButton::indicator {
    width: 14px;
    height: 14px;
    border-radius: 7px;
    background-color: #252525;
    border: 1px solid #444;
}
QRadioButton::indicator:checked {
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

SettingsDialog::SettingsDialog(int currentMode, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("文件管理器 — 设置");
    setFixedSize(320, 180);
    setStyleSheet(kDlgStyle);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(10);

    auto *hint = new QLabel("图标显示模式：");
    layout->addWidget(hint);

    m_group = new QButtonGroup(this);

    m_radioReal   = new QRadioButton("真图标 — 使用系统 Shell 图标");
    m_radioPreset = new QRadioButton("预设图标 — 按文件类型缓存（推荐）");
    m_radioNone   = new QRadioButton("无图标 — 仅文字显示");

    m_group->addButton(m_radioReal,   IconReal);
    m_group->addButton(m_radioPreset, IconPreset);
    m_group->addButton(m_radioNone,   IconNone);

    // 设置当前选中项
    switch (currentMode) {
    case IconReal:   m_radioReal->setChecked(true);   break;
    case IconPreset: m_radioPreset->setChecked(true); break;
    default:         m_radioNone->setChecked(true);   break;
    }

    layout->addWidget(m_radioReal);
    layout->addWidget(m_radioPreset);
    layout->addWidget(m_radioNone);

    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

int SettingsDialog::iconMode() const
{
    return m_group->checkedId();
}

} // namespace FileManager

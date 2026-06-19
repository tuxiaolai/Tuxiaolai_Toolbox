/**
 * @file SettingsDialog.cpp
 * @brief 文件管理器设置对话框实现
 */

#include "SettingsDialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFrame>

static const char *kDlgStyle = R"(
QDialog {
    background-color: #1e1e1e;
}
QLabel {
    color: #999;
    font-size: 12px;
}
QRadioButton, QCheckBox {
    color: #ccc;
    font-size: 12px;
    spacing: 8px;
    padding: 2px 0;
}
QRadioButton::indicator, QCheckBox::indicator {
    width: 14px;
    height: 14px;
    border-radius: 7px;
    background-color: #252525;
    border: 1px solid #444;
}
QRadioButton::indicator:checked, QCheckBox::indicator:checked {
    background-color: #555;
    border: 1px solid #888;
}
QCheckBox::indicator {
    border-radius: 2px;
}
QFrame#separator {
    height: 1px;
    background-color: #333;
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

SettingsDialog::SettingsDialog(int currentMode, bool statusBarVisible,
                               bool deleteToTrash,
                               bool showSizeCol, bool showTypeCol, bool showDateCol,
                               int scrollSpeed,
                               QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("文件管理器 — 设置");
    setFixedSize(360, 470);
    setStyleSheet(kDlgStyle);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);

    // ── 图标模式 ──
    layout->addWidget(new QLabel("图标显示模式："));

    m_group = new QButtonGroup(this);

    m_radioReal   = new QRadioButton("真图标 — 使用系统 Shell 图标");
    m_radioPreset = new QRadioButton("预设图标 — 按文件类型缓存（推荐）");
    m_radioNone   = new QRadioButton("无图标 — 仅文字显示");

    m_group->addButton(m_radioReal,   IconReal);
    m_group->addButton(m_radioPreset, IconPreset);
    m_group->addButton(m_radioNone,   IconNone);

    switch (currentMode) {
    case IconReal:   m_radioReal->setChecked(true);   break;
    case IconPreset: m_radioPreset->setChecked(true); break;
    default:         m_radioNone->setChecked(true);   break;
    }

    layout->addWidget(m_radioReal);
    layout->addWidget(m_radioPreset);
    layout->addWidget(m_radioNone);

    // ── 分隔线 ──
    auto *sep = new QFrame();
    sep->setObjectName("separator");
    sep->setFrameShape(QFrame::HLine);
    layout->addSpacing(4);
    layout->addWidget(sep);
    layout->addSpacing(4);

    // ── 状态栏 / 删除模式 ──
    m_chkStatusBar = new QCheckBox("显示状态栏");
    m_chkStatusBar->setChecked(statusBarVisible);
    layout->addWidget(m_chkStatusBar);

    m_chkDeleteToTrash = new QCheckBox("删除时移到回收站");
    m_chkDeleteToTrash->setChecked(deleteToTrash);
    layout->addWidget(m_chkDeleteToTrash);

    // ── 分隔线 ──
    auto *sep2 = new QFrame();
    sep2->setObjectName("separator");
    sep2->setFrameShape(QFrame::HLine);
    layout->addSpacing(4);
    layout->addWidget(sep2);
    layout->addSpacing(4);

    // ── 表头列显隐 ──
    layout->addWidget(new QLabel("树视图显示列："));

    m_chkSizeCol = new QCheckBox("显示「大小」列");
    m_chkSizeCol->setChecked(showSizeCol);
    layout->addWidget(m_chkSizeCol);

    m_chkTypeCol = new QCheckBox("显示「类型」列");
    m_chkTypeCol->setChecked(showTypeCol);
    layout->addWidget(m_chkTypeCol);

    m_chkDateCol = new QCheckBox("显示「修改日期」列");
    m_chkDateCol->setChecked(showDateCol);
    layout->addWidget(m_chkDateCol);

    // ── 分隔线 ──
    auto *sep3 = new QFrame();
    sep3->setObjectName("separator");
    sep3->setFrameShape(QFrame::HLine);
    layout->addSpacing(4);
    layout->addWidget(sep3);
    layout->addSpacing(4);

    // ── 滚动步长 ──
    layout->addWidget(new QLabel("滚轮每格滚动距离（像素）："));

    m_sliderScroll = new QSlider(Qt::Horizontal);
    m_sliderScroll->setRange(20, 200);
    m_sliderScroll->setValue(scrollSpeed);
    m_sliderScroll->setTickPosition(QSlider::TicksBelow);
    m_sliderScroll->setTickInterval(20);
    layout->addWidget(m_sliderScroll);

    m_lblScrollVal = new QLabel(QString::number(scrollSpeed) + " px");
    m_lblScrollVal->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_lblScrollVal);

    // 滑块值变化时更新标签
    connect(m_sliderScroll, &QSlider::valueChanged, this, [this](int v) {
        m_lblScrollVal->setText(QString::number(v) + " px");
    });

    // ── 按钮 ──
    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addSpacing(4);
    layout->addWidget(buttonBox);
}

int SettingsDialog::iconMode() const
{
    return m_group->checkedId();
}

bool SettingsDialog::statusBarVisible() const
{
    return m_chkStatusBar->isChecked();
}

bool SettingsDialog::deleteToTrash() const
{
    return m_chkDeleteToTrash->isChecked();
}

bool SettingsDialog::showSizeColumn() const
{
    return m_chkSizeCol->isChecked();
}

bool SettingsDialog::showTypeColumn() const
{
    return m_chkTypeCol->isChecked();
}

bool SettingsDialog::showDateColumn() const
{
    return m_chkDateCol->isChecked();
}

int SettingsDialog::scrollSpeed() const
{
    return m_sliderScroll->value();
}

} // namespace FileManager

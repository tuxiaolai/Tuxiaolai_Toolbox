/**
 * @file SettingsDialog.h
 * @brief 文件管理器设置对话框
 */

#ifndef FILEMANAGER_SETTINGSDIALOG_H
#define FILEMANAGER_SETTINGSDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QRadioButton>

namespace FileManager {

/** 图标显示模式 */
enum IconMode {
    IconReal    = 0,   // 真图标（系统 Shell 提取）
    IconPreset  = 1,   // 预设图标（按后缀名缓存）
    IconNone    = 2,   // 无图标
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(int currentMode, QWidget *parent = nullptr);

    int iconMode() const;

private:
    QButtonGroup *m_group;
    QRadioButton *m_radioReal;
    QRadioButton *m_radioPreset;
    QRadioButton *m_radioNone;
};

} // namespace FileManager

#endif

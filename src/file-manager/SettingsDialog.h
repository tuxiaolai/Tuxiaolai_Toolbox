/**
 * @file SettingsDialog.h
 * @brief 文件管理器设置对话框
 */

#ifndef FILEMANAGER_SETTINGSDIALOG_H
#define FILEMANAGER_SETTINGSDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>

namespace FileManager {

/** 图标显示模式 */
enum IconMode {
    IconReal    = 0,
    IconPreset  = 1,
    IconNone    = 2,
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(int currentMode, bool statusBarVisible,
                            bool deleteToTrash,
                            QWidget *parent = nullptr);

    int  iconMode()        const;
    bool statusBarVisible() const;
    bool deleteToTrash()    const;

private:
    QButtonGroup *m_group;
    QRadioButton *m_radioReal;
    QRadioButton *m_radioPreset;
    QRadioButton *m_radioNone;
    QCheckBox    *m_chkStatusBar;
    QCheckBox    *m_chkDeleteToTrash;
};

} // namespace FileManager

#endif

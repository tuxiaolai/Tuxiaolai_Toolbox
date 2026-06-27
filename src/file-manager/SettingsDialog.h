/**
 * @file SettingsDialog.h
 * @brief 文件管理器设置对话框
 *
 * 提供以下设置项：
 *   - 图标显示模式（系统图标 / 预设SVG图标 / 无图标）
 *   - 状态栏显示开关
 *   - 删除时移入回收站（代替永久删除）
 *   - 列显隐（大小 / 类型 / 修改日期）
 *   - 滚动速度调节（滑块 20~200px/步）
 */

#ifndef FILEMANAGER_SETTINGSDIALOG_H
#define FILEMANAGER_SETTINGSDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>

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
                            bool showSizeCol, bool showTypeCol, bool showDateCol,
                            int scrollSpeed,
                            QWidget *parent = nullptr);

    int  iconMode()        const;
    bool statusBarVisible() const;
    bool deleteToTrash()    const;
    bool showSizeColumn()   const;
    bool showTypeColumn()   const;
    bool showDateColumn()   const;
    int  scrollSpeed()      const;

private:
    QButtonGroup *m_group;
    QRadioButton *m_radioReal;
    QRadioButton *m_radioPreset;
    QRadioButton *m_radioNone;
    QCheckBox    *m_chkStatusBar;
    QCheckBox    *m_chkDeleteToTrash;
    QCheckBox    *m_chkSizeCol;
    QCheckBox    *m_chkTypeCol;
    QCheckBox    *m_chkDateCol;
    QSlider      *m_sliderScroll;
    QLabel       *m_lblScrollVal;
};

} // namespace FileManager

#endif

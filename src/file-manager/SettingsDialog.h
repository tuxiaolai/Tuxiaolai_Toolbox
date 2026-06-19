/**
 * @file SettingsDialog.h
 * @brief 文件管理器设置对话框
 */

#ifndef FILEMANAGER_SETTINGSDIALOG_H
#define FILEMANAGER_SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>

namespace FileManager {

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(bool showIcons, QWidget *parent = nullptr);

    bool showIcons() const { return m_checkIcons->isChecked(); }

private:
    QCheckBox *m_checkIcons;
};

} // namespace FileManager

#endif

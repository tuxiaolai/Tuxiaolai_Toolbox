/**
 * @file BrowserWidget.h
 * @brief Tu 浏览器 — 基于 QWebEngineView 的标签页浏览器控件
 *
 * 可嵌入主窗口或独立运行。功能：
 *   - 多标签页浏览（QTabWidget + createWindow 拦截）
 *   - 导航栏（地址栏 / 后退 / 前进 / 刷新）
 *   - Cookie 持久化
 *   - 文件下载管理
 *   - 开发者工具（F12）
 */

#ifndef TU_BROWSER_WIDGET_H
#define TU_BROWSER_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QWebEngineDownloadRequest>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineView>

// ============================================================
// TuBrowserPage — 自定义 WebEnginePage，拦截 createWindow
// ============================================================
class BrowserTabWidget; // 前向声明

class TuBrowserPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit TuBrowserPage(QWebEngineProfile *profile,
                           BrowserTabWidget *tabWidget,
                           QObject *parent = nullptr);

protected:
    QWebEnginePage *createWindow(WebWindowType type) override;

private:
    BrowserTabWidget *m_tabWidget;
};

// ============================================================
// BrowserTabWidget — 标签页容器
// ============================================================
class BrowserTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit BrowserTabWidget(QWidget *parent = nullptr);

    QWebEngineView *currentView() const;
    QWebEngineView *addNewTab(const QUrl &url = QUrl("about:blank"));
    void closeTab(int idx);

signals:
    void urlChanged(const QUrl &url);
    void titleChanged(const QString &title);
    void viewCreated(QWebEngineView *view);

private slots:
    void onTabCloseRequested(int idx);
};

// ============================================================
// BrowserWidget — 完整浏览器控件（导航栏 + 标签页）
// ============================================================
class BrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BrowserWidget(QWidget *parent = nullptr);

    QWebEngineView *currentView() const { return m_tabs->currentView(); }
    BrowserTabWidget *tabWidget() const { return m_tabs; }

public slots:
    void navigateToUrl(const QString &urlStr);
    void addBlankTab();
    void openInExternalBrowser();

private slots:
    void onUrlEntered();
    void onUrlChanged(const QUrl &url);
    void onTitleChanged(const QString &title);
    void onDownloadRequested(QWebEngineDownloadRequest *download);

private:
    void setupUI();
    void setupProfile();
    void connectSignals();

    // 导航栏
    QPushButton  *m_btnBack;
    QPushButton  *m_btnForward;
    QPushButton  *m_btnRefresh;
    QPushButton  *m_btnExternal;
    QLineEdit    *m_urlBar;

    // 标签页
    BrowserTabWidget *m_tabs;

    // 持久化 profile
    QWebEngineProfile *m_profile;
};

#endif // TU_BROWSER_WIDGET_H

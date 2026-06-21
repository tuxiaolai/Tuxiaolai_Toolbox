/**
 * @file BrowserWidget.cpp
 * @brief Tu 浏览器实现
 */

#include "BrowserWidget.h"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMenu>
#include <QStandardPaths>
#include <QTabBar>
#include <QVBoxLayout>

// ═══════════════════════════════════════════════════════════
// TuBrowserPage
// ═══════════════════════════════════════════════════════════

TuBrowserPage::TuBrowserPage(QWebEngineProfile *profile,
                             BrowserTabWidget *tabWidget,
                             QObject *parent)
    : QWebEnginePage(profile, parent)
    , m_tabWidget(tabWidget)
{
}

QWebEnginePage *TuBrowserPage::createWindow(WebWindowType /*type*/)
{
    // target="_blank" / window.open → 在新标签页打开
    auto *newView = m_tabWidget->addNewTab();
    m_tabWidget->setCurrentWidget(newView);
    return newView->page();
}

// ═══════════════════════════════════════════════════════════
// BrowserTabWidget
// ═══════════════════════════════════════════════════════════

BrowserTabWidget::BrowserTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);
    setDocumentMode(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTabWidget::tabCloseRequested,
            this, &BrowserTabWidget::onTabCloseRequested);

    // 右键菜单 → 新建标签
    connect(this, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
        // 在标签栏空白处也可以右键新建
        QMenu menu;
        menu.addAction("新建标签页", this, [this]() { addNewTab(); });
        menu.exec(tabBar()->mapToGlobal(pos));
    });

    addNewTab(QUrl("https://www.baidu.com"));
}

QWebEngineView *BrowserTabWidget::currentView() const
{
    return qobject_cast<QWebEngineView *>(currentWidget());
}

QWebEngineView *BrowserTabWidget::addNewTab(const QUrl &url)
{
    auto *view = new QWebEngineView;

    // 使用 TuBrowserPage 替代默认 QWebEnginePage，以便拦截 createWindow
    auto *page = new TuBrowserPage(view->page()->profile(), this, view);
    view->setPage(page);
    view->setUrl(url);

    int idx = addTab(view, "加载中...");
    setCurrentIndex(idx);

    // 标题变化 → 标签文字
    connect(view, &QWebEngineView::titleChanged, this, [this, view](const QString &t) {
        int i = indexOf(view);
        if (i >= 0) {
            QString text = t.isEmpty() ? "空白页" : t;
            if (text.length() > 20) text = text.left(20) + "...";
            setTabText(i, text);
            setTabToolTip(i, t);
        }
    });

    // URL 变化 → 通知外部
    connect(view, &QWebEngineView::urlChanged, this, &BrowserTabWidget::urlChanged);

    // 图标变化 → 标签图标
    connect(view, &QWebEngineView::iconChanged, this, [this, view](const QIcon &icon) {
        int i = indexOf(view);
        if (i >= 0) setTabIcon(i, icon);
    });

    emit viewCreated(view);
    return view;
}

void BrowserTabWidget::closeTab(int idx)
{
    QWidget *w = widget(idx);
    removeTab(idx);
    w->deleteLater();

    // 至少保留一个标签
    if (count() == 0)
        addNewTab(QUrl("about:blank"));
}

void BrowserTabWidget::onTabCloseRequested(int idx)
{
    closeTab(idx);
}

// ═══════════════════════════════════════════════════════════
// BrowserWidget
// ═══════════════════════════════════════════════════════════

BrowserWidget::BrowserWidget(QWidget *parent)
    : QWidget(parent)
{
    setupProfile();
    setupUI();
    connectSignals();
}

void BrowserWidget::setupProfile()
{
    m_profile = QWebEngineProfile::defaultProfile();
    m_profile->setPersistentCookiesPolicy(
        QWebEngineProfile::ForcePersistentCookies);

    QString dataPath = QApplication::applicationDirPath() + "/browser_data";
    m_profile->setPersistentStoragePath(dataPath);
    m_profile->setCachePath(dataPath + "/cache");
    m_profile->setHttpUserAgent(
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/125.0.0.0 Safari/537.36 TuBrowser/1.0");
}

void BrowserWidget::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── 导航栏 ──
    auto *navBar = new QWidget;
    navBar->setObjectName("browserNavBar");
    navBar->setFixedHeight(36);
    navBar->setStyleSheet(
        "#browserNavBar { background-color: #2d2d2d; border-bottom: 1px solid #3a3a3a; }"
        "QPushButton { background: transparent; color: #ccc; border: 1px solid transparent; "
        "  border-radius: 3px; padding: 2px 8px; font-size: 14px; }"
        "QPushButton:hover { background: #3a3a3a; border-color: #555; }"
        "QPushButton:disabled { color: #555; }"
        "QLineEdit { background: #1e1e1e; color: #ddd; border: 1px solid #3a3a3a; "
        "  border-radius: 4px; padding: 3px 8px; font-size: 13px; }");

    auto *navLayout = new QHBoxLayout(navBar);
    navLayout->setContentsMargins(4, 4, 4, 4);
    navLayout->setSpacing(2);

    m_btnBack = new QPushButton("◀");
    m_btnBack->setToolTip("后退");
    m_btnBack->setFixedWidth(30);
    navLayout->addWidget(m_btnBack);

    m_btnForward = new QPushButton("▶");
    m_btnForward->setToolTip("前进");
    m_btnForward->setFixedWidth(30);
    navLayout->addWidget(m_btnForward);

    m_btnRefresh = new QPushButton("↻");
    m_btnRefresh->setToolTip("刷新");
    m_btnRefresh->setFixedWidth(30);
    navLayout->addWidget(m_btnRefresh);

    m_urlBar = new QLineEdit;
    m_urlBar->setPlaceholderText("输入网址或搜索...");
    navLayout->addWidget(m_urlBar, 1);

    mainLayout->addWidget(navBar);

    // ── 标签页 ──
    m_tabs = new BrowserTabWidget;
    mainLayout->addWidget(m_tabs, 1);
}

void BrowserWidget::connectSignals()
{
    // 地址栏回车 → 导航
    connect(m_urlBar, &QLineEdit::returnPressed,
            this, &BrowserWidget::onUrlEntered);

    // 按钮 → 导航操作
    connect(m_btnBack, &QPushButton::clicked, [this]() {
        if (auto *v = m_tabs->currentView()) v->back();
    });
    connect(m_btnForward, &QPushButton::clicked, [this]() {
        if (auto *v = m_tabs->currentView()) v->forward();
    });
    connect(m_btnRefresh, &QPushButton::clicked, [this]() {
        if (auto *v = m_tabs->currentView()) v->reload();
    });

    // 标签页切换 → 同步地址栏和按钮状态
    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int /*idx*/) {
        if (auto *v = m_tabs->currentView()) {
            m_urlBar->setText(v->url().toString());
            m_btnBack->setEnabled(
                v->page()->action(QWebEnginePage::Back)->isEnabled());
            m_btnForward->setEnabled(
                v->page()->action(QWebEnginePage::Forward)->isEnabled());
        }
    });

    // 标签页 URL 变化 → 同步地址栏
    connect(m_tabs, &BrowserTabWidget::urlChanged,
            this, &BrowserWidget::onUrlChanged);

    // 下载管理 → QWebEngineProfile::downloadRequested (Qt6)
    connect(m_profile, &QWebEngineProfile::downloadRequested,
            this, &BrowserWidget::onDownloadRequested);
}

// ── Slots ──

void BrowserWidget::navigateToUrl(const QString &urlStr)
{
    QString url = urlStr.trimmed();
    if (url.isEmpty()) return;

    if (!url.startsWith("http://") && !url.startsWith("https://")) {
        if (url.contains('.')) {
            url = "https://" + url;
        } else {
            url = "https://www.baidu.com/s?wd=" + url;
        }
    }

    if (auto *v = m_tabs->currentView())
        v->setUrl(QUrl(url));
}

void BrowserWidget::addBlankTab()
{
    m_tabs->addNewTab(QUrl("about:blank"));
}

void BrowserWidget::onUrlEntered()
{
    navigateToUrl(m_urlBar->text());
}

void BrowserWidget::onUrlChanged(const QUrl &url)
{
    if (auto *v = m_tabs->currentView()) {
        if (v->url() == url) {
            m_urlBar->setText(url.toString());
        }
    }

    if (auto *v = m_tabs->currentView()) {
        auto *page = v->page();
        m_btnBack->setEnabled(
            page->action(QWebEnginePage::Back)->isEnabled());
        m_btnForward->setEnabled(
            page->action(QWebEnginePage::Forward)->isEnabled());
    }
}

void BrowserWidget::onTitleChanged(const QString &title)
{
    Q_UNUSED(title);
}

void BrowserWidget::onDownloadRequested(QWebEngineDownloadRequest *download)
{
    QString defaultPath = QStandardPaths::writableLocation(
        QStandardPaths::DownloadLocation)
        + "/" + download->downloadFileName();

    QString path = QFileDialog::getSaveFileName(
        this, "保存文件", defaultPath);

    if (!path.isEmpty()) {
        download->setDownloadDirectory(QFileInfo(path).absolutePath());
        download->setDownloadFileName(QFileInfo(path).fileName());
        download->accept();
    } else {
        download->cancel();
    }
}

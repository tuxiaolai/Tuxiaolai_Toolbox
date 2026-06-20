/**
 * @file ToolboxWindow.cpp
 * @brief 兔小赖工具箱主窗口实现
 *
 * 布局：
 *   [Activity Bar (50px)] [Side Panel (200px)] [Content Area]
 *   活动栏可拖动移动窗口，侧边栏不可拖拽。
 */

#include "ToolboxWindow.h"
#include "../file-manager/MainWindow.h"
#include <QApplication>
#include <QLabel>

static const char *kStyle = R"(
QMainWindow { background-color: #1e1e1e; }
QWidget#activityBar { background-color: #252525; border-right: 1px solid #333; }
QPushButton#actBtn {
    background: transparent; color: #888; border: none;
    font-size: 18px; min-width: 48px; min-height: 40px;
    border-left: 2px solid transparent;
}
QPushButton#actBtn:hover { background-color: #2a2a2a; color: #bbb; }
QPushButton#actBtn:checked { color: #fff; border-left: 2px solid #89b4fa; background-color: #2a2a2a; }
QWidget#contentArea { background-color: #1e1e1e; }
QLabel#placeholderLabel { color: #444; font-size: 18px; }
)";

ToolboxWindow::ToolboxWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("兔小赖工具箱");
    resize(1100, 700);
    setStyleSheet(kStyle);
    setupUI();
}

void ToolboxWindow::setupUI()
{
    auto *w = new QWidget();
    setCentralWidget(w);
    auto *lay = new QHBoxLayout(w);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);

    // ── 活动栏 50px ──
    m_activityBar = new QWidget();
    m_activityBar->setObjectName("activityBar");
    m_activityBar->setFixedWidth(50);
    auto *aLay = new QVBoxLayout(m_activityBar);
    aLay->setContentsMargins(0,8,0,8);
    aLay->setSpacing(2);

    m_btnFileMgr = new QPushButton();
    m_btnFileMgr->setIcon(QIcon(":/icons/folder_dark.svg"));
    m_btnFileMgr->setIconSize(QSize(20,20));
    m_btnFileMgr->setObjectName("actBtn");
    m_btnFileMgr->setToolTip("文件管理器");
    m_btnFileMgr->setCursor(Qt::PointingHandCursor);
    m_btnFileMgr->setCheckable(true);
    m_btnFileMgr->setChecked(m_sideVisible);
    aLay->addWidget(m_btnFileMgr);
    aLay->addStretch();
    lay->addWidget(m_activityBar);
    m_activityBar->installEventFilter(this);

    // ── 侧边栏 200px（固定不可拖拽）──
    m_sidePanel = new QWidget();
    m_sidePanel->setFixedWidth(200);
    m_sidePanel->setVisible(m_sideVisible);
    auto *sLay = new QVBoxLayout(m_sidePanel);
    sLay->setContentsMargins(0,0,0,0);
    sLay->setSpacing(0);
    m_fileManager = new FileManager::MainWindow(m_sidePanel);
    m_fileManager->setWindowFlags(Qt::Widget);
    m_fileManager->setMinimumSize(0,0);
    sLay->addWidget(m_fileManager,1);
    lay->addWidget(m_sidePanel);

    // ── 内容区 ──
    m_contentArea = new QWidget();
    m_contentArea->setObjectName("contentArea");
    auto *cLay = new QVBoxLayout(m_contentArea);
    cLay->setAlignment(Qt::AlignCenter);
    auto *ph = new QLabel("工具内容区\n（待集成）");
    ph->setObjectName("placeholderLabel");
    ph->setAlignment(Qt::AlignCenter);
    cLay->addWidget(ph);
    lay->addWidget(m_contentArea, 1);

    // ── 信号 ──
    connect(m_btnFileMgr, &QPushButton::clicked, this, [this](bool checked) {
        m_sideVisible = checked;
        m_sidePanel->setVisible(checked);
    });
}

bool ToolboxWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_activityBar) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                m_dragStartPos = me->globalPosition().toPoint() - frameGeometry().topLeft();
                return true;
            }
        }
        if (event->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (me->buttons() & Qt::LeftButton) {
                move(me->globalPosition().toPoint() - m_dragStartPos);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

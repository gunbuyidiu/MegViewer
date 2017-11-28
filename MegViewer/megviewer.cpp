#include "megviewer.h"
#include "mainWidget.h"
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QFileDialog>

MegViewer::MegViewer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    initControls();
}

MegViewer::~MegViewer()
{

}

void MegViewer::initControls()
{
    setCentralWidget(new MainWidget());
    QMenuBar* pBar = ui.menuBar;
    QMenu* pMenu = new QMenu(QStringLiteral("文件"));
    QAction* pFileAct = new QAction(QStringLiteral("打开"));
    pMenu->addAction(pFileAct);
    pBar->addMenu(pMenu);
    connect(pFileAct, &QAction::triggered, this, &MegViewer::slotOnFileActTrigger);
}

void MegViewer::loadImg(const QString& strPath)
{
    MainWidget* pCentral = dynamic_cast<MainWidget*>(centralWidget());
    if (pCentral)
    {
        pCentral->loadImg(strPath);
    }
}

void MegViewer::slotOnFileActTrigger()
{
    QString strPath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".",
        tr("Image Files(*.jpg *.png *.tif)"));
    if (!strPath.isEmpty())
    {
        loadImg(strPath);
    }
}

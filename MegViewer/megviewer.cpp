#include "megviewer.h"
#include "mainWidget.h"
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <QImage>
#include <QWheelEvent>
#include <QLabel>
#include "tiffio.h"

static const double sc_declination = 0.00001;
static const double sc_dScaleFactor = 1.2;
static const double sc_dScaleMax = 40.0;
static const double sc_dScaleMin = 1.0;

MegViewer::MegViewer(QWidget *parent)
    : QMainWindow(parent), m_pLabel(new QLabel()), m_dScale(1.0)
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
    resize(800, 600);
}

bool MegViewer::loadImg(const QString& strPath)
{
    MainWidget* pWidget = dynamic_cast<MainWidget*>(centralWidget());
    if (pWidget)
    {
        return pWidget->setImage(strPath);
    }
    return false;

    /************************************************************************/
    /*                                                                      */
    /************************************************************************/
    if (m_pLabel)
    {
        m_pImage = QImage(strPath);
        if (m_pImage.isNull())
        {
        }
        else 
        {
            m_pLabel->setPixmap(QPixmap::fromImage(m_pImage));
        }

        TIFF* pTif;
        int width = 0;
        if ((pTif = TIFFOpen(strPath.toStdString().c_str(), "r")) != NULL) {
            printf("success!\n");
            TIFFGetField(pTif, TIFFTAG_IMAGEWIDTH, &width);
        }
        resize(800, 600);
        m_dScale = 1.0;
        setWindowFilePath(strPath);
        const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(strPath)).arg(m_pImage.width()).arg(m_pImage.height()).arg(m_pImage.depth());
        statusBar()->showMessage(QString("tif width: %1 ").arg(width));
        return true;
    }

    return false;
}

void MegViewer::slotOnFileActTrigger()
{
    QString strPath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".",
        tr("Image Files(*.jpg *.png *.tif *.bmp)"));
    if (!strPath.isEmpty())
    {
        loadImg(strPath);
    }
}

void MegViewer::wheelEvent(QWheelEvent *event)
{
    return QMainWindow::wheelEvent(event);

    /************************************************************************/
    /*                                                                      */
    /************************************************************************/
    QPoint pos, posGlobal;
    pos = event->globalPos();
    posGlobal = mapToGlobal(this->pos());

    //判断鼠标位置是否在图像显示区域
    if (pos.x() > posGlobal.x() && pos.x() < posGlobal.x() + this->width()
        && pos.y() > posGlobal.y() && pos.y() < posGlobal.y() + this->height())
    {
        // 当滚轮远离使用者时进行放大，当滚轮向使用者方向旋转时进行缩小
        if (event->delta() > 0)
        {
            zoom(true);
        }
        else if (event->delta() < 0)
        {
            zoom(false);
        }
    }
}

bool MegViewer::eventFilter(QObject *watched, QEvent *event)
{
    return QMainWindow::eventFilter(watched, event);
}

void MegViewer::scale(double dScale)
{
    if (m_pLabel)
    {
        m_pLabel->resize(dScale * m_pLabel->pixmap()->size());
    }
}

void MegViewer::zoom(bool bIn)
{
    if (bIn)
    {
        m_dScale /= sc_dScaleFactor;
        if (m_dScale - sc_dScaleMin <= sc_declination)
        {
            m_dScale = sc_dScaleMin;
        }
    }
    else
    {
        m_dScale *= sc_dScaleFactor;
        if (m_dScale - sc_dScaleMax >= sc_declination)
        {
            m_dScale = sc_dScaleMax;
        }
    }

    scale(m_dScale);
}

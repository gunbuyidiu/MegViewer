
#include "mainWidget.h"
#include <QPainter>
#include <QImage>
#include <QWheelEvent>
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QScrollBar>

static const double sc_declination = 0.00001;
static const double sc_dScaleFactor = 1.2;
static const double sc_dScaleMax = 40.0;
static const double sc_dScaleMin = 1.0;

static void rectKeepAspectRatio(QRect& oShow, const QRect& osrc)
{
    if (oShow.height() == 0 || oShow.width() == 0 || osrc.height() == 0 || osrc.width() == 0)
    {
        return;
    }
    double oRatio = (double)osrc.width() / osrc.height();

    if (oRatio * oShow.height() - oShow.width() >= sc_declination)
    {
        oShow.setHeight(oShow.width() / oRatio);
    }
    else
    {
        oShow.setWidth(oShow.height()*oRatio);
    }
}

MainWidget::MainWidget(QWidget* parent):m_pImage(new QImage()), m_pLabel(new QLabel()), m_dScale(1.0)
{
    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(m_pLabel);
    this->setLayout(pHLayout);
    m_pLabel->installEventFilter(this);
}

MainWidget::~MainWidget()
{
}

void MainWidget::loadImg(const QString& path)
{
    if (m_pImage && m_pLabel)
    {
        m_pImage->load(path);
        m_pLabel->setPixmap(QPixmap::fromImage(*m_pImage));
        m_dScale = 1.0;
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void MainWidget::paintEvent(QPaintEvent *event)
{
//     if (m_pImage)
//     {
//         QPainter oPaint(this);
//         QRect oRect(this->rect());
//         rectKeepAspectRatio(oRect, m_pImage->rect());
//         oPaint.drawImage(oRect, *m_pImage);
//     }
    QWidget::paintEvent(event);
}

void MainWidget::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pLabel && event->type() == QEvent::Wheel)
    {
        QWheelEvent* pWheelEvent = dynamic_cast<QWheelEvent*>(event);
        if (pWheelEvent)
        {
            QPoint pos, posGlobal;
            pos = pWheelEvent->globalPos();
            posGlobal = mapToGlobal(this->pos());

            //判断鼠标位置是否在图像显示区域
            if (pos.x() > posGlobal.x() && pos.x() < posGlobal.x() + this->width()
                && pos.y() > posGlobal.y() && pos.y() < posGlobal.y() + this->height())
            {
                // 当滚轮远离使用者时进行放大，当滚轮向使用者方向旋转时进行缩小
                if (pWheelEvent->delta() > 0)
                {
                    zoom(true);
                }
                else if(pWheelEvent->delta() < 0)
                {
                    zoom(false);
                }
                
                return true;
            }
        }
    }

    return false;
}

void MainWidget::scale(double dScale)
{
    if (m_pLabel)
    {
        m_pLabel->resize(dScale * m_pLabel->pixmap()->size());
    }
}

void MainWidget::zoom(bool bIn)
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


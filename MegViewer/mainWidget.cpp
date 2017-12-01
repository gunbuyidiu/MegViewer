
#include <QPainter>
#include <QImage>
#include <QWheelEvent>
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QComboBox>
#include <QSplitter>
#include <QSpacerItem>
#include <QDebug>
#include <QByteArray>
#include "mainWidget.h"
#include "tiffio.h"
#include <QMessageBox>

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

MainWidget::MainWidget(QWidget* parent) :QWidget(parent), m_pImgWidget(nullptr)
{
    initControls();
}

MainWidget::~MainWidget()
{
}


bool MainWidget::setImage(const QString& path)
{
    return m_pImgWidget && m_pImgWidget->setImage(path);
}

void MainWidget::initControls()
{
    QHBoxLayout* pHLayout = new QHBoxLayout();
    QVBoxLayout* pVLayout = new QVBoxLayout();
    QSplitter* pMainSplt = new QSplitter(Qt::Horizontal);
    QSplitter* pRightSplt = new QSplitter(Qt::Vertical);

    m_pImgWidget = new ImageWidget();
    pMainSplt->addWidget(m_pImgWidget);

    QWidget* pRightWidget = new QWidget();
    QVBoxLayout* pRightLayout = new QVBoxLayout();

    QWidget* pComWidget1 = new QWidget();
    QWidget* pComWidget2 = new QWidget();
    QVBoxLayout* pComLayout1 = new QVBoxLayout();
    QVBoxLayout* pComLayout2 = new QVBoxLayout();

    QComboBox* pCBoxGender = new QComboBox();
    QComboBox* pCBoxSource = new QComboBox();
    QComboBox* pCBoxFirstType = new QComboBox();
    QComboBox* pCBoxSencondType = new QComboBox();
    initCombox(pCBoxGender, EnCBShowType_Gender);
    initCombox(pCBoxSource, EnCBShowType_Source);
    initCombox(pCBoxFirstType, EnCBShowType_FirstType);
    initCombox(pCBoxSencondType, EnCBShowType_SecondType);

    pComLayout1->setMargin(0);
    pComLayout2->setMargin(0);
    pComLayout1->addWidget(pCBoxSource, 0, Qt::AlignTop);
    pComLayout1->addWidget(pCBoxGender, 0, Qt::AlignTop);
    pComLayout1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    pComLayout2->addWidget(pCBoxFirstType, 0, Qt::AlignTop);
    pComLayout2->addWidget(pCBoxSencondType, 0, Qt::AlignTop);
    pComLayout2->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    pComWidget1->setLayout(pComLayout1);
    pComWidget2->setLayout(pComLayout2);

    pRightSplt->addWidget(pComWidget1);
    pRightSplt->addWidget(pComWidget2);
    pRightSplt->setStretchFactor(0, 1);
    pRightSplt->setStretchFactor(1, 2);

    pRightLayout->addWidget(pRightSplt);
    pRightWidget->setLayout(pRightLayout);

    pMainSplt->addWidget(pRightWidget);

    pHLayout->addWidget(pMainSplt);

    //     pHLayout->setStretch(0, 5);
    //     pHLayout->setStretch(0, 1);
    this->setLayout(pHLayout);
}

void MainWidget::initCombox(QComboBox* pComBox, EnCBShowType enType)
{
    if (!pComBox)
    {
        return;
    }

    switch (enType)
    {
    case EnCBShowType_Gender:
        pComBox->addItem(QStringLiteral("男性"));
        pComBox->addItem(QStringLiteral("女性"));
        pComBox->addItem(QStringLiteral("其它"));
        break;
    case EnCBShowType_Source:
        pComBox->addItem(QStringLiteral("第一医科大学"));
        pComBox->addItem(QStringLiteral("第二医科大学"));
        pComBox->addItem(QStringLiteral("第三医科大学"));
        pComBox->addItem(QStringLiteral("其它"));
        break;
    case EnCBShowType_FirstType:
        pComBox->addItem(QStringLiteral("大病"));
        pComBox->addItem(QStringLiteral("中病"));
        pComBox->addItem(QStringLiteral("小病"));
        pComBox->addItem(QStringLiteral("其它"));
        break;
    case EnCBShowType_SecondType:
        pComBox->addItem(QStringLiteral("感冒"));
        pComBox->addItem(QStringLiteral("发烧"));
        pComBox->addItem(QStringLiteral("咳嗽"));
        pComBox->addItem(QStringLiteral("其它"));
        break;
    }
}

ImageWidget::ImageWidget(QWidget *parent) : QLabel(parent), m_bMouseMidDown(false), m_bMouseLeftDown(false)
{
    m_nCurveNum = 0;
    m_fScalex = 1.0f;
    m_fScaley = 1.0f;
    m_nXTranslate = 0;
    m_nYTranslate = 0;
    m_nMousePosX = 0;
    m_nMousePosY = 0;
    m_nXMoveTrans = 0;
    m_nYMoveTrans = 0;
    resize(600, 600);
}

ImageWidget::~ImageWidget()
{
}

bool ImageWidget::setImage(const QString& path)
{

    if (path.endsWith(".tif"))
    {
        TIFF* tiff = TIFFOpen(path.toStdString().c_str(), "r");
        if (!tiff) return false;
        int width, height;
        TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);

        //读取图像
        //注意：TIFFReadRGBAImage读取的通道顺序为：ABGR
        uint32* image;
        size_t  pixelCount = width*height;
        image = (uint32*)malloc(pixelCount * sizeof(uint32));
        TIFFReadRGBAImage(tiff, width, height, image, 1);

        //读取R通道
        //由于tiff格式的图像数据与bmp图存储方式一致，是从下到上，所以读的时候，需要从下往上读
        BYTE* RImage = new BYTE[pixelCount];    //为存放数据分配内存空间
        uint32 *rowPointerToSrc = image + (height - 1)*width;
        BYTE *rowPointerToR = RImage;
        for (int y = height - 1; y >= 0; --y)
        {
            uint32 *colPointerToSrc = rowPointerToSrc;
            BYTE *colPointerToR = rowPointerToR;
            for (int x = 0; x <= width - 1; ++x)
            {
                colPointerToR[0] = (BYTE)TIFFGetR(colPointerToSrc[0]);//获取R通道
                                                                      //TIFFGetB(colPointerToSrc[0]);//获取B通道
                                                                      //TIFFGetG(colPointerToSrc[0]);//获取G通道

                colPointerToR++;
                colPointerToSrc++;
            }
            rowPointerToSrc -= width;
            rowPointerToR += width;
        }


        m_pImg.loadFromData(QByteArray((char*)RImage, pixelCount), "bmp");
    }
    else
    {
        m_pImg.load(path);
    }

    if (!m_pImg.isNull())
    {
        this->setPixmap(QPixmap::fromImage(m_pImg.scaled(this->x(), this->y(), Qt::KeepAspectRatio)));
        update();
        return true;
    }

    return false;
}


void ImageWidget::iniActions()
{

}

void ImageWidget::wheelEvent(QWheelEvent *e)
{
    if (!m_pImg.isNull() && !m_bMouseMidDown)
    {
        int numDegrees = e->delta();
        if (numDegrees > 0)
        {
            zoomout(e->pos());
        }
        if (numDegrees < 0)
        {
            zoomin(e->pos());
        }
        drawImage();
        update();
    }
    
    QLabel::wheelEvent(e);
}

void ImageWidget::mouseMoveEvent(QMouseEvent * e)
{
    doMouseMove(e);
    QLabel::mouseMoveEvent(e);
}

void ImageWidget::mousePressEvent(QMouseEvent * e)
{
    if (!m_pImg.isNull())
    {
        if (e->button() == Qt::MidButton)
        {
            m_nMousePosX = e->x();
            m_nMousePosY = e->y();
            setCursor(Qt::SizeAllCursor);
            m_bMouseMidDown = true;
        }
        else if (e->button() == Qt::LeftButton)
        {
            m_vecLines.swap(vector<QLine>());
            m_bMouseLeftDown = true;
            m_oStartPt = e->pos();
        }
    }
    
    QLabel::mouseReleaseEvent(e);
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (!m_pImg.isNull())
    {
        if (e->button() == Qt::MidButton)
        {
            m_bMouseMidDown = false;
            setCursor(Qt::ArrowCursor);
        }
        else if (e->button() == Qt::LeftButton)
        {
            m_bMouseLeftDown = false;
            auto ret = QMessageBox::information(this, QStringLiteral("保存"), QStringLiteral("是否保存本次标注？"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (QMessageBox::Yes == ret)
            {
                CurveInfo oInfo;
                oInfo.nCurveNum = ++m_nCurveNum;
                oInfo.vecLines = m_vecLines;
                m_vecCurveInfos.push_back(oInfo);
            }
            m_vecLines.swap(vector<QLine>());
        }
    }
    
    e->accept();
}

void ImageWidget::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);

    QPainter painter(this);
    
    drawCurve(painter);
}

void ImageWidget::doMouseMove(QMouseEvent * e)
{
    if (m_pImg.isNull())
    {
        return;
    }
    if (e->buttons() == Qt::MidButton)
    {
        if (!m_bMouseMidDown)
        {
            return;
        }
        translate(e->x() - m_nMousePosX, e->y() - m_nMousePosY);
        drawImage();
        update();
    }
    else if (e->buttons() == Qt::LeftButton)
    {
        if (!m_bMouseLeftDown)
        {
            return;
        }
        
        m_vecLines.push_back(QLine(m_oStartPt, e->pos()));
        m_oStartPt = e->pos();

        update();
    }
}

void ImageWidget::drawImage()
{
    int nCopyX = -m_nXTranslate, nCopyY = -m_nYTranslate;
//     if (m_bMouseMidDown)
//     {
//         nCopyX -= m_nXMoveTrans;
//         nCopyY -= m_nYMoveTrans;
//     }

    this->setPixmap(QPixmap::fromImage(m_pImg.scaled(this->width()*m_fScalex, this->height()*m_fScaley,
        Qt::KeepAspectRatio).copy(nCopyX, nCopyY, this->width(), this->height())));
}

void ImageWidget::drawCurve(QPainter& oPainter)
{
    oPainter.setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    for each (auto oLine in m_vecLines)
    {
        //oPainter.drawLine(QLine(mapTo(this, oLine.p1()), mapTo(this, oLine.p2())));
        oPainter.drawLine(oLine);
    }
    for each (auto oInfo in m_vecCurveInfos)
    {
        for each (auto oLine in oInfo.vecLines)
        {
            //oPainter.drawLine(QLine(mapTo(this, oLine.p1()), mapTo(this, oLine.p2())));
            oPainter.drawLine(oLine);
        }
    }
}

void ImageWidget::resetPos()
{

}

void ImageWidget::zoomout(const QPoint& pos)
{
    if (m_fScalex <= 100 && m_fScaley <= 100)
    {
        m_fScalex *= 1.2;
        m_fScaley *= 1.2;

        m_nXTranslate = pos.x() - (pos.x() - m_nXTranslate) * 1.2;
        m_nYTranslate = pos.y() - (pos.y() - m_nYTranslate) * 1.2;
    }
    update();
}

void ImageWidget::zoomin(const QPoint& pos)
{
    if (m_fScalex >= 0.01&&m_fScaley >= 0.01)
    {
        m_fScalex *= 1 / 1.2;
        m_fScaley *= 1 / 1.2;

        m_nXTranslate = pos.x() - (pos.x() - m_nXTranslate) / 1.2;
        m_nYTranslate = pos.y() - (pos.y() - m_nYTranslate) / 1.2;
    }
    update();
}

void ImageWidget::translate(int x, int y)
{
    m_nXMoveTrans = x;
    m_nYMoveTrans = y;
}

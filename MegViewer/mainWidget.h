#include <QImage>  
#include <QPainter>  
#include <QWheelEvent>  
#include <QMenu>  
#include <QCursor>  
#include <QFileDialog>  
#include <QSplitter> 
#include <vector>
#include <queue>
#include <QPoint>
#include <QLine>
#include <QLabel>
using namespace std;

class QScrollArea;
class QLabel;
class QImage;
class QComboBox;

typedef struct stCurveInfo
{
    int nCurveNum;
    int nGender;
    int nSource;
    int nFirstType;
    int nSecondType;
    vector<QLine> vecLines;
} CurveInfo;

class ImageWidget : public QLabel
{
    Q_OBJECT
public:
    ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget();

    bool setImage(const QString& path);

    void iniActions();

protected:
    void wheelEvent(QWheelEvent *e);
    void mouseMoveEvent(QMouseEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent *e);

private:
    void doMouseMove(QMouseEvent * e);
    void drawImage();
    void drawCurve(QPainter& oPainter);


public slots:
    void resetPos();
    void zoomout(const QPoint& pos);
    void zoomin(const QPoint& pos);
    void translate(int x, int y);;
signals:
    void sig_condional_dia(int x, int y);
private:
    QImage m_pImg;

    float m_fScalex;
    float m_fScaley;
    int m_nXTranslate;
    int m_nYTranslate;

    int m_nXMoveTrans;
    int m_nYMoveTrans;

    int m_nMousePosX;
    int m_nMousePosY;
    bool m_bMouseMidDown;
    bool m_bMouseLeftDown;

    vector<QLine> m_vecLines;
    vector<CurveInfo> m_vecCurveInfos;
    QPoint m_oStartPt;
    int m_nCurveNum;


public:
    int widgetid;
};

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    enum EnCBShowType
    {
        EnCBShowType_Gender,
        EnCBShowType_Source,
        EnCBShowType_FirstType,
        EnCBShowType_SecondType
    };
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

    bool setImage(const QString& path);

private:
    void initControls();
    void initCombox(QComboBox* pComBox, EnCBShowType enType);

    ImageWidget* m_pImgWidget;
};
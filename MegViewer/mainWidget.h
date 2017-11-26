#include<QWidget>
#include<QString>

class MainWidget : public QWidget
{
    Q_OBJECT
    enum EnDrawType
    {
        EnDTRect,
        EnDTCurve
    };
public:
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

    void loadImg(const QString& path);
protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void paintEvent(QPaintEvent *event);

    virtual void wheelEvent(QWheelEvent *event);

private:
    QImage* pImage;
    int nDrawType;
    

};
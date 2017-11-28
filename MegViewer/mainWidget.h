#include<QWidget>
#include<QString>
#include <QSize>

class QScrollArea;
class QLabel;
class QImage;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

    void loadImg(const QString& path);
protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void paintEvent(QPaintEvent *event) override;

    virtual void wheelEvent(QWheelEvent *event) override;

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void scale(double dScale);
    void zoom(bool bIn);

private:
    QImage* m_pImage;
    QLabel* m_pLabel;
    double m_dScale;
};
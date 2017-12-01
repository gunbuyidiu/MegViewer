#ifndef MEGVIEWER_H
#define MEGVIEWER_H

#include <QtWidgets/QMainWindow>
#include "ui_megviewer.h"
#include <QImage>

class QLabel;

class MegViewer : public QMainWindow
{
    Q_OBJECT

public:
    MegViewer(QWidget *parent = 0);
    ~MegViewer();
    bool loadImg(const QString& strPath);

protected:
    void initControls();

private slots:
    void slotOnFileActTrigger();

private:
    Ui::MegViewerClass ui;
    virtual void wheelEvent(QWheelEvent *event) override;

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void scale(double dScale);
    void zoom(bool bIn);

private:
    QImage m_pImage;
    QLabel* m_pLabel;
    double m_dScale;
};

#endif // MEGVIEWER_H

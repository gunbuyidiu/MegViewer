#ifndef MEGVIEWER_H
#define MEGVIEWER_H

#include <QtWidgets/QMainWindow>
#include "ui_megviewer.h"

class MegViewer : public QMainWindow
{
    Q_OBJECT

public:
    MegViewer(QWidget *parent = 0);
    ~MegViewer();

protected:
    void initControls();
    void loadImg(const QString& strPath);

private slots:
    void slotOnFileActTrigger();

private:
    Ui::MegViewerClass ui;
};

#endif // MEGVIEWER_H

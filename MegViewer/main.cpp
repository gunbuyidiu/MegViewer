#include "megviewer.h"
#include <QtWidgets/QApplication>
#include <QCommandLineParser>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include "mainWidget.h"

/*
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget wnd;

    QTextEdit *editor1 = new QTextEdit;
    QTextEdit *editor2 = new QTextEdit;
    QTextEdit *editor3 = new QTextEdit;

    QSplitter *split1 = new QSplitter;
    QSplitter *split2 = new QSplitter;

    QVBoxLayout *layout = new QVBoxLayout;

    QWidget *container = new QWidget;
    QVBoxLayout *container_layout = new QVBoxLayout;

    split1->addWidget(editor1);
    split1->addWidget(editor2);

    container_layout->addWidget(split1);
    container->setLayout(container_layout);

    split2->setOrientation(Qt::Vertical);
    split2->addWidget(container);
    split2->addWidget(editor3);

    layout->addWidget(split2);

    wnd.setLayout(layout);

    wnd.show();

    return app.exec();


}
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument(MegViewer::tr("[file]"), MegViewer::tr("Image file to open."));
    commandLineParser.process(QCoreApplication::arguments());
    MegViewer w;
    if (!commandLineParser.positionalArguments().isEmpty()
        && !w.loadImg(commandLineParser.positionalArguments().front())) {
        return -1;
    }
    w.show();
    return a.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("fusion"));
    MainWindow w;


    const auto & styles = QStyleFactory::keys();
    for(const auto & s : styles)
    {
        qDebug() << s;
    }

    w.show();
    return a.exec();
}

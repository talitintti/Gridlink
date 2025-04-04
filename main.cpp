#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

void loadStylesheet(QApplication &app, const QString &path) {
    auto app_path = QCoreApplication::applicationDirPath();
    auto style_path = app_path + "/../.." + path;
    qDebug() << style_path;
    QFile file(style_path);
    if (file.open(QFile::ReadOnly)) {
        QString stylesheet = QLatin1String(file.readAll());
        app.setStyleSheet(stylesheet);
        file.close();
    } else {
        qWarning() << "Could not open stylesheet file. \n";
    }
}


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

    //a.setStyle(QStyleFactory::create("qt5ct-style"));

    //loadStylesheet(a, "/styles/style2.qss");

    w.show();
    return a.exec();
}

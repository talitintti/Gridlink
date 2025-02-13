#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include "libraryview.h"
#include "datahandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_listView_playlists_pressed(const QModelIndex &index);
    void on_listView_viewSelects_pressed(const QModelIndex &index);
    void onStackedWidgetCurrentChanged(int current);

private:
    Ui::MainWindow *ui;
    QStringListModel *stringListModel_buttons;
    QStringListModel *stringListModel_playlists;
    void Init_lower_toolbar(Ui::MainWindow *ui);
    void Init_upper_toolbar(Ui::MainWindow *ui);
    void stick_and_tape();

    DataHandler datahandler = DataHandler();

    LibraryView *library_view;

    QVector<QString> artist_data;
  };
#endif // MAINWINDOW_H

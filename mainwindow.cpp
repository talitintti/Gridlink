#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include "searchview.h"
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    SearchView *search_view = new SearchView(this);
    library_view = new LibraryView(this);
    QListWidget *home = new QListWidget();

    ui->stackedWidget->addWidget(home); // this is supposed to be "home" view whcih is not yet implemented
    ui->stackedWidget->addWidget(library_view);
    ui->stackedWidget->addWidget(search_view);

    QObject::connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::onStackedWidgetCurrentChanged);

    stringListModel_buttons = new QStringListModel(this);
    stringListModel_playlists = new QStringListModel(this);

    QStringList buttons;
    QStringList playlists;

    QByteArray hex_note = QByteArray::fromHex("F09F8EB5");
    QString utf_note = QString::fromUtf8(hex_note);
    QByteArray hex_house = QByteArray::fromHex("F09F8FA0");
    QString utf_house = QString::fromUtf8(hex_house);

    buttons << utf_house + " Home";
    buttons << utf_house + " Library";
    buttons << utf_house + " Search";
    buttons << utf_house + " Discover";

    for (int i = 0; i < 50; i++) {
        playlists << utf_note + " You";
        playlists << utf_note + " Found";
        playlists << utf_note + " Freday Brazilectro";
        playlists << utf_note + " Champange Evening";
        playlists << utf_note + " Sleepy Evening";
    }

    stringListModel_buttons -> setStringList(buttons);
    stringListModel_playlists -> setStringList(playlists);

    ui->listView_viewSelects->setModel(stringListModel_buttons);
    ui->listView_playlists->setModel(stringListModel_playlists);

    ui->listView_playlists->setSpacing(2);
    ui->listView_viewSelects->setSpacing(2);


    ui->playing_now_picture->setPixmap(QPixmap("/media/pictures/pepe_smile.jpg"));


    ui->label->setAlignment(Qt::AlignCenter);

    Init_lower_toolbar(ui);
    //Init_upper_toolbar(ui);

}

void MainWindow::onStackedWidgetCurrentChanged(int current) {
    switch(current) {
    case 0: // currently the libraryview (should be home view)
        break;
    case 1: // currently the libraryview
        library_view->show_data(mpd_communicator->GetArtists(QString("AlbumArtist")));
        break;
    case 2: // currently the searchview
        break;
    }
}

void MainWindow::Init_upper_toolbar(Ui::MainWindow *ui) {
    ui->frame_highbar->setFixedHeight(80);
}

void MainWindow::Init_lower_toolbar(Ui::MainWindow *ui) {
    int circ = 24;
    ui->pushButton_last_song->setFixedHeight(circ);
    ui->pushButton_last_song->setFixedWidth(circ);

    ui->pushButton_next_song->setFixedHeight(circ);
    ui->pushButton_next_song->setFixedWidth(circ);

    ui->pushButton_pause->setFixedHeight(circ+4);
    ui->pushButton_pause->setFixedWidth(circ+4);

   // QRect rect(0,0,circ-6,circ-6);
   // QRegion region(rect, QRegion::Ellipse);
   // ui->pushButton_last_song->setMask(region);
   // ui->pushButton_next_song->setMask(region);
}


MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_listView_playlists_pressed(const QModelIndex &index)
{
    ui->listView_viewSelects->clearSelection();
}


void MainWindow::on_listView_viewSelects_pressed(const QModelIndex &index)
{
    ui->listView_playlists->clearSelection();
    int selectedIndex = ui->listView_viewSelects->currentIndex().row();
    ui->stackedWidget->setCurrentIndex(selectedIndex);
}



#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include "searchview.h"
#include "artistview.h"
#include <QListWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    datahandler.Initialize();

    //QString appDir = "Gridlink//Gridlink/Styles/fourth.qss";
    //LoadStyleSheet(appDir);

    SearchView *search_view = new SearchView(this);
    QListWidget *home = new QListWidget();
    library_view = new LibraryView(this);
    artist_view = new ArtistView(this);
    album_view = new AlbumView(this);

    ui->stackedWidget->addWidget(home); // this is supposed to be "home" view whcih is not yet implemented
    ui->stackedWidget->addWidget(library_view);
    ui->stackedWidget->addWidget(artist_view);
    ui->stackedWidget->addWidget(album_view);
    ui->stackedWidget->addWidget(search_view);

    connect(library_view,
            &LibraryView::ArtistDoubleClickedSignal,
            this,
            &MainWindow::OnArtistDoubleClickedSlot);

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

void MainWindow::ChangeView(VIEW view) {
    switch(view) {
    case VIEW_HOME:
        // TODO: IMPL
        break;
    case VIEW_LIBRARY:
        ui->stackedWidget->setCurrentWidget(library_view);
        break;
    case VIEW_ARTIST:
        ui->stackedWidget->setCurrentWidget(artist_view);
        break;
    case VIEW_ALBUM:
        ui->stackedWidget->setCurrentWidget(album_view);
        break;
    case VIEW_SEARCH:
        //ui->stackedWidget->setCurrentWidget(search);
        break;
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Init_upper_toolbar(Ui::MainWindow *ui) {
    ui->frame_highbar->setFixedHeight(80);
}

void MainWindow::Init_lower_toolbar(Ui::MainWindow *ui) {
    int circ = 24;
    //ui->pushButton_last_song->setFixedHeight(circ);
    //ui->pushButton_last_song->setFixedWidth(circ);

    //ui->pushButton_next_song->setFixedHeight(circ);
    //ui->pushButton_next_song->setFixedWidth(circ);

    //ui->pushButton_pause->setFixedHeight(circ+4);
    //ui->pushButton_pause->setFixedWidth(circ+4);

   // QRect rect(0,0,circ-6,circ-6);
   // QRegion region(rect, QRegion::Ellipse);
   // ui->pushButton_last_song->setMask(region);
   // ui->pushButton_next_song->setMask(region);
}






void MainWindow::on_listView_playlists_pressed(const QModelIndex &index)
{
    ui->listView_viewSelects->clearSelection();
}


void MainWindow::on_listView_viewSelects_pressed(const QModelIndex &index)
{
    ui->listView_playlists->clearSelection();
    int selectedIndex = ui->listView_viewSelects->currentIndex().row();

    switch (selectedIndex) {
    case 0:
        ChangeView(VIEW_HOME);
        break;
    case 1:
        library_view->show_data(datahandler.GetArtistNames());
        ChangeView(VIEW_LIBRARY);
        break;
    case 2:
        ChangeView(VIEW_SEARCH);
        break;
    }
}

void MainWindow::OnArtistDoubleClickedSlot(const QString &artistname) {
    ChangeView(VIEW_ARTIST);
    artist_view->show_data(datahandler.GetAlbumsForArtist(artistname));
}



void MainWindow::LoadStyleSheet(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qDebug() << styleSheet;
        this->setStyleSheet(styleSheet);
        QApplication::setStyle(styleSheet);
    }
}

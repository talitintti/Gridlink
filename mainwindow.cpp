#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include "searchview.h"
#include "artistview.h"
#include <QListWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    setWindowTitle("Gridlink");

    datahandler_.Initialize();

    //QString appDir = "Gridlink//Gridlink/Styles/fourth.qss";
    //LoadStyleSheet(appDir);

    SearchView *search_view = new SearchView(this);
    QListWidget *home = new QListWidget();
    library_view_ = new LibraryView(this);
    artist_view_ = new ArtistView(this);
    album_view_ = new AlbumView(this);

    ui_->stackedWidget->addWidget(home); // this is supposed to be "home" view whcih is not yet implemented
    ui_->stackedWidget->addWidget(library_view_);
    ui_->stackedWidget->addWidget(artist_view_);
    ui_->stackedWidget->addWidget(album_view_);
    ui_->stackedWidget->addWidget(search_view);

    connect(library_view_,
            &LibraryView::ArtistDoubleClickedSignal,
            this,
            &MainWindow::OnArtistDoubleClickedSlot);

    connect(artist_view_,
            &ArtistView::AlbumDoubleClickedSignal,
            this,
            &MainWindow::OnAlbumDoubleClickedSlot);

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

    ui_->listView_viewSelects->setModel(stringListModel_buttons);
    ui_->listView_playlists->setModel(stringListModel_playlists);

    ui_->listView_playlists->setSpacing(2);
    ui_->listView_viewSelects->setSpacing(2);


    ui_->playing_now_picture->setPixmap(QPixmap("/media/pictures/pepe_smile.jpg"));


    ui_->label_playing_info->setAlignment(Qt::AlignCenter);

    Init_lower_toolbar(ui_);
    //Init_upper_toolbar(ui);

}

void MainWindow::ChangeView(VIEW view) {
    switch(view) {
    case VIEW_HOME:
        // TODO: IMPL
        break;
    case VIEW_LIBRARY:
        ui_->stackedWidget->setCurrentWidget(library_view_);
        break;
    case VIEW_ARTIST:
        ui_->stackedWidget->setCurrentWidget(artist_view_);
        break;
    case VIEW_ALBUM:
        ui_->stackedWidget->setCurrentWidget(album_view_);
        break;
    case VIEW_SEARCH:
        //ui->stackedWidget->setCurrentWidget(search);
        break;
    }
}


MainWindow::~MainWindow()
{
    delete ui_;
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
    ui_->listView_viewSelects->clearSelection();
}


void MainWindow::on_listView_viewSelects_pressed(const QModelIndex &index)
{
    ui_->listView_playlists->clearSelection();
    int selectedIndex = ui_->listView_viewSelects->currentIndex().row();

    switch (selectedIndex) {
    case 0:
        ChangeView(VIEW_HOME);
        break;
    case 1:
        library_view_->SetData(datahandler_.GetArtistNames());
        ChangeView(VIEW_LIBRARY);
        break;
    case 2:
        ChangeView(VIEW_SEARCH);
        break;
    }
}

void MainWindow::OnArtistDoubleClickedSlot(const QString &artistname) {
    ChangeView(VIEW_ARTIST);
    artist_view_->SetData(datahandler_.GetAlbums(artistname));
}

void MainWindow::OnAlbumDoubleClickedSlot(const Album &album) {
    ChangeView(VIEW_ALBUM);
    album_view_->SetAlbum(album); // check that this isn't retarded
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

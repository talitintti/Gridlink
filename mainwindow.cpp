#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include "searchview.h"
#include "artistview.h"
#include <QListWidget>
#include "progressbarwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    setWindowTitle("Gridlink");

    datahandler_ = new DataHandler(this);
    datahandler_->Initialize();

    //QString appDir = "Gridlink//Gridlink/Styles/fourth.qss";
    //LoadStyleSheet(appDir);

    // Init views
    SearchView *search_view = new SearchView(this);
    QListWidget *home = new QListWidget();
    library_view_ = new LibraryView(this);
    artist_view_ = new ArtistView(this);
    album_view_ = new AlbumView(this);
    playlist_view_ = new PlaylistView(this);

    // Init the progress bar
    auto progressbar_frame = ui_->frame_2;
    progress_bar_ = new ProgressBarWidget(progressbar_frame);
    auto layout = progressbar_frame->layout();
    layout->addWidget(progress_bar_);
    layout->setContentsMargins(0, 0, 0, 0);

    // init the playlist bar on the left
    playlist_model_ = new PlaylistListModel(this);
    auto playlists = datahandler_->GetPlaylists();
    playlist_model_->SetPlaylists(playlists);
    ui_->listView_playlists->setModel(playlist_model_);

    //Volume slider
    volume_slider_ = ui_->volume_slider;
    volume_slider_->setRange(0,100);

    // Buttons
    button_next_song_ = ui_->pushButton_next_song;
    button_previous_song_ = ui_->pushButton_last_song;

    ui_->stackedWidget->addWidget(home); // this is supposed to be "home" view whcih is not yet implemented
    ui_->stackedWidget->addWidget(library_view_);
    ui_->stackedWidget->addWidget(artist_view_);
    ui_->stackedWidget->addWidget(album_view_);
    ui_->stackedWidget->addWidget(search_view);
    ui_->stackedWidget->addWidget(playlist_view_);

    connect(library_view_,
            &LibraryView::ArtistDoubleClickedSignal,
            this,
            &MainWindow::OnArtistDoubleClickedSlot);

    connect(artist_view_,
            &ArtistView::AlbumDoubleClickedSignal,
            this,
            &MainWindow::OnAlbumDoubleClickedSlot);

    connect(album_view_,
            &AlbumView::SongChosenForPlaySignal,
            this,
            &MainWindow::PlaySongsSlot);

    connect(datahandler_,
            &DataHandler::SongUpdateSignal,
            this,
            &MainWindow::SongChanged);

    connect(datahandler_,
            &DataHandler::SongStoppedSignal,
            this,
            &MainWindow::PlaybackStopped);

    connect(datahandler_,
            &DataHandler::PlayStartedSignal,
            this,
            &MainWindow::PlaybackStarted);

    connect(datahandler_,
            &DataHandler::SongPausedSignal,
            this,
            &MainWindow::PlaybackPaused);

    connect(datahandler_,
            &DataHandler::SongElapsedSignal,
            this,
            &MainWindow::SongPositionChanged);

    connect(datahandler_,
            &DataHandler::VolumeChanged,
            this,
            &MainWindow::VolumeUpdate);

    connect(datahandler_,
            &DataHandler::PlaylistsChanged,
            this,
            &MainWindow::PlaylistUpdate);

    connect(datahandler_,
            &DataHandler::DatabaseChanged,
            this,
            &MainWindow::DatabaseUpdated);

    connect(ui_->pushButton_view_back,
            &QPushButton::clicked,
            this,
            &MainWindow::ViewBackClicked);

    connect(ui_->pushButton_view_forward,
            &QPushButton::clicked,
            this,
            &MainWindow::ViewForwardClicked);

    connect(progress_bar_,
            &ProgressBarWidget::PositionChanged,
            this,
            &MainWindow::SongPositionChangeByUser);

    connect(volume_slider_,
            &QSlider::sliderMoved,
            this,
            &MainWindow::VolumeChangeByUserSlot);

    connect(volume_slider_,
            &QSlider::actionTriggered,
            this,
            &MainWindow::UserClickedVolSlider);

    connect(button_next_song_,
            &QPushButton::clicked,
            this,
            &MainWindow::UserClickedNextSong);

    connect(button_previous_song_,
            &QPushButton::clicked,
            this,
            &MainWindow::UserClickedLastSong);


    connect(ui_->listView_playlists,
            &QAbstractItemView::clicked,
            this,
            &MainWindow::UserClickedPlaylist);


    datahandler_->ManualStatusUpdate(); // status update once at start


    stringListModel_buttons = new QStringListModel(this);
    stringListModel_playlists = new QStringListModel(this);

    QStringList buttons;

    QByteArray hex_note = QByteArray::fromHex("F09F8EB5");
    QString utf_note = QString::fromUtf8(hex_note);
    QByteArray hex_house = QByteArray::fromHex("F09F8FA0");
    QString utf_house = QString::fromUtf8(hex_house);

    buttons << utf_house + " Home";
    buttons << utf_house + " Library";
    buttons << utf_house + " Discover";

    stringListModel_buttons -> setStringList(buttons);

    ui_->listView_viewSelects->setModel(stringListModel_buttons);

    ui_->listView_playlists->setSpacing(2);
    ui_->listView_viewSelects->setSpacing(2);


    ui_->playing_now_picture->setPixmap(QPixmap("/media/pictures/pepe_smile.jpg"));


    ui_->label_playing_info->setAlignment(Qt::AlignCenter);

    Init_lower_toolbar(ui_);
    //Init_upper_toolbar(ui);

}

MainWindow::~MainWindow()
{
    delete ui_;
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
        case VIEW_PLAYLIST:
            ui_->stackedWidget->setCurrentWidget(playlist_view_);
            break;
        default:
            qWarning() << "Trying to change view to invalid values\n";
    }
}


void MainWindow::Init_upper_toolbar(Ui::MainWindow *ui) {
    ui->frame_highbar->setFixedHeight(80);
}

void MainWindow::Init_lower_toolbar(Ui::MainWindow *ui) {
//	int circ = 30;
//   ui->pushButton_last_song->setFixedHeight(circ);
//   ui->pushButton_last_song->setFixedWidth(circ);

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


void MainWindow::on_listView_viewSelects_pressed(const QModelIndex &index) {
    ui_->listView_playlists->clearSelection();
    int selectedIndex = ui_->listView_viewSelects->currentIndex().row();

    VIEW corrsp_view;
    switch (selectedIndex) {
    case 0:
        corrsp_view = VIEW_HOME;
        break;
    case 1:
        corrsp_view = VIEW_LIBRARY;
        library_view_->SetData(datahandler_->GetArtistNames());
        break;
    case 2:
        corrsp_view = VIEW_SEARCH;
        break;
    default:
        qWarning() << "Pressed invalid view in viewselect!\n";
        return;
    }

    viewhistory_.AddView(corrsp_view);
    ChangeView(corrsp_view);
}

void MainWindow::OnArtistDoubleClickedSlot(const QString &artistname) {
    viewhistory_.AddView(VIEW_ARTIST);
    artist_view_->SetData(datahandler_->GetAlbums(artistname), artistname);
    ChangeView(VIEW_ARTIST);
}

void MainWindow::OnAlbumDoubleClickedSlot(const Album &album) {
    viewhistory_.AddView(VIEW_ALBUM);
    album_view_->SetAlbum(album); // check that this isn't retarded
    ChangeView(VIEW_ALBUM);
}

// Status updates from outside the UI
//void MainWindow::StatusUpdate(const MPDStatus &info) {
//    // set album_art
//    // set artist_name & song name & album_name
//    QString song_name = info.CurrentSong().GetName();
//    QString artist_name = info.CurrentSong().GetArtist();
//    ui_->label_playing_info->setText(QString("<b>%1</b><br>%2").
//                                     arg(song_name, artist_name));
//
//
//    // Tell albumview that we are playing something (if we are)
//    // TODO:set kbit rate
//    // TODO:set how much has been played and how long the song is
//}


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

void MainWindow::on_pushButton_pause_clicked()
{
    datahandler_->TogglePlay();
}

// When UI component informs that some songs are to be played
void MainWindow::PlaySongsSlot(const QList<Song> &songs, unsigned index) {
    datahandler_->ClearQueue();
    datahandler_->AddToQueue(songs);
    datahandler_->StartPlayingQueue(index);
}


void MainWindow::SongChanged(const Song &song) {
    if (song.IsEmpty()) return;

    QString song_name = song.GetName();
    QString artist_name = song.GetArtist();
    if (song_name.isEmpty() || artist_name.isEmpty()) return;

    ui_->label_playing_info->setText(QString("<b>%1</b><br>%2").
                                     arg(song_name, artist_name));

    album_view_->InformSongPlaying(song);
    progress_bar_->SetLength(song.GetDurationSec());
}

void MainWindow::PlaybackStopped() {
    ui_->label_playing_info->setText("");
    album_view_->InformSongNotPlaying();
    progress_bar_->HandleStop();
}

void MainWindow::PlaybackPaused() {
    progress_bar_->HandlePause();
}

void MainWindow::PlaybackStarted() {
    progress_bar_->HandlePlay();
}

void MainWindow::ViewBackClicked() {
    VIEW last_view = viewhistory_.MoveBack();
    ChangeView(last_view);
}

void MainWindow::SongPositionChanged(unsigned elapsed_ms) {
    progress_bar_->SetPosition(elapsed_ms);
}

void MainWindow::ViewForwardClicked() {
    VIEW next_view = viewhistory_.MoveForward();
    ChangeView(next_view);
}

void MainWindow::SongPositionChangeByUser(unsigned ms) {
    datahandler_->SeekPos(ms);
}

void MainWindow::VolumeChangeByUserSlot(int value) {
    datahandler_->SetVolume(value);
}

void MainWindow::VolumeUpdate(unsigned volume) {
    ui_->volume_slider->setValue(volume);
}

void MainWindow::UserClickedNextSong() {
    datahandler_->PlayNext();
}

void MainWindow::UserClickedLastSong() {
    datahandler_->PlayPrevious();
}

void MainWindow::UserClickedVolSlider(int action) {
if (action == QAbstractSlider::SliderPageStepAdd ||
    action == QAbstractSlider::SliderPageStepSub)
    {
        QPoint local_mouse_pos = volume_slider_->mapFromGlobal(QCursor::pos());
        float pos_ratio = local_mouse_pos.x() / (float)volume_slider_->size().width();
        int volume_sliderrange = volume_slider_->maximum() - volume_slider_->minimum();
        int clicked_volume_sliderpos = volume_slider_->minimum() + volume_sliderrange * pos_ratio;

        volume_slider_->setValue(clicked_volume_sliderpos);
        VolumeChangeByUserSlot(clicked_volume_sliderpos);
    }
}

void MainWindow::PlaylistUpdate() {
    playlist_model_->ResetModel();
}

// Update all the views
// TODO: IF OTHER VIEWS ADDED REMEMBER TO UPDATE THIS
void MainWindow::DatabaseUpdated() {
    library_view_->SetData(datahandler_->GetArtistNames());

    auto current_artist_in_av = artist_view_->GetCurrentArtist();
    artist_view_->SetData(datahandler_->GetAlbums(current_artist_in_av), current_artist_in_av);

    auto current_album = album_view_->GetCurrentAlbum();
    auto a_name = current_album.GetName();
    auto a_artist = current_album.GetAlbumArtist();
    album_view_->SetAlbum(datahandler_->GetAlbum(a_artist, a_name));

    datahandler_->GetPlaylists();
}

void MainWindow::UserClickedPlaylist(const QModelIndex &index) {
    auto row = index.row();
    auto playlist = datahandler_->GetPlaylist(row);
    qDebug() << playlist.GetName();
    playlist_view_->SetPlaylist(playlist);
    ChangeView(VIEW_PLAYLIST);
}

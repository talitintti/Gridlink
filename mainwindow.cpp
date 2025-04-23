#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include "searchview.h"
#include "artistview.h"
#include <QListWidget>
#include "progressbarwidget.h"
#include <QMenu>
#include "enums.h"
#include "songcollectionview.h"
#include <utility>
#include "playlistadddialog.h"

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
    album_view_ = new SongCollectionView(this);
    playlist_view_ = new SongCollectionView(this);

    // albumview stuff
    auto get_playlists = [this]() {
        return datahandler_->GetPlaylists();
    };
    album_view_->playlist_provider_ = get_playlists;

    // playlistview stuff
    playlist_view_->playlist_provider_ = get_playlists;


    // Init the progress bar
    auto progressbar_frame = ui_->frame_2;
    progress_bar_ = new ProgressBarWidget(progressbar_frame);
    auto layout = progressbar_frame->layout();
    layout->addWidget(progress_bar_);
    layout->setContentsMargins(0, 0, 0, 0);

    // init the playlist list on the left
    //playlist_model_ = new PlaylistListModel(this);
    //auto playlists = datahandler_->GetPlaylists();
    //playlist_model_->SetPlaylists(playlists);
    //ui_->listView_playlists->setModel(playlist_model_);
    //ui_->listView_playlists->setContextMenuPolicy(Qt::CustomContextMenu);
    playlist_list_ = new PlaylistList(this);
    auto playlists = datahandler_->GetPlaylists();
    playlist_list_->SetPlaylists(playlists);
    ui_->verticalLayout_leftside->insertWidget(1,playlist_list_);

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
            &SongCollectionView::SongChosenForPlaySignal,
            this,
            &MainWindow::PlaySongsSlot);

    connect(playlist_view_,
            &SongCollectionView::SongChosenForPlaySignal,
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
            playlist_list_,
            &PlaylistList::PlaylistUpdate);

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

    connect(playlist_list_,
            &PlaylistList::PlaylistClickedSignal,
            this,
            &MainWindow::UserClickedPlaylist);

    connect(playlist_list_,
            &PlaylistList::DeletingPlaylist,
            this,
            &MainWindow::DeletePlaylist);

    connect(album_view_,
           &SongCollectionView::UserAddingSongsToQueue,
           this,
           &MainWindow::AddToQueue);

    connect(album_view_,
            &SongCollectionView::UserAddingSongsToPlaylist,
            this,
            &MainWindow::AddToPlaylist);

    connect(playlist_view_,
            &SongCollectionView::UserDeletingFromPlaylist,
            this,
            &MainWindow::OnDeletingFromPlaylists);

    //these two below are the same
    connect(album_view_,
            &SongCollectionView::ShowPlaylistAddDialog,
            this,
            &MainWindow::ShowPlaylistDialog);
    connect(playlist_view_,
            &SongCollectionView::ShowPlaylistAddDialog,
            this,
            &MainWindow::ShowPlaylistDialog);

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

    //ui_->listView_playlists->setSpacing(2);
    //ui_->listView_viewSelects->setSpacing(2);


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
        case VIEW::HOME:
                // TODO: IMPL
                break;
        case VIEW::LIBRARY:
                ui_->stackedWidget->setCurrentWidget(library_view_);
                break;
        case VIEW::ARTIST:
                ui_->stackedWidget->setCurrentWidget(artist_view_);
                break;
        case VIEW::ALBUM:
                ui_->stackedWidget->setCurrentWidget(album_view_);
                break;
        case VIEW::SEARCH:
                //ui->stackedWidget->setCurrentWidget(search);
                break;
        case VIEW::PLAYLIST:
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


void MainWindow::on_listView_viewSelects_pressed(const QModelIndex &index) {
    playlist_list_->ClearSelection();
    int selectedIndex = ui_->listView_viewSelects->currentIndex().row();

    VIEW corrsp_view;
    switch (selectedIndex) {
    case 0:
        corrsp_view = VIEW::HOME;
        break;
    case 1:
        corrsp_view = VIEW::LIBRARY;
        library_view_->SetData(datahandler_->GetArtistNames());
        break;
    case 2:
        corrsp_view = VIEW::SEARCH;
        break;
    default:
        qWarning() << "Pressed invalid view in viewselect!\n";
        return;
    }

    viewhistory_.AddView(corrsp_view, std::any{});

    ChangeView(corrsp_view);
}


void MainWindow::OnArtistDoubleClickedSlot(const QString &artistname) {
    auto albums = datahandler_->GetAlbumsForArtist(artistname);
    std::any data = albums;
    viewhistory_.AddView(VIEW::ARTIST, std::move(data));
    artist_view_->SetData(datahandler_->GetAlbumsForArtist(artistname), artistname);

    ChangeView(VIEW::ARTIST);
}


void MainWindow::OnAlbumDoubleClickedSlot(const Album &album) {
    auto album_ptr = QSharedPointer<Album>::create(album);

    auto raw_ptr = album_ptr.get();
    const auto *collection = dynamic_cast<const SongCollection*>(raw_ptr);
    album_view_->SetSongCollection(collection);

    std::any data = std::move(album_ptr);
    viewhistory_.AddView(VIEW::ALBUM, std::move(data));
    ChangeView(VIEW::ALBUM);
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

void MainWindow::AddToQueue(const QList<Song> &songs)  {
    datahandler_->AddToQueue(songs);
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
    auto last_view = viewhistory_.MoveBack();
    HandleViewHistoryRet(last_view);
}

void MainWindow::SongPositionChanged(unsigned elapsed_ms) {
    progress_bar_->SetPosition(elapsed_ms);
}

void MainWindow::ViewForwardClicked() {
    auto next_view = viewhistory_.MoveForward();
    HandleViewHistoryRet(next_view);
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


// Update all the views
// TODO: IF OTHER VIEWS ADDED REMEMBER TO UPDATE THIS
void MainWindow::DatabaseUpdated() {
    // Clean all views except the first added and move to it

    // playlist update
    datahandler_->GetPlaylists();
}

void MainWindow::HandleViewHistoryRet(std::tuple<VIEW, std::any> &tuple) {
    auto view = get<0>(tuple);
    auto data = get<1>(tuple);

    switch (view) {
    case VIEW::LIBRARY:
        break;
    case VIEW::ARTIST:
        break;
    case VIEW::ALBUM: {
        auto raw_ptr = std::any_cast<QSharedPointer<Album>>(data).data();
        album_view_->SetSongCollection(raw_ptr);
        break;
    }
    case VIEW::PLAYLIST: {
        auto raw_ptr = std::any_cast<QSharedPointer<Playlist>>(data).data();
        playlist_view_->SetSongCollection(raw_ptr);
        break;
    }
    case VIEW::SEARCH:
        break;
    case VIEW::HOME:
        break;
    case VIEW::UNKNOWN:
        break;
    }

    ChangeView(view);
}


void MainWindow::UserClickedPlaylist(size_t hash) {
    ui_->listView_viewSelects->clearSelection();

    auto shared_ptr = datahandler_->GetPlaylistH(hash);

    std::any data(shared_ptr);

    viewhistory_.AddView(VIEW::PLAYLIST, std::move(data));
    playlist_view_->SetSongCollection(shared_ptr.data());
    ChangeView(VIEW::PLAYLIST);
}


void MainWindow::AddToPlaylist(const QList<Song> &list, const Playlist *playlist) {
    datahandler_->AddToPlaylist(list, playlist);
}


void MainWindow::DeletePlaylist(size_t hash) {
    datahandler_->DeletePlaylist(hash);
}


//void MainWindow::OnPlaylistContextMenu(const QPoint &pos) {
//    QModelIndex index = ui_->listView_playlists->indexAt(pos);
//
//    if (!index.isValid()) return;
//
//    int row = index.row();
//
//    QString delete_playlist_text = "Delete playlist";
//    QString rename_playlist_text = "Rename playlist";
//
//    QMenu menu;
//    QAction *delete_playlist_action = menu.addAction(delete_playlist_text);
//    QAction *rename_playlist_action = menu.addAction(rename_playlist_text);
//
//    QAction *selectedAction = menu.exec(ui_->listView_playlists->viewport()->mapToGlobal(pos));
//
//    if (selectedAction == delete_playlist_action) {
//        auto ptr = datahandler_->GetPlaylist(row);
//        viewhistory_.Remove(ptr->GetHash());
//        datahandler_->DeletePlaylist(row);
//    }
//    if (selectedAction == rename_playlist_action) {
//        //datahandler_.RenamePlaylist(uint row);
//    }
//}


void MainWindow::OnDeletingFromPlaylists(const QList<Song> &songs, const Playlist *playlist) {
    datahandler_->DeleteFromPlaylist(songs, playlist);
}


void MainWindow::ShowPlaylistDialog() {
    PlaylistAddDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString plname = dialog.GetPlaylistName();
        if (!plname.isEmpty()) {
            datahandler_->AddPlaylist(plname);
        }
    }

}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include "libraryview.h"
#include "songcollectionview.h"
#include "datahandler.h"
#include "artistview.h"
#include "progressbarwidget.h"
#include "enums.h"
#include "viewhistory.h"
#include "playlistlist.h"
#include <any>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //void StatusUpdate(const MPDStatus&);

private slots:
    // TODO: make uniform casing
    void on_listView_viewSelects_pressed(const QModelIndex &index);//TODO: connnect by hand
    void on_pushButton_pause_clicked();//TODO: connnect by hand
    void PlaySongsSlot(const QList<Song> &, unsigned);
    void ViewBackClicked();
    void ViewForwardClicked();
    void OnArtistDoubleClickedSlot(const QString &artistname);
    void OnAlbumDoubleClickedSlot(const Album &album);
    void SongPositionChangeByUser(unsigned ms);
    void VolumeChangeByUserSlot(int);
    void UserClickedVolSlider(int action);
    void UserClickedNextSong();
    void UserClickedLastSong();
    void UserClickedPlaylist(size_t hash);

    void PlaybackPaused();
    void PlaybackStarted();
    void SongPositionChanged(unsigned elapsed_ms);
    void SongChanged(const Song &);
    void PlaybackStopped();
    void VolumeUpdate(unsigned vol);
    void DatabaseUpdated();
    void AddToQueue(const QList<Song> &);
    void AddToPlaylist(const QList<Song> &, const Playlist *);
    void PlaylistsDeleted(const QList<QString> names);

    void OnDeletingFromPlaylists(const QList<Song> &, const Playlist *);
    void DeletePlaylist(unsigned row);
    void OnPlaylistUpdate(const QList<QSharedPointer<Playlist>> changed);

    void HandleViewHistoryRet(std::tuple<VIEW, std::any> &tuple);

    void ShowPlaylistDialog(const QList<Song> &);

private:
    Ui::MainWindow *ui_;
    DataHandler *datahandler_;
    ViewHistory viewhistory_;

    LibraryView *library_view_;
    ArtistView *artist_view_;
    SongCollectionView *album_view_;
    SongCollectionView *playlist_view_;

    QStringListModel *stringListModel_buttons;
    QStringListModel *stringListModel_playlists;

    ProgressBarWidget *progress_bar_; //TODO: better name
    QSlider *volume_slider_;
    QPushButton *button_next_song_;
    QPushButton *button_previous_song_;

    PlaylistList *playlist_list_;

    void Init_lower_toolbar(Ui::MainWindow *ui);
    void Init_upper_toolbar(Ui::MainWindow *ui);
    void ChangeView(VIEW);

    void LoadStyleSheet(const QString &filePath);
  };
#endif // MAINWINDOW_H

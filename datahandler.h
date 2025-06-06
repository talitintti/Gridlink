#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "mpd_communication.h"
#include "config.h"
#include <QFile>
#include "album.h"
#include "mpdstatus.h"
#include "mpdnotif.h"
#include "playlist.h"

class DataHandler : public QObject
{
    Q_OBJECT

public:
    DataHandler(QObject *parent = nullptr) : QObject(parent) {};
    ~DataHandler();

    bool Initialize();
    QList<QString> GetArtistNames();
    QList<QString> GetAlbumNames(const QString &artist_name);
    QList<Album> GetAlbumsForArtist(const QString &artist_name);
    const QSharedPointer<const Album> GetAlbum(const QString &artist_name, const QString &album_name);

    void TogglePlay();
    void AddToQueue(const QList<Song> &);
    void StartPlayingQueue(unsigned index = 0);
    void ClearQueue();
    void ManualStatusUpdate();
    void SeekPos(unsigned pos_ms);
    void SetVolume(unsigned vol);
    void PlayNext();
    void PlayPrevious();

    QList<QSharedPointer<Playlist>> *GetPlaylists();
    const Playlist *GetPlaylist(unsigned row);
    // Row based search (faster)
    const QSharedPointer<Playlist> GetPlaylistR(unsigned row);
    // Hash based search (slower)
    const QSharedPointer<Playlist> GetPlaylistH(size_t hash);
    void AddToPlaylist(const QList<Song> &, const Playlist *);
    void DeletePlaylist(unsigned row);
    void DeleteFromPlaylist(const QList<Song> &, const Playlist *);
    void CreatePlaylist(const QString &name, const QList<Song> &songs);

signals:
    //void StatusUpdateSignal(const MPDStatus&);
    void SongUpdateSignal(const Song &);
    void PlayStartedSignal();
    void SongPausedSignal();
    void SongStoppedSignal();
    void SongElapsedSignal(unsigned elapsed_ms);
    void VolumeChanged(unsigned volume);
    void PlaylistsChanged(QList<QSharedPointer<Playlist>> changed);
    void PlaylistAdded();
    void PlaylistsDeleted(QList<QString> names);
    void DatabaseChanged();


private slots:
    void StatusUpdateSlot(mpd_idle);

private:
    MPDCommunication mpd_communicator_ = MPDCommunication();
    MPDNotif *mpd_status_updates_;
    Config config_;
    MPDStatus last_update_;
    QList<QSharedPointer<Playlist>> playlists_;


    bool WriteConfigFile(QFile &configFile, const Config &config);
    Config ReadConfigFile();
    void SetAlbumCover(Album &album) const;
    bool FetchStatusUpdate();
    void ParseStatusUpdate();
    void FetchPlaylists();
    void PlaylistChangeHandler();
};

#endif // DATAHANDLER_H

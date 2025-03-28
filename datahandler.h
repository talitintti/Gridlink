#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "mpd_communication.h"
#include "config.h"
#include <QFile>
#include "album.h"
#include "mpdstatus.h"
#include "mpdnotif.h"

class DataHandler : public QObject
{
    Q_OBJECT

public:
    DataHandler(QObject *parent = nullptr) : QObject(parent) {};
    ~DataHandler();

    bool Initialize();
    QList<QString> GetArtistNames();
    QList<QString> GetAlbumNames(const QString &artist_name);
    QList<Album> GetAlbums(const QString &artist_name);
    Album GetAlbum(const QString &artist_name, const QString &album_name);
    void TogglePlay();
    void AddToQueue(const QList<Song> &);
    void StartPlayingQueue(unsigned index = 0);
    void ClearQueue();
    void ManualStatusUpdate();
    void SeekPos(unsigned pos_ms);
    void SetVolume(unsigned vol);
    void PlayNext();
    void PlayPrevious();

signals:
    //void StatusUpdateSignal(const MPDStatus&);
    void SongUpdateSignal(const Song &);
    void PlayStartedSignal();
    void SongPausedSignal();
    void SongStoppedSignal();
    void SongElapsedSignal(unsigned elapsed_ms);
    void VolumeChanged(unsigned volume);


private slots:
    void StatusUpdateSlot(mpd_idle);

private:
    bool WriteConfigFile(QFile &configFile, const Config &config);
    Config ReadConfigFile();
    void SetAlbumCover(Album &album) const;
    bool FetchStatusUpdate();
    void ParseStatusUpdate();


    MPDCommunication mpd_communicator_ = MPDCommunication();
    MPDNotif *mpd_status_updates_;
    Config config_;
    MPDStatus last_update_;
};

#endif // DATAHANDLER_H

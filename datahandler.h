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

    // Returns an empty string on success
    // Return error msg on failure
    bool Initialize();
    QList<QString> GetArtistNames();
    QList<QString> GetAlbumNames(const QString &artist_name);
    QList<Album> GetAlbums(const QString &artist_name);
    Album GetAlbum(const QString &artist_name, const QString &album_name);

signals:
    void StatusUpdateSignal(const MPDStatus&);

private slots:
    void StatusUpdateSlot(mpd_idle);

private:
    bool WriteConfigFile(QFile &configFile, const Config &config);
    Config ReadConfigFile();
    void SetAlbumCover(Album &album) const;
    void StatusUpdate();


    MPDCommunication mpd_communicator_ = MPDCommunication();
    MPDNotif *mpd_status_updates_;
    Config config_;
    MPDStatus last_update_;



};

#endif // DATAHANDLER_H

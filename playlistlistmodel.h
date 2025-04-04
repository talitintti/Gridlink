#ifndef PLAYLISTLISTMODEL_H
#define PLAYLISTLISTMODEL_H
#include <QDebug>
#include <QAbstractListModel>
#include "playlist.h"
#include "album.h"
#include <QMimeData>
#include <QIODevice>
#include "mimedata.h"

class PlaylistListModel : public QAbstractListModel {
    Q_OBJECT

public:
    PlaylistListModel(QObject *parent) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent) const {
        if (parent.isValid()) return 0;
        return playlists_.size();
    }

    QVariant data(const QModelIndex &index, int role) const {
        if (!index.isValid() || index.row() >= playlists_.size()) return QVariant();

        if (role == Qt::DisplayRole) {
            return playlists_.at(index.row()).GetName();
        }

        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex &index) const {
        Qt::ItemFlags defaultFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

        if (!index.isValid()) {
            return defaultFlags | Qt::ItemIsDropEnabled; // Allow dropping at root
        }

        return defaultFlags;
    }

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
        Q_UNUSED(row)
        Q_UNUSED(column)

        if (action != Qt::CopyAction && action != Qt::MoveAction) return false;

        if (!data->hasFormat("application/x-album-pointer")) return false;

        const AlbumMimeData *album_mime_data = qobject_cast<const AlbumMimeData *>(data);
        auto album_pointer = album_mime_data->albumPointer();

        if (!parent.isValid()) {
            // Dropped at the root level, emit signal for general drop
            emit AlbumDroppedOnModel(album_pointer);
        } else {
            // Dropped on a specific playlist
            Playlist target_playlist = playlists_.at(row);
            emit AlbumDroppedOnPlaylist(album_pointer, target_playlist);
        }

        return true;
    }

    QStringList mimeTypes() const {
        return {"application/x-album-pointer"}; // Accept text data as album name
    }

    Qt::DropActions supportedDropActions() const {
        return Qt::CopyAction | Qt::MoveAction;
    }

    void AddPlaylist(const Playlist &playlist) {
        beginInsertRows(QModelIndex(), playlists_.size(), playlists_.size());
        playlists_.append(playlist);
        endInsertRows();
    }

    void ResetModel() {
        beginResetModel();
        endResetModel();
    }

    void SetPlaylists(const QList<Playlist> &playlists) {
        beginResetModel();
        playlists_ = playlists;
        endResetModel();
    }

signals:
    void AlbumDroppedOnModel(Album *album);
    void AlbumDroppedOnPlaylist(Album *album, const Playlist &playlist);

private:
    QList<Playlist> playlists_;

};


#endif // PLAYLISTLISTMODEL_H

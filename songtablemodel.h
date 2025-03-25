#ifndef SONGTABLEMODEL_H
#define SONGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "helpers.h"
#include "song.h"
#include <QBrush>

class SongTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit SongTableModel(QObject *parent = nullptr)
        : QAbstractTableModel(parent),
        playing_song_row_(-1),
        last_played_song_hash_(-1),
        song_highlight_color_(Qt::green) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return song_list_.size(); // Number of songs
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 3; // Track Number, Name, Duration
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= song_list_.size())
            return QVariant();

        const Song &song = song_list_.at(index.row());
        qDebug() << index.row() << " : " << index.column() << " : " << role <<"\n";

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case TRACK_NUMBER: return index.row() + 1; // Track #
                case SONG_NAME: return song.GetName();        // Name
                case SONG_LENGTH: return FormattedTime(song.GetDurationSec());    // Duration
                default: return QVariant();
            }
        }

        if (role == Qt::ForegroundRole) {
            if (playing_song_row_ == index.row() && (index.column() == 0 || index.column() == 1)) {
                return QBrush(QColor(song_highlight_color_));
            }
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                switch (section) {
                    case TRACK_NUMBER: return "#";         // Track Number
                    case SONG_NAME: return "Song Name"; // Name
                    case SONG_LENGTH: return "Duration";  // Duration
                }
            }
        }
        return QVariant();
    }


    void SetSongAsPlaying(size_t song_hash) {
        if (playing_song_row_ != -1) return; // Something is already playing

        int song_index = FindSongInList(song_hash);
        if (song_index == -1) {
            qWarning() << "Trying to highlight song that doens't exist" << "\n";
            return;
        }

        playing_song_row_ = song_index;
        last_played_song_hash_ = song_hash;

        emit dataChanged(createIndex(song_index, TRACK_NUMBER),
                         createIndex(song_index, SONG_NAME),
                         {Qt::ForegroundRole});
    }

    void SetPlayingAsStopped() {
        if (playing_song_row_ == -1) return; // We are not playing anything
        int song_index = FindSongInList(last_played_song_hash_);
        if (song_index == -1) {
            qWarning() << "Trying to unhighlight nonexistent song" << "\n";
            return;
        }

        emit dataChanged(createIndex(song_index, TRACK_NUMBER),
                         createIndex(song_index, SONG_NAME),
                         {Qt::ForegroundRole});

        playing_song_row_ = -1;  // set as not playing
    }



    void SetHighlightColor(QRgb color) {
        song_highlight_color_ = color;
    }

    void SetSongs(const QList<Song> &songs) {
        beginResetModel();
        song_list_ = songs;
        //RedrawAll();
        endResetModel();
    }

private:
    QList<Song> song_list_;
    int playing_song_row_;
    size_t last_played_song_hash_; // needed since I wont rely on the song order not changing within the list
    QRgb song_highlight_color_;

    int counter = 0;

    static constexpr unsigned TRACK_NUMBER = 0;
    static constexpr unsigned SONG_NAME = 1;
    static constexpr unsigned SONG_LENGTH = 2;


    int FindSongInList(size_t song_hash) {
        for (int i=0; i<song_list_.count(); i++) {
            if (song_hash == song_list_.at(i).GetHash()) return i;
        }
        return -1;
    }

};

#endif // SONGTABLEMODEL_H

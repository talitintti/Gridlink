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
        return 4; // Track Number, Name, Duration
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= song_list_.size())
            return QVariant();

        const Song &song = song_list_.at(index.row());

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case TRACK_NUMBER: return index.row() + 1;
                case SONG_NAME: return song.GetName();
                case SONG_ARTIST: return song.GetAlbumArtist();
                case SONG_LENGTH: return FormattedTime(song.GetDurationSec());
                default: return QVariant();
            }
        }

        if (role == Qt::ForegroundRole) {
            if (playing_song_row_ == index.row() && (index.column() == 0 || index.column() == 1)) {
                return QBrush(QColor(song_highlight_color_));
            }
        }

        if (role == Qt::TextAlignmentRole) {
            if (index.column() == SONG_ARTIST) {
                return Qt::AlignCenter;
            }
            if (index.column() == SONG_LENGTH) {
                return Qt::AlignRight;
            }
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                switch (section) {
                    case TRACK_NUMBER: return "#";        // Track Number
                    case SONG_NAME: return "Song Name";   // Name
                    case SONG_ARTIST: return "Artist";
                    case SONG_LENGTH: return "⏲";  // Duration
                }
            }
        }

        if (role == Qt::TextAlignmentRole) {
            if (section == SONG_ARTIST) {
                return Qt::AlignCenter;
            }
            if (section == SONG_LENGTH) {
                return Qt::AlignRight;
            }
        }

        return QVariant();
    }


    void SetSongAsPlaying(size_t song_hash) {
        if (playing_song_row_ != -1) return; // Something is already playing

        int song_index = FindSongInList(song_hash);
        if (song_index == -1) {
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

    void ResetModel() {
        beginResetModel();
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
    static constexpr unsigned SONG_ARTIST = 2;
    static constexpr unsigned SONG_LENGTH = 3;


    int FindSongInList(size_t song_hash) {
        for (int i=0; i<song_list_.count(); i++) {
            if (song_hash == song_list_.at(i).GetHash()) return i;
        }
        return -1;
    }

};

#endif // SONGTABLEMODEL_H

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
    explicit SongTableModel(const QList<Song> &songs, QObject *parent = nullptr)
        : QAbstractTableModel(parent),
        song_list_(songs) ,
        playing_song_row_(-1),
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

    void ChangeSongList(const QList<Song> &songs) {
        song_list_ = QList<Song>(songs);
    }

    void HighlightSong(const Song &current_song) {
        for (int i=0; i<song_list_.count(); i++) {
            if (current_song == song_list_.at(i)) {
                emit dataChanged(createIndex(i,TRACK_NUMBER),
                                 createIndex(i,SONG_NAME),
                                 {Qt::ForegroundRole});
            }
        }
    }

    void SetHighlightColor(QRgb color) {
        song_highlight_color_ = color;
    }

private:
    QList<Song> song_list_;
    int playing_song_row_;
    QRgb song_highlight_color_;

    static constexpr unsigned TRACK_NUMBER = 0;
    static constexpr unsigned SONG_NAME = 1;
    static constexpr unsigned SONG_LENGTH = 2;
};

#endif // SONGTABLEMODEL_H

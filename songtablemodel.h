#ifndef SONGTABLEMODEL_H
#define SONGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "helpers.h"
#include "song.h"

class SongTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit SongTableModel(const QList<Song> &songs, QObject *parent = nullptr)
        : QAbstractTableModel(parent), song_list_(songs) {}

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
                case 0: return index.row() + 1; // Track #
                case 1: return song.GetName();        // Name
                case 2: return FormattedTime(song.GetDurationSec());    // Duration
                default: return QVariant();
            }
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                switch (section) {
                    case 0: return "#";         // Track Number
                    case 1: return "Song Name"; // Name
                    case 2: return "Duration";  // Duration
                }
            }
        }
        return QVariant();
    }

    void ChangeSongList(const QList<Song> &songs) {
        song_list_ = QList<Song>(songs);
    }

private:
    QList<Song> song_list_;
};

#endif // SONGTABLEMODEL_H

#ifndef ARTISTVIEW_H
#define ARTISTVIEW_H

#include <QWidget>
#include <QAbstractListModel>
#include "album.h"
#include <QListView>

class AlbumListModel : public QAbstractListModel {
    Q_OBJECT
 public:
    explicit AlbumListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    void setAlbums(QList<Album> albumList) {
        beginResetModel();
        albums_ = albumList;
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return static_cast<int>(albums_.size());
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= static_cast<int>(albums_.size()))
            return QVariant();

        if (role == Qt::DisplayRole) {
            return albums_.at(index.row()).GetName();
        }

        return QVariant();
    }

private:
   QList<Album> albums_;
};


namespace Ui {
class ArtistView;
}

class ArtistView : public QWidget
{
    Q_OBJECT



public:
    explicit ArtistView(QWidget *parent = nullptr);
    ~ArtistView();

    void SetData(QList<Album> albums);


private:
    Ui::ArtistView *ui_;
    AlbumListModel album_list_model_;
};

#endif // ARTISTVIEW_H

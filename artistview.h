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

    void SetAlbums(QList<Album> albumList) {
        beginResetModel();
        qDebug() << albumList.at(0).GetName() << " \n";
        albums_ = albumList;
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return static_cast<int>(albums_.size());
    }

    // TODO: maybe make shared_ptr in datahandler so we can avoid copying everything
    const Album &at(int index) {
        if (index >= 0 && index < albums_.count()) {
            return albums_.at(index);
        }
        throw "Trying to get with wrong index"; // TODO: Fix
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

    void SetData(QList<Album> albums, const QString &artist);
    const QString &GetCurrentArtist();

signals:
    void AlbumDoubleClickedSignal(const Album &clicked_album);

private slots:
    void AlbumDoubleClickedSlot(const QModelIndex &album_index);

private:
    Ui::ArtistView *ui_;
    AlbumListModel album_list_model_;
    QString current_artist_;
};

#endif // ARTISTVIEW_H

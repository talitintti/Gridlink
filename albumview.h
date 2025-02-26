#ifndef ALBUMVIEW_H
#define ALBUMVIEW_H

#include <QWidget>
#include "album.h"
#include "SongTableModel.h"
#include <QTableView>

namespace Ui {
class AlbumView;
}

class AlbumView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumView(QWidget *parent = nullptr);
    ~AlbumView();

    void SetData(const Album &album);
private:
    Ui::AlbumView *ui_;
    SongTableModel *song_table_model_ = nullptr;
    Album album_;

    void SetTableAppearance(QTableView *table);
};

#endif // ALBUMVIEW_H

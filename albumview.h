#ifndef ALBUMVIEW_H
#define ALBUMVIEW_H

#include <QWidget>
#include "album.h"
#include "SongTableModel.h"


namespace Ui {
class AlbumView;
}

class AlbumView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumView(QWidget *parent = nullptr);
    void SetData(const Album &album);
    ~AlbumView();

private:
    Ui::AlbumView *ui_;
    SongTableModel *song_table_model_ = nullptr;
};

#endif // ALBUMVIEW_H

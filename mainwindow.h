#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include "libraryview.h"
#include "datahandler.h"
#include "artistview.h"
#include "albumview.h"
#include "mpdstatus.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void OnArtistDoubleClickedSlot(const QString &artistname);
    void OnAlbumDoubleClickedSlot(const Album &album);
    void StatusUpdate(const MPDStatus&);
private slots:
    // TODO: Change these to camelCase or whatever
    void on_listView_playlists_pressed(const QModelIndex &index);
    void on_listView_viewSelects_pressed(const QModelIndex &index);
    void on_pushButton_pause_clicked();
    void PlaySongsSlot(const QList<Song> &, unsigned);

private:
    enum VIEW {
        VIEW_LIBRARY, VIEW_ARTIST, VIEW_ALBUM, VIEW_SEARCH, VIEW_HOME
    };

    Ui::MainWindow *ui_;
    QStringListModel *stringListModel_buttons;
    QStringListModel *stringListModel_playlists;
    void Init_lower_toolbar(Ui::MainWindow *ui);
    void Init_upper_toolbar(Ui::MainWindow *ui);
    void ChangeView(VIEW view);

    void LoadStyleSheet(const QString &filePath);

    DataHandler *datahandler_;

    LibraryView *library_view_;
    ArtistView *artist_view_;
    AlbumView *album_view_;

    QVector<QString> artist_data_;

  };
#endif // MAINWINDOW_H

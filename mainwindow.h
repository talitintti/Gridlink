#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include "libraryview.h"
#include "datahandler.h"
#include "artistview.h"
#include "albumview.h"
#include "progressbarwidget.h"
#include "enums.h"
#include "viewhistory.h"

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

    //void StatusUpdate(const MPDStatus&);
    void PlaybackPaused();
    void PlaybackStarted();
    void SongPositionChanged(unsigned elapsed_ms);
    void SongChanged(const Song &);
    void PlaybackStopped();
private slots:
    // TODO: make uniform casing
    void on_listView_playlists_pressed(const QModelIndex &index); //TODO: connnect by hand
    void on_listView_viewSelects_pressed(const QModelIndex &index);//TODO: connnect by hand
    void on_pushButton_pause_clicked();//TODO: connnect by hand
    void PlaySongsSlot(const QList<Song> &, unsigned);
    void ViewBackClicked();
    void ViewForwardClicked();
    void OnArtistDoubleClickedSlot(const QString &artistname);
    void OnAlbumDoubleClickedSlot(const Album &album);

private:
    Ui::MainWindow *ui_;
    DataHandler *datahandler_;
    ViewHistory viewhistory_;

    LibraryView *library_view_;
    ArtistView *artist_view_;
    AlbumView *album_view_;

    QStringListModel *stringListModel_buttons;
    QStringListModel *stringListModel_playlists;

    ProgressBarWidget *progress_bar_;

    void Init_lower_toolbar(Ui::MainWindow *ui);
    void Init_upper_toolbar(Ui::MainWindow *ui);
    void ChangeView(VIEW view);

    void LoadStyleSheet(const QString &filePath);

  };
#endif // MAINWINDOW_H

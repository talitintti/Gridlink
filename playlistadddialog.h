#ifndef PLAYLISTADDDIALOG_H
#define PLAYLISTADDDIALOG_H

#include <QDialog>
#include <QLineEdit>

class PlaylistAddDialog : public QDialog
{
    Q_OBJECT


public:
    explicit PlaylistAddDialog(QWidget *parent = nullptr);
    QString GetPlaylistName();

private:
    QLineEdit *lineedit_name_;
};

#endif // PLAYLISTADDDIALOG_H

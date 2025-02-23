#ifndef LIBRARYVIEW_H
#define LIBRARYVIEW_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class LibraryView;
}

class LibraryView : public QWidget
{
    Q_OBJECT


public:
    explicit LibraryView(QWidget *parent = nullptr);
    void SetData(const QList<QString> &data);
    ~LibraryView();

signals:
    void ArtistDoubleClickedSignal(const QString &artistname);

private slots:
    void ArtistDoubleClickedSlot(QListWidgetItem *item);

private:
    Ui::LibraryView *ui_;

};

#endif // LIBRARYVIEW_H

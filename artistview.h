#ifndef ARTISTVIEW_H
#define ARTISTVIEW_H

#include <QWidget>

namespace Ui {
class ArtistView;
}

class ArtistView : public QWidget
{
    Q_OBJECT



public:
    explicit ArtistView(QWidget *parent = nullptr);
    ~ArtistView();

    void show_data(const QList<QString> &data);

private:
    Ui::ArtistView *ui;
};

#endif // ARTISTVIEW_H

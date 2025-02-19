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

    void SetData(const QList<QString> &data);

private:
    Ui::ArtistView *ui_;
};

#endif // ARTISTVIEW_H

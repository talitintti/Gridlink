#include "albumview.h"
#include "ui_albumview.h"
#include <QTableView>
#include <QAbstractItemModel>
#include <QRgb>

AlbumView::AlbumView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::AlbumView)
{
    ui_->setupUi(this);
    song_table_model_ = new SongTableModel(this);
    ui_->tableView->setModel(song_table_model_);
    SetTableAppearance(ui_->tableView);
}

AlbumView::~AlbumView()
{
    delete ui_;
}

void AlbumView::SetAlbum(const Album &album) {
    album_ = album;

    QList<Song> albums = album_.GetSongs();

    song_table_model_->SetSongs(album.GetSongs());

    // Let's show the picture
    if (album_.HasCoverData()) {
        auto label = ui_->label_album_pic;
        const OImage &cover = album.GetCover();
        QImage image(cover.GetImageData(),
                     cover.GetWidth(),
                     cover.GetHeight(),
                     cover.GetFormat());
        QImage scaled_image = image.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(scaled_image));
    }

    connect(ui_->tableView, &QTableView::doubleClicked, this, &AlbumView::SongChosenForPlaySlot);
}

void AlbumView::SetTableAppearance(QTableView *table_view) {
    // This is actually songtablemodel appearance but whatever
    song_table_model_->SetHighlightColor(0xFF1DB954);

    // The actual table
    table_view->setShowGrid(false); // Hide the grid
    table_view->setSelectionBehavior(QAbstractItemView::SelectRows); // Select whole row
    //tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    table_view->setEditTriggers(QAbstractItemView::NoEditTriggers); // Disable editing
    table_view->verticalHeader()->setVisible(false); // Hide row numbers
    table_view->horizontalHeader()->setSectionsMovable(false); // Prevent column dragging
    table_view->horizontalHeader()->setHighlightSections(false); // Remove column click highlight

    // --- Column Widths & Alignment ---
    table_view->setColumnWidth(0, 40); // Track Number (Fixed width)
    table_view->setColumnWidth(2, 80); // Duration (Fixed width)
    table_view->horizontalHeader()->setStretchLastSection(false);
    table_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Song Name column expands

    // --- Apply Theme ---
    table_view->setStyleSheet(
        "QTableView {"
        "   background-color: #121212;"
        "   color: white;" // White text
        "   selection-background-color: #333333;" // Dark Gray when row is selected
        "   gridline-color: #121212;"
        " 	outline: none;"
        "} "
        "QTableView::item {"
        "   background-color: #121212;" // Ensure all rows are uniform dark color
        "   padding: 5px;"
        "} "
        "QTableView::item:selected {"
        "   background-color: #333333;" // Row selection color
        "   color: white;"
        "} "
        "QTableView::item:focus {"
        "   outline: 0;"
        "	background: none;"
        "   border: none;" // Removes dashed border on click
        "}"
        "QTableView::item:!selected:focus {"
        "	background:transparent;"
        "}"
        "QTableView::item:selected:focus { background:#333333; }"
        "QHeaderView::section {		"
        "background-color: #121212;		"
        "color: lightgray;		"
        "font-weight: normal;		"
        "padding: 5px;		"
        "border: 1px solid #292929;	"
        "}	"
    );

}

void AlbumView::SongChosenForPlaySlot(const QModelIndex &q_index) {
    if (!q_index.isValid()) {
        return;
    }
    unsigned row = q_index.row();

    emit SongChosenForPlaySignal(album_.GetSongs(), row);
}

// Takes a song that is playing
void AlbumView::InformSongPlaying(const Song &song) {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetSongAsPlaying(song.GetHash());

}

void AlbumView::InformSongNotPlaying() {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetPlayingAsStopped();
}

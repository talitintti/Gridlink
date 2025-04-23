#include "playlistadddialog.h"

#include "playlistadddialog.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

PlaylistAddDialog::PlaylistAddDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("New Playlist");

    auto *layout = new QVBoxLayout(this);

    // Label and Line Edit
    layout->addWidget(new QLabel("Enter playlist name:"));

    lineedit_name_ = new QLineEdit(this);
    layout->addWidget(lineedit_name_);

    // Buttons
    auto *buttonLayout = new QHBoxLayout();

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(buttonLayout);

    // Connect buttons
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString PlaylistAddDialog::GetPlaylistName() {
    return lineedit_name_->text().trimmed();
}

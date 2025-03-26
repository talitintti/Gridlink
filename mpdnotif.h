#ifndef MPDNOTIF_H
#define MPDNOTIF_H


#include <QObject>
#include <QThread>
#include <mpd/client.h>
#include <QDebug>
#include <QSocketNotifier>


class MPDNotif : public QObject {
    Q_OBJECT

public:
    explicit MPDNotif(QObject *parent = nullptr, const char *hostname = NULL, unsigned port = 0, unsigned timeout = 0) : QObject(parent) {
        conn_ = mpd_connection_new(hostname, port, timeout);
        if (!conn_ || mpd_connection_get_error(conn_) != MPD_ERROR_SUCCESS) {
            qWarning() << "Failed to connect to MPD\n";
            return;
        }

        mpd_fd_ = mpd_connection_get_fd(conn_);
        notifier_ = new QSocketNotifier(mpd_fd_, QSocketNotifier::Read, this);
        connect(notifier_, &QSocketNotifier::activated, this, &MPDNotif::handleMPDEvent);

        mpd_send_idle(conn_);  // Enter idle mode
    }

    ~MPDNotif() {
        mpd_connection_free(conn_);
    }

signals:
    void PlayerStateChanged(mpd_idle);

private slots:
    void handleMPDEvent() {
        notifier_->setEnabled(false);

        mpd_idle events = mpd_recv_idle(conn_, true);
        emit PlayerStateChanged(events);

        mpd_send_idle(conn_);  // Re-enter idle mode
        notifier_->setEnabled(true);
    }

private:
    mpd_connection *conn_;
    int mpd_fd_;
    QSocketNotifier *notifier_;
};

#endif // MPDNOTIF_H

#ifndef PROGRESSBARWIDGET_H
#define PROGRESSBARWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>

class ProgressBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProgressBarWidget(QWidget *parent = nullptr);

    bool IsOn();
    void SetLength(unsigned seconds);  // Set total song duration in seconds
    void SetPosition(unsigned seconds);    // Update progress bar with current time
    void HandlePlay();
    void HandlePause();
    void HandleStop();
    void Renew(unsigned length_seconds); //HandleStop, set new length, HandlePlay

    void SetUpdateInterval(unsigned interval_ms);

private:
    QSlider *progress_bar_;
    QTimer *timer_;
    QLabel *current_time_lab_;
    QLabel *total_time_lab_;
    QHBoxLayout *layout_;

    unsigned total_length_ms_;  // Total duration of song in seconds
    unsigned current_time_ms_; // Current position in seconds

    unsigned update_interval_ms_;

    void UpdateProgress();
    void UpdateCurrentTime(unsigned ms);
    void UpdateTotalTime(unsigned ms);
};

#endif // PROGRESSBARWIDGET_H

#include "progressbarwidget.h"
#include "helpers.h"

ProgressBarWidget::ProgressBarWidget(QWidget *parent)
    : QWidget(parent), total_length_ms_(0), current_time_ms_(0), update_interval_ms_(1000){


    // Initialize UI elements
    progress_bar_ = new QSlider(Qt::Horizontal, this);
    current_time_lab_ = new QLabel(this);
    total_time_lab_ = new QLabel(this);

    layout_ = new QHBoxLayout(this);
    layout_->addWidget(current_time_lab_);
    layout_->addWidget(progress_bar_);
    layout_->addWidget(total_time_lab_);
    setLayout(layout_);

    // Timer for auto-progressing the bar
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &ProgressBarWidget::UpdateProgress);

    SetUpdateInterval(update_interval_ms_); // Keeps tick step in once place
    UpdateCurrentTime(0);
    UpdateTotalTime(0);
}

void ProgressBarWidget::SetLength(unsigned seconds) {
    UpdateTotalTime(seconds);
}

void ProgressBarWidget::SetPosition(unsigned ms) {
    if (ms > total_length_ms_) return;
    UpdateCurrentTime(ms);
}

void ProgressBarWidget::UpdateProgress() {
    if (current_time_ms_ < total_length_ms_) {
        UpdateCurrentTime(current_time_ms_ + update_interval_ms_);
    } else {
        timer_->stop();
    }
}

void ProgressBarWidget::Renew(unsigned length_seconds) {
    HandleStop();
    UpdateTotalTime(length_seconds);
    HandlePlay();
}

void ProgressBarWidget::HandlePause() {
    timer_->stop();
}

void ProgressBarWidget::HandlePlay() {
    timer_->start(update_interval_ms_);
}

void ProgressBarWidget::HandleStop() {
    timer_->stop();
    UpdateCurrentTime(0);
}

void ProgressBarWidget::UpdateCurrentTime(unsigned current_time_ms) {
    current_time_ms_ = current_time_ms;
    current_time_lab_->setText(FormattedTime(current_time_ms));
    progress_bar_->setValue(current_time_ms);
}

void ProgressBarWidget::UpdateTotalTime(unsigned seconds) {
    total_length_ms_ = seconds * 1000;
    total_time_lab_->setText(FormattedTime(seconds));
    progress_bar_->setRange(0, total_length_ms_);
}


void ProgressBarWidget::SetUpdateInterval(unsigned update_interval_ms) {
    if (update_interval_ms == 0) return;
    update_interval_ms_ = update_interval_ms;
    progress_bar_->setSingleStep(update_interval_ms_);
}

bool ProgressBarWidget::IsOn() {
    return timer_->isActive();
}



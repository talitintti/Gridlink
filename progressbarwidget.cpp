#include "progressbarwidget.h"
#include "helpers.h"
#include <QApplication>

ProgressBarWidget::ProgressBarWidget(QWidget *parent)
    : QWidget(parent), total_length_ms_(0), current_time_ms_(0), update_interval_ms_(1000){


    // Initialize UI elements
    slider_ = new QSlider(Qt::Horizontal, this);
    current_time_lab_ = new QLabel(this);
    total_time_lab_ = new QLabel(this);

    layout_ = new QHBoxLayout(this);
    layout_->addWidget(current_time_lab_);
    layout_->addWidget(slider_);
    layout_->addWidget(total_time_lab_);
    setLayout(layout_);

    // Timer for auto-progressing the bar
    timer_ = new QTimer(this);

    SetUpdateInterval(update_interval_ms_); // Keeps tick step in once place
    UpdateCurrentTime(0);
    UpdateTotalTime(0);

    connect(timer_, &QTimer::timeout, this, &ProgressBarWidget::UpdateProgress);
    connect(slider_, &QSlider::actionTriggered, this, &ProgressBarWidget::UserClickedOnSlider);
    connect(slider_, &QSlider::sliderMoved, this, &ProgressBarWidget::SliderDragged);
    connect(slider_, &QSlider::sliderReleased, this, &ProgressBarWidget::UserReleasedDrag);
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
    unsigned current_time_sec = round(current_time_ms / 1000.0);
    current_time_lab_->setText(FormattedTime(current_time_sec));
    slider_->setValue(current_time_ms);
}

void ProgressBarWidget::UpdateTotalTime(unsigned seconds) {
    total_length_ms_ = seconds * 1000;
    total_time_lab_->setText(FormattedTime(seconds));
    slider_->setRange(0, total_length_ms_);
}


void ProgressBarWidget::SetUpdateInterval(unsigned update_interval_ms) {
    if (update_interval_ms == 0) return;
    update_interval_ms_ = update_interval_ms;
    slider_->setSingleStep(update_interval_ms_);
}

bool ProgressBarWidget::IsOn() {
    return timer_->isActive();
}

void ProgressBarWidget::UserClickedOnSlider(int action) {
if (action == QAbstractSlider::SliderPageStepAdd ||
    action == QAbstractSlider::SliderPageStepSub)
    {
        QPoint local_mouse_pos = slider_->mapFromGlobal(QCursor::pos());
        float pos_ratio = local_mouse_pos.x() / (float)slider_->size().width();
        int slider_range = slider_->maximum() - slider_->minimum();
        int clicked_slider_pos = slider_->minimum() + slider_range * pos_ratio;
        qDebug() << clicked_slider_pos;
        UpdateCurrentTime(clicked_slider_pos);
        emit PositionChanged(current_time_ms_);
    }
}

void ProgressBarWidget::SliderDragged(int value) {
    temp_drag_value_ = value;
}

void ProgressBarWidget::UserReleasedDrag() {
    emit PositionChanged(temp_drag_value_);
}

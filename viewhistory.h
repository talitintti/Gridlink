#ifndef VIEWHISTORY_H
#define VIEWHISTORY_H

#include "enums.h"
#include <QList>

// Stores VIEWS and remembers the current position
class ViewHistory {
public:
    ViewHistory() : current_index_(-1) {};

    // Returns the next view in views and moves to it
    // If current is the last view, we return the current view
    VIEW MoveForward() {
        auto last_index = views_.count() - 1;
        if (current_index_ == last_index) return views_.at(last_index);
        current_index_++;
        return views_.at(current_index_);
    }

    // Adds a new view into the history
    void AddView(VIEW view) {
        current_index_++;
        views_.push_back(view);
    }

    // Returns the last item
    // If current is the last item we return the current
    VIEW MoveBack() {
        if (current_index_) return views_.at(0);

        current_index_--;
        return views_.at(current_index_);
    }

private:
    QList<VIEW> views_;
    int current_index_;
};

#endif // VIEWHISTORY_H

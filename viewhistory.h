#ifndef VIEWHISTORY_H
#define VIEWHISTORY_H

#include "enums.h"
#include <QList>
#include <QDebug>
#include <any>
#include "playlist.h"

// Stores VIEWS and correspondign data and remembers the current position
class ViewHistory {
public:
    ViewHistory() : current_index_(-1) {};

    // Returns the next view in views and moves to it
    // If current is the last view, we return the current view
    const std::tuple<VIEW, std::any> &MoveForward() {
        auto last_index = views_.count() - 1;
        if (last_index == -1) return unkonwn_;
        if (current_index_ == last_index) return views_.at(last_index);
        current_index_++;
        return views_.at(current_index_);
    }

    // Adds a new view into the history
    // if you store data you have to wrap it in a QStoredPointer, otherwise give empty
    void AddView(VIEW view, std::any &&data) {
        if (current_index_ == 0) {
            auto temp = views_.at(0);
            views_.clear();
            views_.append(temp);
        }
        current_index_++;
        views_.emplace_back(view, std::move(data));
    }

    // Returns the last item
    // If current is the last item we return the current
    const std::tuple<VIEW, std::any> &MoveBack() {
        if (current_index_ == -1) return unkonwn_;
        if (!current_index_) return views_.at(0);

        current_index_--;
        return views_.at(current_index_);
    }

    // Removes all playlist views from history with given hash
    // True means that something was removed
    bool Remove(size_t data_hash) {
        bool smth_rmvd = false;
        size_t i = 0;
        while (i < views_.count()) {
            auto tuple = views_.at(i);
            if (std::get<0>(tuple) == VIEW::PLAYLIST
                && std::any_cast<QSharedPointer<Playlist>>(std::get<1>(tuple))->GetHash() == data_hash) { // TODO:This could be uuhhhhh prettified
                views_.remove(i);
                smth_rmvd = true;
                current_index_--;
            }
            else {
                i++;
            }
        }
        return smth_rmvd;
    }

    const std::tuple<VIEW, std::any> &MoveToFirstAndCleanOthers() {
        if (current_index_ == -1) return unkonwn_;
        size_t remove_n = views_.count() - 1;
        views_.remove(1, remove_n);
        return views_.at(0);
    }

private:
    QList<std::tuple<VIEW, std::any>> views_;
    int current_index_;
    const std::tuple<VIEW, std::any> unkonwn_ = {VIEW::UNKNOWN, std::any()};
};

#endif // VIEWHISTORY_H

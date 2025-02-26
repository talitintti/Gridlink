#ifndef HELPERS_H
#define HELPERS_H

#include <QString>


// Provides time formatted as
// hours >= 1 "h:mm:ss"
// hours < 1 "m:ss"
inline QString FormattedTime(unsigned time_sec) {
    unsigned hours = time_sec / 3600;
    unsigned minutes = time_sec / 60;
    unsigned seconds = time_sec % 60;

    if (hours >= 1) {
        minutes = (time_sec - (hours * 3600)) / 60;
    }

    if (hours > 0) {
        return QString::asprintf("%u:%02u:%02u", hours, minutes, seconds);
    } else {
        return QString::asprintf("%u:%02u", minutes, seconds);
    }

}


#endif // HELPERS_H

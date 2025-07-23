#include <QString>

struct ShoutPayload {
    bool iPlayerState;
    int iPlayerPosMS;
    int iDuration;
    int tYear;
    QString tFileLocation;
    QString tName;
    QString tAlbum;
    QString tArtist;
    QString tGenre;
    QString tDateSkipped;
    QString tDatePlayed;
};
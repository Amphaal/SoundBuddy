#ifdef APPLE

#include <unistd.h>
#include <QProcess>

#include <rapidjson/document.h>

#include "src/workers/shout/ShoutThread.h"
#include "src/helpers/stringHelper/stringHelper.hpp"

void ShoutThread::run() {
    //
    this->_inst();

    emit printLog(tr("Waiting for iTunes to launch..."));

    // define applescript to get shout values
    Q_INIT_RESOURCE(resources);
    const auto scriptContent = QFile(":/mac/CurrentlyPlaying.applescript").readAll();

    // prepare script exec
    QProcess p;
    p.setProgram("/usr/bin/osascript");
    p.setArguments({ "-l", "AppleScript", "-s", "s"});
    p.write(scriptContent);
    p.closeWriteChannel();

    // loop until user said not to
    while (this->_mustListen) {
        // get shout results
        p.start();
        p.waitForReadyRead();
        auto result = p.readAll();
        p.waitForFinished();

        // default values and inst
        QString tName;
        QString tAlbum;
        QString tArtist;
        QString tGenre;
        int iDuration;
        int iPlayerPos;
        bool iPlayerState = false;
        // bool iRepeatMode;
        QString tDateSkipped;
        QString tDatePlayed;
        int tYear;

        // if has result
        if (result.size()) {
            // turn results into array
            result[0] = '[';
            result[result.size() - 1] = ']';

            // cast to json
            rapidjson::Document trackObj;
            trackObj.Parse(result.data());

            // get values for shout
            tName = trackObj[0].GetString();
            tAlbum = trackObj[1].GetString();
            tArtist = trackObj[2].GetString();
            tGenre = trackObj[3].GetString();
            iDuration = trackObj[4].GetInt();
            tYear = trackObj[5].GetInt();
            iPlayerPos = trackObj[6].GetInt();
            iPlayerState = trackObj[7].GetString() == "paused" ? 0 : 1;
            tDateSkipped = trackObj[8].GetString();
            tDatePlayed = trackObj[9].GetString();
        }

        // compare with old shout, if equivalent, don't reshout
        if (this->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
            // if had results
            if (result.size()) {
                // say track infos
                this->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
            } else {
                // say nothing happens
                this->shoutEmpty();
            }
        }

       // wait a bit before re-asking
       this->sleep(1);
    }

    this->shoutEmpty();
    emit printLog(tr("Stopped listening to iTunes."));
}
#endif

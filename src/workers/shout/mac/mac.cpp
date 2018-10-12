#include <unistd.h>
#include <QProcess>

#include "rapidjson/document.h"

#include "../shout.h" 
#include "../../../helpers/stringHelper.cpp"

void ShoutWorker::run() { 

    emit printLog("Listening to iTunes !");

    //define applescript to get shout values
    std::string script = 
    "tell application \"iTunes\" \n"
        "get the {name, album, artist, genre, duration} of the current track & {player position} & {player state} \n"
    "end tell";

    //prepare script exec
    QString osascript = "/usr/bin/osascript";
    QStringList processArguments;
    processArguments << "-l" << "AppleScript" << "-s" << "s";

    //hash
    size_t lastTrackHash;

    while (this->mustListen) {

        //get shout results
        QProcess p;
        p.start(osascript, processArguments);
        p.write(QString::fromStdString(script).toUtf8());
        p.closeWriteChannel();
        p.waitForReadyRead();
        auto result = p.readAll().toStdString();
        
        //default values and inst
        const std::string defaultHashComp = "Nothing";
        std::string hashComp = defaultHashComp;
        std::string tName;
        std::string tAlbum;
        std::string tArtist;
        std::string tGenre;
        int iDuration;
        int iPlayerPos;
        bool iPlayerState;

        //if has result
        if (result.size()) {
            
            //cast to array
            result = result.substr(0, result.size()-2);
            result = result.substr(1, result.size()-1);
            size_t found = result.find_last_of(", ");
            result = result.insert(found+1, "\"");
            result = result + "\"";
            result = "[" + result + "]";
            
            //cast to json
            rapidjson::Document trackObj;
            trackObj.Parse(result.c_str());
            
            //get values for shout
            tName = trackObj[0].GetString();
            tAlbum = trackObj[1].GetString();
            tArtist = trackObj[2].GetString();
            tGenre = trackObj[3].GetString();
            iDuration = trackObj[4].GetFloat();
            iPlayerPos = trackObj[5].GetFloat();
            iPlayerState = std::string(trackObj[6].GetString()) == "paused" ? 0 : 1;

            //alter hash component
            hashComp = StringHelper::boolToString(iPlayerState) + tName + tAlbum + tArtist;
        }

        //calculate hash
        size_t currHash = std::hash<std::string>{}(hashComp);

        if (lastTrackHash != currHash) {
            
            if(hashComp == defaultHashComp) this->shoutEmpty(); //if no result
            else this->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState);

            lastTrackHash = currHash;
        }

        //wait before retry
        this->sleep(1);

    }

    this->shoutEmpty();
    emit printLog("Stopped listening.");
}

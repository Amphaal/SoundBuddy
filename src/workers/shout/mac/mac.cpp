#ifdef __APPLE__

#include <unistd.h>
#include <QProcess>

#include <rapidjson/document.h>

#include "../shout.h" 
#include "../../../helpers/stringHelper.cpp"
#include "../../../localization/i18n.cpp"

void ShoutWorker::run() { 

    emit printLog(I18n::tr()->Shout_StartListening());

    //define applescript to get shout values
    std::string script = "tell application \"iTunes\" \n ";
    script += "if skipped date of current track is not missing value then \n "
              "set SkpDt to skipped date of current track as «class isot» as string \n "
              "else \n "
              "set SkpDt to \"\" \n "
              "end if \n ";
    script += "if played date of current track is not missing value then \n "
              "set PlyDt to played date of current track as «class isot» as string \n "
              "else \n "
              "set PlyDt to \"\" \n "
              "end if \n ";
    script += "get {name, album, artist, genre, duration} of current track & ";
    script += "player position & ";
    script += "(player state as string) & ";
    script += "SkpDt & ";
    script += "PlyDt ";
    script += "\n end tell";

    //prepare script exec
    QString osascript = "/usr/bin/osascript";
    QStringList processArguments;
    processArguments << "-l" << "AppleScript" << "-s" << "s";

    while (this->mustListen) {

        //get shout results
        QProcess p;
        p.start(osascript, processArguments);
        p.write(QString::fromStdString(script).toUtf8());
        p.closeWriteChannel();
        p.waitForReadyRead();
        auto result = p.readAll().toStdString();
        
        //default values and inst
        std::string tName;
        std::string tAlbum;
        std::string tArtist;
        std::string tGenre;
        int iDuration;
        int iPlayerPos;
        bool iPlayerState = false;
        //bool iRepeatMode;
        std::string tDateSkipped;
        std::string tDatePlayed;

        //if has result
        if (result.size()) {
            
            //turn results into array
            result = result.substr(0, result.size()-2);
            result = result.substr(1, result.size()-1);
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
            tDateSkipped = trackObj[7].GetString();
            tDatePlayed = trackObj[8].GetString();
        }

        //compare with old shout, if equivalent, don't reshout
        if(this->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
            if(result.size()) {
                //shout !
                this->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState);
            }
            else {
                this->shoutEmpty();
            }
        } 

        //wait before retry
        this->sleep(1);

    }

    this->shoutEmpty();
    emit printLog(I18n::tr()->Shout_StopListening());
}
#endif
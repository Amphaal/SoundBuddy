#include <unistd.h>

#include "../shout.h" 

void ShoutWorker::run() { 

    emit printLog("Listening to iTunes !");

    //define applescript to get shout values
    std::string script = 
    "tell application \"iTunes\""
        "get the {name, album, artist, genre, duration} of the current track & {player position} & {player state}"
    "end tell";

    //add script execution
    script = "osascript -e '" + script + "'";

    while (this->mustListen) {

        //get shout results
        char buffer[100];
        FILE * f = popen(script.c_str(), "r");
        fgets(buffer, 99, f);
        pclose(f);

        //wait before retry
        usleep(1000);
    }

    emit printLog("Stopped listening.");
}

 // $Id: //poco/1.4/Foundation/samples/Timer/src/Timer.cpp#1 $
 // This sample demonstrates the Timer and Stopwatch classes.
 // Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
 // and Contributors.
 // SPDX-License-Identifier:    BSL-1.0

 #include "Poco/Timer.h"
 #include "Poco/Thread.h"
 #include "Poco/Stopwatch.h"
 #include <iostream>
 #include <QApplication>

 using Poco::Timer;
 using Poco::TimerCallback;
 using Poco::Thread;
 using Poco::Stopwatch;

 class TimerExample{
 public:
     TimerExample(){ _sw.start();}

     void onTimer(Timer& timer){
         std::cout << "Callback called after " << _sw.elapsed()/1000 << " milliseconds." << std::endl;
     }
 private:
     Stopwatch _sw;
 };

 int main(int argc, char** argv){
    QApplication app(argc, argv);
 
    return app.exec();
 }
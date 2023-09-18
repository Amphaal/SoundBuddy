#include <functional>

class HeartbeatState {
   private:
        bool latestMessageIsError = false;
        bool cycledThroughLatestError = false;
        bool pongReceivedInPreviousCycle = false;
        bool pongMissedInPreviousCycle = false;
        bool reconnectionRegistered = false;

   public:
      //
      void basicMessage() {
         latestMessageIsError = false;
         cycledThroughLatestError = false;
      }

      //
      void errorHappened() {
         qDebug("MBeat: Error");
         latestMessageIsError = true;
         cycledThroughLatestError = false;
      }

      void registerReconnection() {
        qDebug("MBeat: Reconnection registered");
        reconnectionRegistered = true;
      }

    void resetAnyReconnectionRegistered() {
        reconnectionRegistered = false;
      }

      void pingOrReconnect(const std::function<void()>& goPing, const std::function<void()>& goReconnect) {
        if (reconnectionRegistered) {
            reconnectionRegistered = false;
            qDebug("MBeat: Reconnect...");
            goReconnect();
        } else {
            qDebug("MBeat: Ping...");
            goPing();
        }
      }

      //
      void ackPong(const std::function<void()>& goRecovery) {
         qDebug("MBeat: ...Pong.");
         //
         pongReceivedInPreviousCycle = true;

         //
         if (pongMissedInPreviousCycle) {
            qDebug("MBeat: Connected again, Recovering.");
            pongMissedInPreviousCycle = false;
            goRecovery();
         }
      }

      //
      void cycled(const std::function<void()>& goReconnecting, const std::function<void()>& goPing) {
            // whenever pong has been received or not...
            if(pongReceivedInPreviousCycle) {
               // reset pong flag
               pongReceivedInPreviousCycle = false;
               qDebug("MBeat: Ping...");
               goPing();
            } else {
               //
               // pong missed...
               //
               
               // if latest message wasnt an error, or if already cycled once on the error (so the user had time to see it)
               if (!latestMessageIsError || (latestMessageIsError && cycledThroughLatestError)) {
                  goReconnecting();
               } else {
                  qDebug("MBeat: Error shown 1 cycle, will be reset next.");
                  cycledThroughLatestError = true;
               }

               // ack that we missed at last 1 pong on cycle
               qDebug("MBeat: Pong missed...");
               pongMissedInPreviousCycle = true;
            }
      }
};

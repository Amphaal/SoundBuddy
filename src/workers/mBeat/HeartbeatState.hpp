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
         latestMessageIsError = true;
         cycledThroughLatestError = false;
      }

      void registerReconnection() {
        reconnectionRegistered = true;
      }

    void resetAnyReconnectionRegistered() {
        reconnectionRegistered = false;
      }

      void pongOrReconnect(const std::function<void()>& onPong, const std::function<void()>& onReconnect) {
        if (reconnectionRegistered) {
            reconnectionRegistered = false;
            onReconnect();
        } else {
            onPong();
        }
      }

      //
      void ackPong(const std::function<void()>& onRecovery) {
         //
         pongReceivedInPreviousCycle = true;

         //
         if (pongMissedInPreviousCycle) {
            pongMissedInPreviousCycle = false;
            onRecovery();
         }
      }

      //
      void cycled(const std::function<void()>& beReconnecting) {
            // whenever pong has been received or not...
            if(pongReceivedInPreviousCycle) {
               // reset pong flag
               pongReceivedInPreviousCycle = false;
            } else {
               //
               // pong missed...
               //
               
               // if latest message wasnt an error, or if already cycled once on the error (so the user had time to see it)
               if (!latestMessageIsError || (latestMessageIsError && cycledThroughLatestError)) {
                  beReconnecting();
               } else {
                  cycledThroughLatestError = true;
               }

               // ack that we missed at last 1 pong on cycle
               pongMissedInPreviousCycle = true;
            }
      }
};

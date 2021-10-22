#pragma once

// this file exists because of "emit" keyword

#include <sio_client.h>

void _sio_checkCredentials(sio::client* sioClient, sio::message::list& p) {
   sioClient->socket("/login")->emit("checkCredentials", p);
}
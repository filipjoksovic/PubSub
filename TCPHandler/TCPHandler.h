#pragma once

#include <WS2tcpip.h>
#include "../Utility/Message.h"

bool Send(SOCKET socket, Message* m);

bool Send(SOCKET socket, char* c);

bool Receive(SOCKET socket, char* buf, size_t size);
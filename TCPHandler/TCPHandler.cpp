#include <WS2tcpip.h>
#include "../Utility/Message.h"
#include <iostream>

using namespace std;

bool Receive(SOCKET connectSocket, char* buf, size_t len) {
	int recRes = recv(connectSocket, (char*)buf, len, 0);

	if (recRes < 0) {
		cerr << "Error while receiving data: " << WSAGetLastError();
		return false;
	}
	return true;
}

bool Send(SOCKET connectSocket, Message m) {
	char* buf = (char*)malloc(sizeof(Message));
	if (buf > 0) {
		memcpy(buf, (void*)&m, sizeof(m));
		int sendRes = send(connectSocket, buf, sizeof(Message), 0);

		if (sendRes == SOCKET_ERROR) {
			cerr << "Error while sending data: " << WSAGetLastError();
			return false;
		}
		return true;
	}
}
bool Send(SOCKET connectSocket, char* k) {
	int sendRes = send(connectSocket, (char*)k, sizeof(char), 0);
	if (sendRes == SOCKET_ERROR) {
		cerr << "Error while sending data: " << WSAGetLastError();
		return false;
	}
	return true;
}

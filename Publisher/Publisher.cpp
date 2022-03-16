#include<WS2tcpip.h>
#include<winsock.h>
#include <windows.h>
#include<string>
#include <iostream>
#include <conio.h>
#include <time.h>

using namespace std;
#pragma comment(lib,"WS2_32")

#include "../Utility/Message.h";
#include "../TCPHandler/TCPHandler.h";
#include "../TCPHandler/TCPHandler.cpp";
#include "Publisher.h";

void main() {
	srand(time(NULL));
	int result = Init();
	if (!result) {
		return;
	}
	cout << "Connected as publisher" << endl;
	int picked = 0;
	int interval = 1000;
		cout << "> Pick a way of publishing your messages: 1) Randomly-generated 2) User-generated ";
	while (true) {
		cin >> picked;
		if (picked != 1 && picked != 2 && picked != 3) {
			cout << "> Invalid publishing type. Try again." << endl;
		}
		else if (picked == 1) {
			cout << "> Pick an interval of publishing(ms): " << endl;
			cin >> interval;
			break;
		}
		else {
			break;
		}
	}
	getchar();
	if (picked == 1 || picked == 2) {
		SendMessage(picked, interval);
	}
	else {
		for (int i = 0; i < 1000; i++) {
			SendMessage(1, interval);
		}
	}
	getchar();
}

#pragma once
#include <iostream>

using namespace std;

typedef enum Signal{Analog = 0, Status};
typedef enum Type{sec_A = 0, sec_V, fuse, breaker};
using namespace std;

typedef struct Message {
	Signal signal;
	Type type;
	int value;
};

const char* GetStringFromEnum(Signal t) {
	const char* values[] = { "Analog", "Status" };
	return values[t];
}

const char* GetStringFromEnum(Type t) {
	const char* values[] = {"sec_A","sec_V","fuse","breaker"};
	return values[t];
}

void PrintMessage(Message* m) {
	cout << "Measurment: " << GetStringFromEnum(m->signal) << " " << GetStringFromEnum(m->type) << " " << m->value <<endl;
}

int GetRandomTypeAnalog() {
	return rand() % 2;
}
int GetRandomTypeDigital() {
	return rand() % 2 + 2;
}
int GetRandomAnalogValue() {
	return rand() % 101;
}
int GetRandomStatusValue() {
	return rand() % 2;
}
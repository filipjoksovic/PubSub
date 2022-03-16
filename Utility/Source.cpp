#include <iostream>
#include "LinkedList.h";
#include "Message.h";
using namespace std;

void main() {
	LinkedList list;
	Message m;
	m.signal = Analog;
	m.type = sec_A;
	m.value = 100;
	cout << GetStringFromEnum(m.type);
}
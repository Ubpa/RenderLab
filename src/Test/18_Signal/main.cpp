#include <functional>
#include <iostream>

#include <3rdParty/lsignal.h>

using namespace lsignal;

class Sender {
public:
	Sender(int num) : num(num) { id = 10; }
	void FuncA(int) {}
	void FuncB(int) {}
	void FuncC(int) {}

	void SetNum(int num) {
		this->num = num;
		numChanged(num);
	}

	signal<void(int)> numChanged;
public:
	int id;
private:
	int num;
};

class Receiver {
public:
	void Repeater(int i) {
		printf("%d\n",i);
	}
};

void pow2(int num) {
	printf("%d * %d = %d\n", num, num, num*num);
}

int main()
{
	Sender sender(3);
	Receiver receiver;

	sender.numChanged.connect(&receiver, &Receiver::Repeater);
	sender.numChanged.connect([&](int) {
		printf("num change! id is %d\n", sender.id);
	});
	sender.numChanged.connect(pow2);

	sender.SetNum(10);

	return 0;
}

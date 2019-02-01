#include <CppUtil/Basic/Element.h>
#include <CppUtil/Basic/EleVisitor.h>

#include <iostream>
#include <string>

using namespace CppUtil::Basic;

class A : public Element {
	ELE_SETUP(A)
public:
	A(int n) :n(n) { }
	
	int n;
};

class B : public Element {
	ELE_SETUP(B)
public:
	B(int n) :n(n) { }

	int n;
};

class Vc : public EleVisitor {
	HEAP_OBJ_SETUP_SELF_DEL(Vc)
public:
	Vc(const std::string & name):name(name) {
		Reg(&Vc::VisitA);
		Reg(&Vc::VisitB);
	}
protected:
	virtual ~Vc() {
		UnReg(&Vc::VisitA);
		UnReg(&Vc::VisitB);
	}
private:
	void VisitA(A::Ptr a) { std::cout << "Vc " << name << ": a's n is " << a->n << std::endl; }
	void VisitB(B::Ptr b) { std::cout << "Vc " << name << ": b's n is " << b->n << std::endl; }
	std::string name;
};

class Vd : public EleVisitor {
	HEAP_OBJ_SETUP_SELF_DEL(Vd)
public:
	Vd(const std::string & name) :name(name) {
		Reg(&Vd::VisitA);
		Reg(&Vd::VisitB);
	}
protected:
	virtual ~Vd() {
		UnReg(&Vd::VisitA);
		UnReg(&Vd::VisitB);
	}
private:
	void VisitA(A::Ptr a) { std::cout << "Vd " << name << ": a's n is " << a->n << std::endl; }
	void VisitB(B::Ptr b) { std::cout << "Vd " << name << ": b's n is " << b->n << std::endl; }
	std::string name;
};

int main() {
	Element::Ptr eles[4] = { ToPtr(new A(1)) ,ToPtr(new A(2)),ToPtr(new B(3)),ToPtr(new B(4)) };
	EleVisitor::Ptr visitors[2] = { ToPtr(new Vc("v1")) ,ToPtr(new Vd("v2")) };

	const int elesNum = sizeof(eles) / sizeof(Element::Ptr);
	const int visitorsNum = sizeof(visitors) / sizeof(EleVisitor::Ptr);

	/*
	* Vc v1: a's n is 1
	* Vd v2: a's n is 1
	* Vc v1: a's n is 2
	* Vd v2: a's n is 2
	* Vc v1: b's n is 3
	* Vd v2: b's n is 3
	* Vc v1: b's n is 4
	* Vd v2: b's n is 4
	*/

	for (int i = 0; i < elesNum; i++) {
		for (int j = 0; j < visitorsNum; j++) {
			eles[i]->Accept(visitors[j]);
		}
	}

	return 0;
}
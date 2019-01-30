#include <CppUtil/Basic/Element.h>
#include <CppUtil/Basic/EleVisitor.h>

#include <iostream>
#include <string>

class A : public CppUtil::Basic::Element {
	ELE_SETUP(A)
public:
	A(int n) :n(n) { }
	
	int n;
};

class B : public CppUtil::Basic::Element {
	ELE_SETUP(B)
public:
	B(int n) :n(n) { }

	int n;
};

class V : public CppUtil::Basic::EleVisitor {
	HEAP_OBJ_SETUP_SELF_DELETE(V)
public:
	V(const std::string & name):name(name) {
		Reg(&V::VisitA);
		Reg(&V::VisitB);
	}
protected:
	virtual ~V() {
		UnReg(&V::VisitA);
		UnReg(&V::VisitB);
	}
private:
	void VisitA(A::Ptr a) { std::cout << name << ": a's n is " << a->n << std::endl; }
	void VisitB(B::Ptr b) { std::cout << name << ": b's n is " << b->n << std::endl; }
	std::string name;
};

int main() {
	A::Ptr a1 = ToPtr(new A(1));
	A::Ptr a2 = ToPtr(new A(2));
	B::Ptr b1 = ToPtr(new B(3));
	B::Ptr b2 = ToPtr(new B(4));
	V::Ptr v1 = ToPtr(new V("v1"));
	V::Ptr v2 = ToPtr(new V("v2"));

	a1->Accept(v1);
	a1->Accept(v2);
	a2->Accept(v1);
	a2->Accept(v2);

	b1->Accept(v1);
	b1->Accept(v2);
	b2->Accept(v1);
	b2->Accept(v2);

	return 0;
}
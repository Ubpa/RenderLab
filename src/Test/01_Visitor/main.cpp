#include <CppUtil/Basic/Element.h>
#include <CppUtil/Basic/Visitor.h>

#include <iostream>
#include <vector>
#include <string>

using namespace CppUtil::Basic;
using namespace std;

class A final : public Element<A> {
public:
	A(int n) :n(n) { }
public:
	static const Ptr<A> New(int n) {return CppUtil::Basic::New<A>(n); }

public:
	int n;
};

class B final : public Element<B> {
public:
	B(int n) :n(n) { }
public:
	static const Ptr<B> New(int n) { return CppUtil::Basic::New<B>(n); }
public:
	int n;
};

class Vc final : public Visitor<Vc> {
public:
	Vc(const std::string & name):name(name) {
		RegMemberFunc<A>(&Vc::Visit);
		RegMemberFunc<B>(&Vc::Visit);
	}

public:
	static const Ptr<Vc> New(const std::string & name) { return CppUtil::Basic::New<Vc>(name); }

private:
	void Visit(Ptr<A> a) { std::cout << "Vc " << name << ": a's n is " << a->n << std::endl; }
	void Visit(Ptr<B> b) { std::cout << "Vc " << name << ": b's n is " << b->n << std::endl; }

public:
	std::string name;
};

class Vd final : public Visitor<Vd> {
public:
	Vd(const std::string & name) :name(name) {
		RegMemberFunc<A>(&Vd::Visit);
		RegMemberFunc<B>(&Vd::Visit);
	}

public:
	static const Ptr<Vd> New(const std::string & name) { return CppUtil::Basic::New<Vd>(name); }

private:
	void Visit(Ptr<A> a) { std::cout << "Vd " << name << ": a's n is " << a->n << std::endl; }
	void Visit(Ptr<B> b) { std::cout << "Vd " << name << ": b's n is " << b->n << std::endl; }

public:
	std::string name;
};

int main() {
	vector<Ptr<ElementBase>> eles = { A::New(1) , A::New(2), B::New(3), B::New(4) };
	vector<Ptr<VisitorBase>> visitors = { Vc::New("v1") , Vd::New("v2") };

	auto visitorDynamic = VisitorDynamic::New();
	visitorDynamic->Reg([=](Ptr<A> a) {
		std::cout << "visitorDynamic : a's n is " << a->n << std::endl;
	});
	visitorDynamic->Reg([=](Ptr<B> b) {
		std::cout << "visitorDynamic : b's n is " << b->n << std::endl;
	});
	visitors.push_back(visitorDynamic);

	/*
	Vc v1: a's n is 1
	Vd v2: a's n is 1
	visitorDynamic : a's n is 1
	Vc v1: a's n is 2
	Vd v2: a's n is 2
	visitorDynamic : a's n is 2
	Vc v1: b's n is 3
	Vd v2: b's n is 3
	visitorDynamic : b's n is 3
	Vc v1: b's n is 4
	Vd v2: b's n is 4
	visitorDynamic : b's n is 4
	*/

	for (size_t i = 0; i < eles.size(); i++) {
		for (size_t j = 0; j < visitors.size(); j++) {
			eles[i]->Accept(visitors[j]);
		}
	}

	return 0;
}
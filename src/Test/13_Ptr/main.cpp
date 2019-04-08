#include <CppUtil/Basic/HeapObj.h>

#include <iostream>
#include <vector>
#include <string>

using namespace CppUtil::Basic;
using namespace std;

// 子类用模板定义，将 ImplT 传到 HeapObj 中
// 子类可直接用 Ptr, CPtr, WPtr, WCPtr
template<typename ImplT>
class Node: public HeapObj<ImplT> {
protected:// 构造函数 protected，无需写相应的 New 函数
	Node(Ptr parent = nullptr)
		: parent(parent) { }

protected:// 定义一个默认的虚析构函数
	virtual ~Node() = default;

public:
	const Ptr GetParent() { return parent.lock(); }
	const CPtr GetParent() const { return parent.lock(); }
	vector<Ptr> & GetChildren() { return children; }
	const vector<Ptr> & GetChildren() const { return children; }

protected:
	virtual void Init() override {
		if (!parent.expired())
			parent.lock()->children.push_back(This());
	}

private:
	WPtr parent;
	vector<Ptr> children;
};

class SObj final : public Node<SObj> {
public:// 由于 New 需要用到构造函数，所以要 public
	// 通过 New<SObj>(const string & name, SObj::Ptr parent = nullptr) 来创建
	SObj(const string & name, SObj::Ptr parent = nullptr)
		: name(name), Node<SObj>(parent) { }

public:
	// 通过写一个 static 方法，方便用户使用（利用了 IDE）
	static const Ptr New(const string & name, SObj::Ptr parent = nullptr) {
		return CppUtil::Basic::New<SObj>(name, parent);
	}

public:
	void Print(int depth = 0) const {
		for (int i = 0; i < depth; i++)
			cout << " ";

		cout << name << endl;
		for (const auto child : GetChildren())
			child->Print(depth + 1);
	}

private:
	virtual ~SObj() = default;// 防止堆对象的创建

private:
	string name;
};

int main() {
	const auto a = New<SObj>("A");
	const auto ab = New<SObj>("B", a);
	const auto ac = New<SObj>("C", a);
	const auto abd = SObj::New("D", ab);
	const auto ace = SObj::New("E", ac);

	a->Print();

	return 0;
}

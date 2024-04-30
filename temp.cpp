#include <iostream>
#include <memory>

using namespace std;

class Parent {
 public:
  void a() { cout << "Parent a" << endl; }

  virtual void b() { cout << "Parent b" << endl; }

  Parent() = default;
  Parent(const Parent& p) {
    cout << "Parent copy constructor" << endl;
  }

  Parent& operator=(const Parent& p) {
    cout << "Parent operator=" << endl;
    return *this;
  }

  // 복사 대입 연산자
  Parent& operator=(const Parent&& p) {
    cout << "Parent operator= &&" << endl;
    return *this;
  }

  int c = 1;
  int d = 2;
};

class Child : public Parent {
 public:
  void a() { cout << "Child a" << endl; }

  void b() { cout << "Child b" << endl; }

  int c = 3;
  int d = 4;
};

int main() {
  short result = 3;
  result ^= 1;
  cout << result << endl;
  result ^= 1;
  cout << result << endl;

  Child c = Child();
  Parent p = c;
  p.b();

  Child c2 = c;

  shared_ptr<Parent> p_sptr = make_shared<Child>();
  p_sptr->b();

  Parent* p_ptr = new Child();
  p_ptr->b();

  return 0;
}

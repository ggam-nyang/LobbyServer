#include <iostream>
#include <memory>

using namespace std;

class Mentor {
 public:
  Mentor() { cout << "Mentor 생성" << endl; }
  ~Mentor() { cout << "Mentor 소멸" << endl; }

  Mentor(const Mentor& mentor) { cout << "Mentor 복사 생성" << endl; }
    Mentor& operator=(const Mentor& mentor) {
        cout << "Mentor 대입 생성" << endl;
        return *this;
    }
};

void printMentor(shared_ptr<Mentor> mentor) {
  cout << "함수 내부: " << mentor.get() << endl;
  shared_ptr<Mentor> tempMentor = std::move(mentor);
  cout << "함수 내부: " << mentor.get() << endl;
}

int main() {
  shared_ptr<Mentor> mentor(new Mentor());
  cout << "함수 외부: " << mentor.get() << endl;
  printMentor(mentor);
  cout << "함수 외부: " << mentor.get() << endl;

  return 0;
}

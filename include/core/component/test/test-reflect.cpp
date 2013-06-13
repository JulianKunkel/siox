#include "reflect.cpp"

#include <iostream>
#include <string>
#include <map>

using namespace std;

class foo {
public:
  int x;
  string y;

  RTTI(foo, MEMBER(x) MEMBER(y))
};

class schuh {
public:
  long x;
  int y;

  RTTI(schuh, MEMBER(x) MEMBER(y))
};


int main() {
  foo * f = new foo();
  schuh * s = new schuh();
  f->x = 45;
  f->y = "testcase 1";

  s->x = 23;
  s->y = 402;
 
  cout << f->info().name() << endl;


  printf("type: %s\n", foo::info().name());
  printf("size: %ld\n", foo::info().size());
  for (size_t i = 0; i != foo::info().memberCount(); ++i) {
    printf("  %s: offset %ld  size %ld  type %s\n",
      f->info().members()[i].name, f->info().members()[i].offset,
      f->info().members()[i].type->size(), f->info().members()[i].type->name());
    
    if(f->info().members()[i].type->id() == typeid(int)){
	printf ("Int: %d\n", *f->info().members()[i].value<int>(f));
    }
    if(f->info().members()[i].type->id() == typeid(string)){
	cout << "String: " << *f->info().members()[i].value<string>(f) << endl;
    }
  }
  return 0;
}

#include <stdio.h>
 
// g++ -ggdb test-destructor-lib.cpp -O3 -fPIC -shared -o libdestructor-test.so

class Lib{
public:
  Lib(){
    printf ("Lib ()\n");
  }

  ~Lib(){
    printf ("Lib~ ()\n");
  }
};

static Lib obj;

static void begin () __attribute__((constructor));
static void end () __attribute__((destructor));
 
void begin ()
{
  printf ("beginLib ()\n");
}
 
void end ()
{
  printf ("endLib ()\n");
}


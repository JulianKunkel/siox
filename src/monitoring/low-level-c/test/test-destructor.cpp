#include <stdio.h>
 
// g++ -ggdb test-destructor.cpp -O3 -Wl,--no-as-needed -L. -ldestructor-test
// exec with: LD_LIBRARY_PATH=. ./a.out 

/*

Expected destructor behavior:
beginLib ()
Lib ()
begin ()
Destroy ()

main ()
Destroy~ ()
end ()
endLib ()
Lib~ ()


Behavior leading to issues:
Lib ()
beginLib ()
Destroy ()
begin ()

main ()
Destroy~ ()
end ()
Lib~ ()
endLib ()

*/ 

class Destroy{
public:
  Destroy(){
    printf ("Destroy ()\n");
  }

  ~Destroy(){
    printf ("Destroy~ ()\n");
  }
};

Destroy obj;

void begin () __attribute__((constructor));
void end () __attribute__((destructor));
 
int main ()
{
  printf ("main ()\n");
}
 
void begin ()
{
  printf ("begin ()\n");
}
 
void end ()
{
  printf ("end ()\n");
}


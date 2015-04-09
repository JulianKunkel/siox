#include <stdio.h>
 
// gcc -ggdb test-destructor.cpp -O3
/*

Expected destructor behavior:
begin ()
Destroy ()
main ()
Destroy~ ()
end ()

Behavior leading to issues:
Destroy ()
begin ()
main ()
Destroy~ ()
end ()
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


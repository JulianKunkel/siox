#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>
#include <assert.h>

GSList * get_activity_list();

int main(int argc, char const *argv[])
{

  int fd = open("tmp_posix_test", O_CREAT|O_RDWR);
  write(fd, "TEST", 5);
  close(fd);	

  GSList * list = get_activity_list();

  printf("%p\n", list);
  assert(list != NULL);

  for(GSList *__glist = list; __glist; __glist = __glist->next){
        char * item = __glist->data;  
	printf("%s \n", item);
  }

  return 0;
}

## -*- coding: utf-8 -*-


Options = {
  "Trace" : True
}


  # write can actually invoke a nested write from the tracing library, this must be prevented
before = {
  "write" :  "static int write_entered = 0;\n if (write_entered){ return (* static_write) (fd,buf,count);\n}"
	 }
beforeTracing = {               
      "write" :  "write_entered = 1;"
	 }

after = { "write" : "write_entered = 0;"}

attributes = {
  "read" : ["fd='%d' size='%lld' ret='%lld'", "fd, (long long int) count, (long long int) ret" ],
  "write" : ["fd='%d' size='%lld' ret='%lld'", "fd, (long long int) count, (long long int) ret" ],
  "open" : ["fd='%d' name='%s'", "ret, pathname" ],
  "open64" : ["fd='%d' name='%s'", "ret, pathname" ],
  "close" : ["fd='%d' ret='%d'", "fd, ret"],
  "unlink" : ["name='%s'", "name" ],
  "lseek" : ["fd='%d' offset='%llu' whence='%d'", "fd, (long long unsigned) offset, whence" ],
  "fseek" : ["fp='%p' offset='%llu'", "stream, (long long unsigned) off" ],
  "fopen" : ["fp='%p' name='%s'", "ret, filename" ],
  "freopen" : ["fp='%p' name='%s' stream='%p'", "ret, filename,stream" ],
  "fdopen" : ["fp='%p' fd='%d'", "ret, fd" ],
  "fclose" :  ["fp='%p' ret='%d'", "stream,ret" ],
  "fflush" :  ["fp='%p' ret='%d'", "stream, ret" ],
}

conditions = {
#  "read" : "fd != 1",
}

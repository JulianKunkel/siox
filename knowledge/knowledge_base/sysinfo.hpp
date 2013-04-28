#ifndef __SYSINFO_HPP__
#define __SYSINFO_HPP__


// We need to use the header files to obitain the informations.
// It is not ensured that the nessecary tools like ifconfig, lshw ...
// are installed. The libstatgrab can be used for some hints to fetch
// the needed data.
//  http://www.fortran-2000.com/ArnaudRecipes/hardwareinfo.html
//  sysinfo can be found here: http://www.s-gms.ms.edus.si/cgi-bin/man-cgi?sysinfo+2


#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

// Network interface information
struct Nic
{
  char name[10];
  char mac[18];
  enum medium;
  double * speed; // Speed of the interface (measured?)
};

// File system informations
struct Fs
{
  char uuid[100];
};

struct Node
{
  char hardwareuuid[100];
};

class sysinfo
{

public:
  sysinfo(arguments);
  ~sysinfo();

  struct Nic * getNetworkInterfaces();
  struct Fs  * getFileSystems();

private:
  struct Nic * network_interfaces;
  struct Fs  * filesystems;
};

#endif

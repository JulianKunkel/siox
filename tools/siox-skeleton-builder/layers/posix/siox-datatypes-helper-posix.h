#ifndef SIOX_DATATYPES_HELPER_LOW_LEVEL_IO_H_JFTT3HM5
#define SIOX_DATATYPES_HELPER_LOW_LEVEL_IO_H_JFTT3HM5

// Generic flags for file open modes
// apply to: open, creat, dup3,
// TODO: mapping to bits.. 1,2,4,8,16..
enum POSIXOpenFileFlags {
	/* as defined by fcntl.h */
	SIOX_LOW_LEVEL_O_RDONLY=0,
	SIOX_LOW_LEVEL_O_WRONLY=1,
	SIOX_LOW_LEVEL_O_RDWR=2,
	SIOX_LOW_LEVEL_O_APPEND=1024,
	SIOX_LOW_LEVEL_O_ASYNC=8192,
	SIOX_LOW_LEVEL_O_CLOEXEC=524288,
	SIOX_LOW_LEVEL_O_CREAT=64,
	SIOX_LOW_LEVEL_O_DIRECTORY=65536,
	SIOX_LOW_LEVEL_O_EXCL=128,
	SIOX_LOW_LEVEL_O_NOCTTY=256,
	SIOX_LOW_LEVEL_O_NOFOLLOW=131072,
	SIOX_LOW_LEVEL_O_NONBLOCK=2048,
	SIOX_LOW_LEVEL_O_TRUNC=512,

	/* as from manpage open(2), bits are arbitrary
	SIOX_POSIX_MODE_O_RDONLY=0,
	SIOX_POSIX_MODE_O_WRONLY=1,
	SIOX_POSIX_MODE_O_RDWR=2,
	SIOX_POSIX_MODE_O_APPEND=4,
	SIOX_POSIX_MODE_O_ASYNC=8,
	SIOX_POSIX_MODE_O_CLOEXEC=16,
	SIOX_POSIX_MODE_O_CREAT=32,
	SIOX_POSIX_MODE_O_DIRECT=64,
	SIOX_POSIX_MODE_O_DIRECTORY=128,
	SIOX_POSIX_MODE_O_EXCL=256,
	SIOX_POSIX_MODE_O_LARGEFILE=512,
	SIOX_POSIX_MODE_O_NOATIME=,
	SIOX_POSIX_MODE_O_NOCTTY,
	SIOX_POSIX_MODE_O_NOFOLLOW,
	SIOX_POSIX_MODE_O_NONBLOCK,  // or O_NDELAY
	SIOX_POSIX_MODE_O_PATH,
	SIOX_POSIX_MODE_O_TRUNC
	*/
};

enum POSIXError{
SIOX_ESUCCESS = 0 /* should not happen*/
// definitions copied from errno.h, may need extension...
, SIOX_EPERM	=	 1	/* Operation not permitted */
, SIOX_ENOENT =		 2	/* No such file or directory */
, SIOX_ESRCH =		 3	/* No such process */
, SIOX_EINTR =		 4	/* Interrupted system call */
, SIOX_EIO =		 5	/* I/O error */
, SIOX_ENXIO =		 6	/* No such device or address */
, SIOX_E2BIG =		 7	/* Argument list too long */
, SIOX_ENOEXEC =		 8	/* Exec format error */
, SIOX_EBADF =		 9	/* Bad file number */
, SIOX_ECHILD =		10	/* No child processes */
, SIOX_EAGAIN =		11	/* Try again */
, SIOX_ENOMEM =		12	/* Out of memory */
, SIOX_EACCES =		13	/* Permission denied */
, SIOX_EFAULT =		14	/* Bad address */
, SIOX_ENOTBLK =		15	/* Block device required */
, SIOX_EBUSY =		16	/* Device or resource busy */
, SIOX_EEXIST =		17	/* File exists */
, SIOX_EXDEV =		18	/* Cross-device link */
, SIOX_ENODEV =		19	/* No such device */
, SIOX_ENOTDIR =		20	/* Not a directory */
, SIOX_EISDIR =		21	/* Is a directory */
, SIOX_EINVAL =		22	/* Invalid argument */
, SIOX_ENFILE =		23	/* File table overflow */
, SIOX_EMFILE =		24	/* Too many open files */
, SIOX_ENOTTY =		25	/* Not a typewriter */
, SIOX_ETXTBSY =		26	/* Text file busy */
, SIOX_EFBIG =		27	/* File too large */
, SIOX_ENOSPC =		28	/* No space left on device */
, SIOX_ESPIPE =		29	/* Illegal seek */
, SIOX_EROFS =		30	/* Read-only file system */
, SIOX_EMLINK =		31	/* Too many links */
, SIOX_EPIPE =		32	/* Broken pipe */
, SIOX_EDOM =		33	/* Math argument out of domain of func */
, SIOX_ERANGE =		34	/* Math result not representable */
, SIOX_EDEADLK =		35	/* Resource deadlock would occur */
, SIOX_ENAMETOOLONG =	36	/* File name too long */
, SIOX_ENOLCK =		37	/* No record locks available */
, SIOX_ENOSYS =		38	/* Function not implemented */
, SIOX_ENOTEMPTY =	39	/* Directory not empty */
, SIOX_ELOOP =		40	/* Too many symbolic links encountered */
, SIOX_EWOULDBLOCK =	41	/* Operation would block */
, SIOX_ENOMSG =		42	/* No message of desired type */
, SIOX_EIDRM =		43	/* Identifier removed */
, SIOX_ECHRNG =		44	/* Channel number out of range */
, SIOX_EL2NSYNC =	45	/* Level 2 not synchronized */
, SIOX_EL3HLT =		46	/* Level 3 halted */
, SIOX_EL3RST =		47	/* Level 3 reset */
, SIOX_ELNRNG =		48	/* Link number out of range */
, SIOX_EUNATCH =		49	/* Protocol driver not attached */
, SIOX_ENOCSI =		50	/* No CSI structure available */
, SIOX_EL2HLT =		51	/* Level 2 halted */
, SIOX_EBADE =		52	/* Invalid exchange */
, SIOX_EBADR =		53	/* Invalid request descriptor */
, SIOX_EXFULL =		54	/* Exchange full */
, SIOX_ENOANO =		55	/* No anode */
, SIOX_EBADRQC =		56	/* Invalid request code */
, SIOX_EBADSLT =		57	/* Invalid slot */
, SIOX_EDEADLOCK =	58
, SIOX_EBFONT =		59	/* Bad font file format */
, SIOX_ENOSTR =		60	/* Device not a stream */
, SIOX_ENODATA =		61	/* No data available */
, SIOX_ETIME =		62	/* Timer expired */
, SIOX_ENOSR =		63	/* Out of streams resources */
, SIOX_ENONET =		64	/* Machine is not on the network */
, SIOX_ENOPKG =		65	/* Package not installed */
, SIOX_EREMOTE =		66	/* Object is remote */
, SIOX_ENOLINK =		67	/* Link has been severed */
, SIOX_EADV =		68	/* Advertise error */
, SIOX_ESRMNT =		69	/* Srmount error */
, SIOX_ECOMM =		70	/* Communication error on send */
, SIOX_EPROTO =		71	/* Protocol error */
, SIOX_EMULTIHOP =	72	/* Multihop attempted */
, SIOX_EDOTDOT =		73	/* RFS specific error */
, SIOX_EBADMSG =		74	/* Not a data message */
, SIOX_EOVERFLOW =	75	/* Value too large for defined data type */
, SIOX_ENOTUNIQ =	76	/* Name not unique on network */
, SIOX_EBADFD =		77	/* File descriptor in bad state */
, SIOX_EREMCHG =		78	/* Remote address changed */
, SIOX_ELIBACC =		79	/* Can not access a needed shared library */
, SIOX_ELIBBAD =		80	/* Accessing a corrupted shared library */
, SIOX_ELIBSCN =		81	/* .lib section in a.out corrupted */
, SIOX_ELIBMAX =		82	/* Attempting to link in too many shared libraries */
, SIOX_ELIBEXEC =	83	/* Cannot exec a shared library directly */
, SIOX_EILSEQ =		84	/* Illegal byte sequence */
, SIOX_ERESTART =	85	/* Interrupted system call should be restarted */
, SIOX_ESTRPIPE =	86	/* Streams pipe error */
, SIOX_EUSERS =		87	/* Too many users */
, SIOX_ENOTSOCK =	88	/* Socket operation on non-socket */
, SIOX_EDESTADDRREQ =	89	/* Destination address required */
, SIOX_EMSGSIZE =	90	/* Message too long */
, SIOX_EPROTOTYPE =	91	/* Protocol wrong type for socket */
, SIOX_ENOPROTOOPT =	92	/* Protocol not available */
, SIOX_EPROTONOSUPPORT =	93	/* Protocol not supported */
, SIOX_ESOCKTNOSUPPORT =	94	/* Socket type not supported */
, SIOX_EOPNOTSUPP =	95	/* Operation not supported on transport endpoint */
, SIOX_EPFNOSUPPORT =	96	/* Protocol family not supported */
, SIOX_EAFNOSUPPORT =	97	/* Address family not supported by protocol */
, SIOX_EADDRINUSE =	98	/* Address already in use */
, SIOX_EADDRNOTAVAIL =	99	/* Cannot assign requested address */
, SIOX_ENETDOWN =	100	/* Network is down */
, SIOX_ENETUNREACH =	101	/* Network is unreachable */
, SIOX_ENETRESET =	102	/* Network dropped connection because of reset */
, SIOX_ECONNABORTED =	103	/* Software caused connection abort */
, SIOX_ECONNRESET =	104	/* Connection reset by peer */
, SIOX_ENOBUFS =		105	/* No buffer space available */
, SIOX_EISCONN =		106	/* Transport endpoint is already connected */
, SIOX_ENOTCONN =	107	/* Transport endpoint is not connected */
, SIOX_ESHUTDOWN =	108	/* Cannot send after transport endpoint shutdown */
, SIOX_ETOOMANYREFS =	109	/* Too many references: cannot splice */
, SIOX_ETIMEDOUT =	110	/* Connection timed out */
, SIOX_ECONNREFUSED =	111	/* Connection refused */
, SIOX_EHOSTDOWN =	112	/* Host is down */
, SIOX_EHOSTUNREACH =	113	/* No route to host */
, SIOX_EALREADY =	114	/* Operation already in progress */
, SIOX_EINPROGRESS =	115	/* Operation now in progress */
, SIOX_ESTALE =		116	/* Stale file handle */
, SIOX_EUCLEAN =		117	/* Structure needs cleaning */
, SIOX_ENOTNAM =		118	/* Not a XENIX named type file */
, SIOX_ENAVAIL =		119	/* No XENIX semaphores available */
, SIOX_EISNAM =		120	/* Is a named type file */
, SIOX_EREMOTEIO =	121	/* Remote I/O error */
, SIOX_EDQUOT =		122	/* Quota exceeded */
, SIOX_ENOMEDIUM =	123	/* No medium found */
, SIOX_EMEDIUMTYPE =	124	/* Wrong medium type */
, SIOX_ECANCELED =	125	/* Operation Canceled */
, SIOX_ENOKEY =		126	/* Required key not available */
, SIOX_EKEYEXPIRED =	127	/* Key has expired */
, SIOX_EKEYREVOKED =	128	/* Key has been revoked */
, SIOX_EKEYREJECTED =	129	/* Key was rejected by service */
, SIOX_EOWNERDEAD =	130	/* Owner died */
, SIOX_ENOTRECOVERABLE =	131	/* State not recoverable */
, SIOX_ERFKILL =		132	/* Operation not possible due to RF-kill */
, SIOX_EHWPOISON =	133	/* Memory page has hardware error */

, SIOX_ERR_UNKNOWN = 4000000000
};

#endif
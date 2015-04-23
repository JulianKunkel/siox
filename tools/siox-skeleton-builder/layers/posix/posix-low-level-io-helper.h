#include "siox-datatypes-helper-posix.h"

#include <errno.h>

#include <sys/types.h>
#include <unistd.h>
#include <glib.h>
#include <fcntl.h>

/* Functions that need helpers:
 *
 * readv
 * writev
 *
 */
static inline unsigned translatePOSIXFlagsToSIOX( unsigned flags )
{
	return ( ( flags & O_RDONLY ) > 0 ? SIOX_LOW_LEVEL_O_RDONLY : 0 )
		| ( ( flags & O_WRONLY ) > 0 ? SIOX_LOW_LEVEL_O_WRONLY : 0 )
		| ( ( flags & O_RDWR ) > 0 ? SIOX_LOW_LEVEL_O_RDWR : 0 )
		| ( ( flags & O_APPEND ) > 0 ? SIOX_LOW_LEVEL_O_APPEND : 0 )
		| ( ( flags & O_ASYNC ) > 0 ? SIOX_LOW_LEVEL_O_ASYNC : 0 )
		| ( ( flags & O_CLOEXEC ) > 0 ? SIOX_LOW_LEVEL_O_CLOEXEC : 0 )
		| ( ( flags & O_CREAT ) > 0 ? SIOX_LOW_LEVEL_O_CREAT : 0 )
		| ( ( flags & O_DIRECTORY ) > 0 ? SIOX_LOW_LEVEL_O_DIRECTORY : 0 )
		| ( ( flags & O_EXCL ) > 0 ? SIOX_LOW_LEVEL_O_EXCL : 0 )
		| ( ( flags & O_NOCTTY ) > 0 ? SIOX_LOW_LEVEL_O_NOCTTY : 0 )
		| ( ( flags & O_NOFOLLOW ) > 0 ? SIOX_LOW_LEVEL_O_NOFOLLOW : 0 )
		| ( ( flags & O_NONBLOCK ) > 0 ? SIOX_LOW_LEVEL_O_NONBLOCK : 0 )
		| ( ( flags & O_TRUNC ) > 0 ? SIOX_LOW_LEVEL_O_TRUNC : 0 );
}

static inline unsigned translateSIOXFlagsToPOSIX( unsigned flags )
{
	return ( ( flags & SIOX_LOW_LEVEL_O_RDONLY ) > 0 ? O_RDONLY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_WRONLY ) > 0 ? O_WRONLY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_RDWR ) > 0 ? O_RDWR : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_APPEND ) > 0 ? O_APPEND : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_ASYNC ) > 0 ? O_ASYNC : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_CLOEXEC ) > 0 ? O_CLOEXEC : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_CREAT ) > 0 ? O_CREAT : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_DIRECTORY ) > 0 ? O_DIRECTORY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_EXCL ) > 0 ? O_EXCL : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_NOCTTY ) > 0 ? O_NOCTTY : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_NOFOLLOW ) > 0 ? O_NOFOLLOW : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_NONBLOCK ) > 0 ? O_NONBLOCK : 0 )
		| ( ( flags & SIOX_LOW_LEVEL_O_TRUNC ) > 0 ? O_TRUNC : 0 );
}

/*
 This helper function translates the mode flag from stdio to the same flags as for low-level I/O.
 Not storing this short string conserves space and standardizes these attributes.
 */
static inline unsigned translateFILEFlagsToSIOX( const char * str )
{
	unsigned flags = 0;

	for(const char * c = str; *c != '\0' ; c++ ){
		switch(*c){
			case 'a' :
				flags |= SIOX_LOW_LEVEL_O_APPEND | SIOX_LOW_LEVEL_O_CREAT;
				break;
			case 'r' :
				flags |= SIOX_LOW_LEVEL_O_RDONLY;
				break;
			case 'w' :
				flags |= SIOX_LOW_LEVEL_O_WRONLY | SIOX_LOW_LEVEL_O_CREAT | SIOX_LOW_LEVEL_O_TRUNC;
				break;
			case '+':
  				flags |= SIOX_LOW_LEVEL_O_RDWR;
				break;
		}
	}

	if( flags & SIOX_LOW_LEVEL_O_RDONLY && flags & SIOX_LOW_LEVEL_O_WRONLY){
		flags = (flags | SIOX_LOW_LEVEL_O_RDWR) & ~SIOX_LOW_LEVEL_O_RDONLY & ~SIOX_LOW_LEVEL_O_WRONLY;
	}

	return flags;
}

static inline void translatePOSIXFlagsToFILE( char * outBuff, int s_flags )
{
	if (s_flags & SIOX_LOW_LEVEL_O_APPEND){
		outBuff[0] = 'a';
		outBuff++;
	}
	if (s_flags & SIOX_LOW_LEVEL_O_RDONLY){
		outBuff[0] = 'r';
		outBuff++;
	}
	if (s_flags & SIOX_LOW_LEVEL_O_WRONLY){
		outBuff[0] = 'w';
		outBuff++;
	}	
	if (s_flags & SIOX_LOW_LEVEL_O_RDWR){
		outBuff[0] = '+';
		outBuff++;
	}	
}



static inline unsigned prepareIovec( const struct iovec * iov, int iovcnt ) {
	// TODO
	return 0;
}



static inline int translateErrnoToSIOX(int err){
	switch(err){
		case (0):{
			return SIOX_ESUCCESS;
		}
		case (EPERM) : {
			return SIOX_EPERM;
		}
		case (ENOENT) : {
			return SIOX_ENOENT;
		}
		case (ESRCH) : {
			return SIOX_ESRCH;
		}
		case (EINTR) : {
			return SIOX_EINTR;
		}
		case (EIO) : {
			return SIOX_EIO;
		}
		case (ENXIO) : {
			return SIOX_ENXIO;
		}
		case (E2BIG) : {
			return SIOX_E2BIG;
		}
		case (ENOEXEC) : {
			return SIOX_ENOEXEC;
		}
		case (EBADF) : {
			return SIOX_EBADF;
		}
		case (ECHILD) : {
			return SIOX_ECHILD;
		}
		case (EAGAIN) : {
			return SIOX_EAGAIN;
		}
		case (ENOMEM) : {
			return SIOX_ENOMEM;
		}
		case (EACCES) : {
			return SIOX_EACCES;
		}
		case (EFAULT) : {
			return SIOX_EFAULT;
		}
		case (ENOTBLK) : {
			return SIOX_ENOTBLK;
		}
		case (EBUSY) : {
			return SIOX_EBUSY;
		}
		case (EEXIST) : {
			return SIOX_EEXIST;
		}
		case (EXDEV) : {
			return SIOX_EXDEV;
		}
		case (ENODEV) : {
			return SIOX_ENODEV;
		}
		case (ENOTDIR) : {
			return SIOX_ENOTDIR;
		}
		case (EISDIR) : {
			return SIOX_EISDIR;
		}
		case (EINVAL) : {
			return SIOX_EINVAL;
		}
		case (ENFILE) : {
			return SIOX_ENFILE;
		}
		case (EMFILE) : {
			return SIOX_EMFILE;
		}
		case (ENOTTY) : {
			return SIOX_ENOTTY;
		}
		case (ETXTBSY) : {
			return SIOX_ETXTBSY;
		}
		case (EFBIG) : {
			return SIOX_EFBIG;
		}
		case (ENOSPC) : {
			return SIOX_ENOSPC;
		}
		case (ESPIPE) : {
			return SIOX_ESPIPE;
		}
		case (EROFS) : {
			return SIOX_EROFS;
		}
		case (EMLINK) : {
			return SIOX_EMLINK;
		}
		case (EPIPE) : {
			return SIOX_EPIPE;
		}
		case (EDOM) : {
			return SIOX_EDOM;
		}
		case (ERANGE) : {
			return SIOX_ERANGE;
		}
		case (EDEADLK) : {
			return SIOX_EDEADLK;
		}
		case (ENAMETOOLONG) : {
			return SIOX_ENAMETOOLONG;
		}
		case (ENOLCK) : {
			return SIOX_ENOLCK;
		}
		case (ENOSYS) : {
			return SIOX_ENOSYS;
		}
		case (ENOTEMPTY) : {
			return SIOX_ENOTEMPTY;
		}
		case (ELOOP) : {
			return SIOX_ELOOP;
		}
		#if (EWOULDBLOCK != EAGAIN)
		case (EWOULDBLOCK) : {
			return SIOX_EWOULDBLOCK;
		}
		#endif 
		case (ENOMSG) : {
			return SIOX_ENOMSG;
		}
		case (EIDRM) : {
			return SIOX_EIDRM;
		}
		case (ECHRNG) : {
			return SIOX_ECHRNG;
		}
		case (EL2NSYNC) : {
			return SIOX_EL2NSYNC;
		}
		case (EL3HLT) : {
			return SIOX_EL3HLT;
		}
		case (EL3RST) : {
			return SIOX_EL3RST;
		}
		case (ELNRNG) : {
			return SIOX_ELNRNG;
		}
		case (EUNATCH) : {
			return SIOX_EUNATCH;
		}
		case (ENOCSI) : {
			return SIOX_ENOCSI;
		}
		case (EL2HLT) : {
			return SIOX_EL2HLT;
		}
		case (EBADE) : {
			return SIOX_EBADE;
		}
		case (EBADR) : {
			return SIOX_EBADR;
		}
		case (EXFULL) : {
			return SIOX_EXFULL;
		}
		case (ENOANO) : {
			return SIOX_ENOANO;
		}
		case (EBADRQC) : {
			return SIOX_EBADRQC;
		}
		case (EBADSLT) : {
			return SIOX_EBADSLT;
		}
		#if (EDEADLOCK != EDEADLK)
		case (EDEADLOCK) : {
			return SIOX_EDEADLOCK;
		}
		#endif
		case (EBFONT) : {
			return SIOX_EBFONT;
		}
		case (ENOSTR) : {
			return SIOX_ENOSTR;
		}
		case (ENODATA) : {
			return SIOX_ENODATA;
		}
		case (ETIME) : {
			return SIOX_ETIME;
		}
		case (ENOSR) : {
			return SIOX_ENOSR;
		}
		case (ENONET) : {
			return SIOX_ENONET;
		}
		case (ENOPKG) : {
			return SIOX_ENOPKG;
		}
		case (EREMOTE) : {
			return SIOX_EREMOTE;
		}
		case (ENOLINK) : {
			return SIOX_ENOLINK;
		}
		case (EADV) : {
			return SIOX_EADV;
		}
		case (ESRMNT) : {
			return SIOX_ESRMNT;
		}
		case (ECOMM) : {
			return SIOX_ECOMM;
		}
		case (EPROTO) : {
			return SIOX_EPROTO;
		}
		case (EMULTIHOP) : {
			return SIOX_EMULTIHOP;
		}
		case (EDOTDOT) : {
			return SIOX_EDOTDOT;
		}
		case (EBADMSG) : {
			return SIOX_EBADMSG;
		}
		case (EOVERFLOW) : {
			return SIOX_EOVERFLOW;
		}
		case (ENOTUNIQ) : {
			return SIOX_ENOTUNIQ;
		}
		case (EBADFD) : {
			return SIOX_EBADFD;
		}
		case (EREMCHG) : {
			return SIOX_EREMCHG;
		}
		case (ELIBACC) : {
			return SIOX_ELIBACC;
		}
		case (ELIBBAD) : {
			return SIOX_ELIBBAD;
		}
		case (ELIBSCN) : {
			return SIOX_ELIBSCN;
		}
		case (ELIBMAX) : {
			return SIOX_ELIBMAX;
		}
		case (ELIBEXEC) : {
			return SIOX_ELIBEXEC;
		}
		case (EILSEQ) : {
			return SIOX_EILSEQ;
		}
		case (ERESTART) : {
			return SIOX_ERESTART;
		}
		case (ESTRPIPE) : {
			return SIOX_ESTRPIPE;
		}
		case (EUSERS) : {
			return SIOX_EUSERS;
		}
		case (ENOTSOCK) : {
			return SIOX_ENOTSOCK;
		}
		case (EDESTADDRREQ) : {
			return SIOX_EDESTADDRREQ;
		}
		case (EMSGSIZE) : {
			return SIOX_EMSGSIZE;
		}
		case (EPROTOTYPE) : {
			return SIOX_EPROTOTYPE;
		}
		case (ENOPROTOOPT) : {
			return SIOX_ENOPROTOOPT;
		}
		case (EPROTONOSUPPORT) : {
			return SIOX_EPROTONOSUPPORT;
		}
		case (ESOCKTNOSUPPORT) : {
			return SIOX_ESOCKTNOSUPPORT;
		}
		case (EOPNOTSUPP) : {
			return SIOX_EOPNOTSUPP;
		}
		case (EPFNOSUPPORT) : {
			return SIOX_EPFNOSUPPORT;
		}
		case (EAFNOSUPPORT) : {
			return SIOX_EAFNOSUPPORT;
		}
		case (EADDRINUSE) : {
			return SIOX_EADDRINUSE;
		}
		case (EADDRNOTAVAIL) : {
			return SIOX_EADDRNOTAVAIL;
		}
		case (ENETDOWN) : {
			return SIOX_ENETDOWN;
		}
		case (ENETUNREACH) : {
			return SIOX_ENETUNREACH;
		}
		case (ENETRESET) : {
			return SIOX_ENETRESET;
		}
		case (ECONNABORTED) : {
			return SIOX_ECONNABORTED;
		}
		case (ECONNRESET) : {
			return SIOX_ECONNRESET;
		}
		case (ENOBUFS) : {
			return SIOX_ENOBUFS;
		}
		case (EISCONN) : {
			return SIOX_EISCONN;
		}
		case (ENOTCONN) : {
			return SIOX_ENOTCONN;
		}
		case (ESHUTDOWN) : {
			return SIOX_ESHUTDOWN;
		}
		case (ETOOMANYREFS) : {
			return SIOX_ETOOMANYREFS;
		}
		case (ETIMEDOUT) : {
			return SIOX_ETIMEDOUT;
		}
		case (ECONNREFUSED) : {
			return SIOX_ECONNREFUSED;
		}
		case (EHOSTDOWN) : {
			return SIOX_EHOSTDOWN;
		}
		case (EHOSTUNREACH) : {
			return SIOX_EHOSTUNREACH;
		}
		case (EALREADY) : {
			return SIOX_EALREADY;
		}
		case (EINPROGRESS) : {
			return SIOX_EINPROGRESS;
		}
		case (ESTALE) : {
			return SIOX_ESTALE;
		}
		case (EUCLEAN) : {
			return SIOX_EUCLEAN;
		}
		case (ENOTNAM) : {
			return SIOX_ENOTNAM;
		}
		case (ENAVAIL) : {
			return SIOX_ENAVAIL;
		}
		case (EISNAM) : {
			return SIOX_EISNAM;
		}
		case (EREMOTEIO) : {
			return SIOX_EREMOTEIO;
		}
		case (EDQUOT) : {
			return SIOX_EDQUOT;
		}
		case (ENOMEDIUM) : {
			return SIOX_ENOMEDIUM;
		}
		case (EMEDIUMTYPE) : {
			return SIOX_EMEDIUMTYPE;
		}
		case (ECANCELED) : {
			return SIOX_ECANCELED;
		}
		case (ENOKEY) : {
			return SIOX_ENOKEY;
		}
		case (EKEYEXPIRED) : {
			return SIOX_EKEYEXPIRED;
		}
		case (EKEYREVOKED) : {
			return SIOX_EKEYREVOKED;
		}
		case (EKEYREJECTED) : {
			return SIOX_EKEYREJECTED;
		}
		case (EOWNERDEAD) : {
			return SIOX_EOWNERDEAD;
		}
		case (ENOTRECOVERABLE) : {
			return SIOX_ENOTRECOVERABLE;
		}
		case (ERFKILL) : {
			return SIOX_ERFKILL;
		}
		case (EHWPOISON) : {
			return SIOX_EHWPOISON;
		}
	default:
		return SIOX_ERR_UNKNOWN;
	}
}


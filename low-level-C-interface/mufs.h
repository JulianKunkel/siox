/**
 * @file	mufs.h
 *			Headerdatei für das Mock-Up-File-System.
 * 
 * @authors	Julian Kunkel & Michaela Zimmer
 * @date	2011
 * 			GNU Public License
 */


#ifndef mufs_H
#define mufs_H


/**
 * Write the contents string into a file specified, overwriting any pre-existing file of that name.
 * Opening the file before writing and closing it afterwards are all included:
 * No fuss, no juggling stupid file handles, all nice, easy and clean. Just as you like it. ;)
 *
 * @param	filename	The name of the file. May include a relative or absolute path.
 * @param	contents	The contents to be written, presumably a string.
 *
 * @return	If successful, the number of bytes written; otherwise, 0.
 */
long mufs_putfile( const char * filename, const char * contents );


#endif

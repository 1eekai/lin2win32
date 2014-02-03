#pragma once

#include <conio.h>
#include <sys/types.h>
#include <direct.h>
#include <errno.h>
#include <stdio.h>  
#include <windows.h>
#include <process.h>

/* only writes the screen with any difference between DOS/POSIX programs. */
#if defined(__XMK__)
typedef signed char pid_t;
#else
typedef int pid_t;			/* Define this is just only fit for the fork() system calls.  */
#endif

inline int getch(void){
	char ch;
	ch=getch();
	putchar(ch);
	return ch;
}
/*
 * getch() under Unix-based systems has the console echo, but it's not in the Windows/DOS.
 * And in the UNIX-based systems, the getch() functions always declares in the unistd.h, and
 * Windows/DOS was in the conio.h.
 * If an C source wants to call this function and it uses unistd.h, it must be an POSIX programs.
 * and this parts of code takes it effect. Or not, the conio.h effects.
 *
 */

inline int chdir(const char *__path){	return _chdir(__path);	}	/* calles the Microsoft POSIX-compatitable functions. */
inline int chmod(const char *__path, mode_t __mode ){	return _chmod(__path, __mode);	}
inline int chown(const char *__path, uid_t __owner, gid_t __group ){	return _chmod(__path, __owner, __group);	}

pid_t fork(void){
	
	
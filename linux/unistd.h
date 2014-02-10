#include <windows.h>
#include <malloc.h>
#include <Psapi.h>
#include <conio.h>
#include <io.h>

#pragma comment(lib,"Psapi.lib")
/* #pragma comment(lib,"User32.lib") */

#define __T(x) L ## x
#define _T(x) __T(x)

#define __mode_t __MODE_T_TYPE
#define __MODE_T_TYPE __U32_TYPE
#define __U32_TYPE unsigned int

#ifndef __mode_t_defined
typedef __mode_t mode_t;
# define __mode_t_defined
#endif

typedef  unsigned int kernel_uid32_t;
typedef kernel_uid32_t uid_t;

#if defined(__XMK__)
typedef signed char pid_t;
#else
typedef int pid_t;			/* Define this is just only fit for the fork() system calls.  */
#endif

#ifndef __pid_t_defined
typedef __pid_t pid_t;
# define __pid_t_defined
#endif

#define __S32_TYPE      int
#define __PID_T_TYPE        __S32_TYPE

/* only writes the screen with any difference between DOS/POSIX programs. */

int getch(void){
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	char* chCon=(char *)malloc(sizeof(char)*2);
	char ch;
	DWORD num;
	ch=_getch();
	chCon[0]=ch;
	WriteConsole(hstdout,chCon,sizeof(chCon),&num,NULL);
	free(chCon);
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

int chdir(const char *__path){	return _chdir(__path);	}	/* calles the Microsoft POSIX-compatitable functions. */
int chmod(const char *__path, mode_t __mode ){	return _chmod(__path, (int)__mode);	}
int chown(const char *__path, uid_t __owner, gid_t __group ){	return _chmod(__path, __owner, __group);	}

BOOL GetProcessFullPath(DWORD dwPID, TCHAR pszFullPath[MAX_PATH]){
    TCHAR szImagePath[MAX_PATH];
    HANDLE hProcess;
    if(!pszFullPath)
        return FALSE;
    pszFullPath[0] = '\0';
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
    if(!hProcess)
        return FALSE;
    if(!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH)){
        CloseHandle(hProcess);
        return FALSE;
    }
    if(!DosPathToNtPath(szImagePath, pszFullPath)){
        CloseHandle(hProcess);
        return FALSE;
    }
    CloseHandle(hProcess);
    return TRUE;
}

BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath){
    TCHAR szDriveStr[500];
    TCHAR szDrive[3];
    TCHAR szDevName[100];
    INT cchDevName;
    INT i;
    /* Check the parameters. */
    if(!pszDosPath || !pszNtPath)   return FALSE;
    /* Get the local disk volume. */
    if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr)){
        for(i = 0; szDriveStr[i];i+=4){
            if(!lstrcmpi(&(szDriveStr[i]),_T("A:\\")) || !lstrcmpi(&(szDriveStr[i]),_T("B:\\")))    continue;
            szDrive[0] = szDriveStr[i];
            szDrive[1] = szDriveStr[i + 1];
            szDrive[2] = '\0';
            if(!QueryDosDevice(szDrive,szDevName,100))  return FALSE;   /* Get the DOS device name. */
            cchDevName = lstrlen(szDevName);
            if(_tcsnicmp(pszDosPath, szDevName, cchDevName)==0){/* Bingo!! */
                lstrcpy(pszNtPath, szDrive);/* Copy the volume. */
                lstrcat(pszNtPath, pszDosPath + cchDevName);/* Copy the path. */
                return TRUE;
            }
        }
    }
    lstrcpy(pszNtPath, pszDosPath);
    return FALSE;
}

pid_t fork(void){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    int createResult;
    TCHAR* procpath=(TCHAR *)malloc(sizeof(TCHAR)*255);
    ZeroMemory(&si,sizeof(si));
    si.cb=sizeof(si);
    ZeroMemory(&pi,sizeof(pi));
    GetProcessFullPath(_getpid(),procpath);
    createResult=CreateProcess(NULL,
							  (char *)procpath,
							  NULL,
							  NULL,
							  FALSE,
							  0,
							  NULL,
							  NULL,
							  &si,
							  &pi
							  );
    free(procpath);
    return 0;
}

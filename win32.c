#include <windows.h>
#include <psapi.h>   // use -lpsapi when linking

#define PREPARE() PROCESS_INFORMATION pif[argc-1]; STARTUPINFO si[argc-1];
#define RUN() Run(argv[i], &pif[i-1], &si[i-1], null_output, own_directory)
#define WAIT_AND_READ() WaitAndRead(&pif[i-1], i, Num(i, parallel, null_output, own_directory))
#define CORRECT() pif[i-1].hProcess
#define UNIX_KILL()

BOOL Run(char* command, PROCESS_INFORMATION *pif, STARTUPINFO *si, int null_output, int own_directory) {
   ZeroMemory(si,sizeof(*si));
   si->cb = sizeof(*si);
   if (null_output) {
      si->dwFlags |= STARTF_USESTDHANDLES;
      si->hStdOutput = NULL;
   }

   char* dir = NULL;
   char d[strlen(command)+3];
   if (own_directory) {
      dir = d;
      strcpy(dir, command);
      strcat(dir, "\\..");
   }

   return CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, dir, si, pif);
}

void Wait(HANDLE hProcess) {
   WaitForSingleObject(hProcess, INFINITE);
}

BOOL Read(PROCESS_INFORMATION *pif, int i) {
   PROCESS_MEMORY_COUNTERS p;
   BOOL res = GetProcessMemoryInfo(pif->hProcess, &p, sizeof(p));
   if (res)
      printf(PEAK_PRINT, i, p.PeakWorkingSetSize >> 10, p.PeakWorkingSetSize);

   res &= CloseHandle(pif->hProcess);
   res &= CloseHandle(pif->hThread);
   return res;
}

void WaitAndRead(PROCESS_INFORMATION *pif, int i, int num) {
   Wait(pif->hProcess);

   if (!Read(pif, num))
      fprintf(stderr, READ_ERROR, num);
}

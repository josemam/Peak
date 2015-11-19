#include <windows.h>
#include <stdio.h>
#include <psapi.h>   // use -lpsapi when linking

void Usage(char* name) {
   fprintf(stderr, "usage: %s program1 [program2 program3 ...] [-p] [-n]\n", name);
}

BOOL Run(char* command, PROCESS_INFORMATION *pif, STARTUPINFO *si, int null_output) {
   ZeroMemory(si,sizeof(*si));
   si->cb = sizeof(*si);
   if (null_output) {
      si->dwFlags |= STARTF_USESTDHANDLES;
      si->hStdOutput = NULL;
   }

   return CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, si, pif);
}

BOOL Read(PROCESS_INFORMATION *pif, int i) {
   PROCESS_MEMORY_COUNTERS p;
   int res = GetProcessMemoryInfo(pif->hProcess, &p, sizeof(p));
   if (res)
      printf("#%d peak memory used: %d KB\t(%d bytes)\n", i,
             p.PeakWorkingSetSize >> 10, p.PeakWorkingSetSize);
   else {
      return res;
   }

   CloseHandle(pif->hProcess);
   CloseHandle(pif->hThread);
   return res;
}

int Num(int i, int p, int no) {
    return i-(p && i > p)-(no && i > no);
}

void Wait(HANDLE hProcess) {
	WaitForSingleObject(hProcess, INFINITE);
}

int main(int argc, char* argv[]) {
   if (argc < 2) {
      Usage(argv[0]);
      return 1;
   }
    
   int i;
   int parallel = 0;
   int null_output = 0;
   for (i = 1; i < argc; i++)
      if (strcmp(argv[i], "-p") == 0)
         parallel = i;
      else if (strcmp(argv[i], "-n") == 0)
         null_output = i;
            
    
   PROCESS_INFORMATION pif[argc-1];
   STARTUPINFO si[argc-1];

   for (i = 1; i < argc; i++) {
      if (i == parallel || i == null_output)
         continue;
        
      if (!Run(argv[i], &pif[i-1], &si[i-1], null_output)) {
         fprintf(stderr, "Unable to start program #%d\n", Num(i, parallel, null_output));
         continue;
      }

      if (!parallel) {
         Wait(pif[i-1].hProcess);

         if (!Read(&pif[i-1], Num(i, parallel, null_output)))
            fprintf(stderr, "Unable to read program #%d attributes\n", Num(i, parallel, null_output));
      }
   }
    
   if (parallel)
      for (i = 1; i < argc; i++) {
         if (i == parallel || i == null_output || !pif[i-1].hProcess)
            continue;

         Wait(pif[i-1].hProcess);

         if (!Read(&pif[i-1], Num(i, parallel, null_output)))
            fprintf(stderr, "Unable to read program #%d attributes\n", Num(i, parallel, null_output));
      }
}

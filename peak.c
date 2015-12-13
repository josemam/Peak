#include <windows.h>
#include <stdio.h>
#include <psapi.h>   // use -lpsapi when linking

int Usage(char* name) {
   fprintf(stderr, "usage: %s program1 [program2 program3 ...] [-p] [-n]\n", name);
   return 1;
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

void Wait(HANDLE hProcess) {
   WaitForSingleObject(hProcess, INFINITE);
}

BOOL Read(PROCESS_INFORMATION *pif, int i) {
   PROCESS_MEMORY_COUNTERS p;
   BOOL res = GetProcessMemoryInfo(pif->hProcess, &p, sizeof(p));
   if (res)
      printf("#%d peak memory used: %d KB\t(%d bytes)\n", i,
             p.PeakWorkingSetSize >> 10, p.PeakWorkingSetSize);

   res &= CloseHandle(pif->hProcess);
   res &= CloseHandle(pif->hThread);
   return res;
}

void WaitAndRead(PROCESS_INFORMATION *pif, int i, int num) {
   Wait(pif->hProcess);

   if (!Read(pif, num))
      fprintf(stderr, "Unable to read program #%d attributes\n", num);
}

int Num(int i, int p, int no) {
    return i-(p && i > p)-(no && i > no);
}

int main(int argc, char* argv[]) {
   int i;
   int parallel = 0;
   int null_output = 0;
   int valid_inputs = 0;
   for (i = 1; i < argc; i++)
      if (strcmp(argv[i], "-p") == 0)
         parallel = i;
      else if (strcmp(argv[i], "-n") == 0)
         null_output = i;
      else
         ++valid_inputs;

   if (!valid_inputs)
      return Usage(argv[0]);

   PROCESS_INFORMATION pif[argc-1];
   STARTUPINFO si[argc-1];

   for (i = 1; i < argc; i++)
      if (i != parallel && i != null_output)        
         if (!Run(argv[i], &pif[i-1], &si[i-1], null_output))
            fprintf(stderr, "Unable to start program #%d\n", Num(i, parallel, null_output));
         else if (!parallel)
            WaitAndRead(&pif[i-1], i, Num(i, parallel, null_output));
    
   if (parallel)
      for (i = 1; i < argc; i++)
         if (i != parallel && i != null_output && pif[i-1].hProcess)
            WaitAndRead(&pif[i-1], i, Num(i, parallel, null_output));
}

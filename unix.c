#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#define BOOL int
#define FALSE 0
#define TRUE 1

#define PREPARE() pid_t pif[argc-1]; int si[argc-1];
#define RUN() Run(argv[i], &pif[i-1], null_output, own_directory)
#define WAIT_AND_READ() Wait(&pif[i-1], &si[i-1], Num(i, parallel, null_output, own_directory))
#define CORRECT() TRUE
#define UNIX_KILL() kill(getpid(), SIGKILL)

int FindNextParam(char* command) {
   BOOL dcommas = 0;
   int i;
   for (i = 0; ((dcommas&2) || command[i] != ' ') && command[i] != '\0'; i++)
      if (command[i] == '"')
         dcommas++;

   if (command[i] != '\0')
      command[i++] = '\0';
   return i;
}

BOOL Run(char* command, pid_t *pif, int null_output, int own_directory) {
   *pif = fork();
   if (*pif == -1)
      return FALSE;
   if (*pif == 0) {
      #define LIMIT 256
      char* first_parameter = command;
      char** parameters = malloc(LIMIT*sizeof(char*));
      int i;
      for (i = 0; i < LIMIT; i++) {
         if (command[0] == '\0')
            break;
         else {
            parameters[i] = malloc(LIMIT*sizeof(char));
            int next = FindNextParam(command);
            strcpy(parameters[i], command);
            command += next;
         }
      }
      parameters[i] = NULL;

      if (null_output)
         dup2(open("/dev/null", 0), 1);

      if (own_directory) {
         char* dir = strdup(first_parameter);
         chdir(dirname(dir));
         free(dir);
      }

      execv(first_parameter, parameters);
      for (; i >= 0; i--)
         free(parameters[i]);
      free(parameters);
      return FALSE;
   }

   return TRUE;
}

BOOL Wait(pid_t *pif, int* si, int i) {
   struct rusage ru;
   if(wait4(*pif, si, 0, &ru) == 0)
      return FALSE;

   if (!WIFSIGNALED(*si) || WTERMSIG(*si) != SIGKILL)
      return Read(pif, si, i, &ru);

   return TRUE;
}

BOOL Read(pid_t *pif, int* si, int i, struct rusage *ru) {
   int memory = ru->ru_maxrss;
   printf("#%d peak memory used: %d KB\t(%d bytes)\n", i, memory, memory << 10);
   return TRUE;
}

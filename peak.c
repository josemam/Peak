#include <stdio.h>

#ifdef _WIN32
   #include "win32.c"  // requires -lpsapi when linking
#elif __linux__
   #include "unix.c"
#else 
   #error "OS not supported"
#endif

int Usage(char* name) {
   fprintf(stderr, "usage: %s program1 [program2 program3 ...] [-p] [-n] [-d]\n", name);
   return 1;
}

int Num(int i, int p, int no, int dir) {
   return i-(p && i > p)-(no && i > no)-(dir && i > dir);
}

int main(int argc, char* argv[]) {
   int i;
   int parallel = 0;
   int null_output = 0;
   int own_directory = 0;
   int valid_inputs = 0;
   for (i = 1; i < argc; i++)
      if (strcmp(argv[i], "-p") == 0)
         parallel = i;
      else if (strcmp(argv[i], "-n") == 0)
         null_output = i;
      else if (strcmp(argv[i], "-d") == 0)
         own_directory = i;
      else
         ++valid_inputs;

   if (!valid_inputs)
      return Usage(argv[0]);

   PREPARE();

   for (i = 1; i < argc; i++)
      if (i != parallel && i != null_output && i != own_directory)        
         if (!RUN()) {
            fprintf(stderr, "Unable to start program #%d\n", Num(i, parallel, null_output, own_directory));
            UNIX_KILL();
         } else if (!parallel)
            WAIT_AND_READ();

   if (parallel)
      for (i = 1; i < argc; i++)
         if (i != parallel && i != null_output && i != own_directory && CORRECT())
            WAIT_AND_READ();
}

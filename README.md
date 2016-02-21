# Peak
Measures peak memory usage of a program

This program uses Windows/Linux API to run a process and get its peak memory usage.

## Usage

`peak program1 [program2 program3 ...] [-p] [-n] [-d]`

You may place the arguments anywhere and in any order.

 - If `-p` __is not__ used, every program will be started after the previous one
   has been finished. If it __is__, all programs will be started (almost)
   simultaneously. (The peak memory usage will always be printed in order, though)
 - It `-n` __is__ used, every program standard output (but not the standard error)
   will be redirected to null.
 - If `-d` __is__ used, the working directory for every program will be set to the
   folder where it is placed. If it __is not__, the working directory will be the
   same as it was for the terminal or command prompt.

## Please note

 - Windows and Linux memory management is not strictly identical. 
 - The sources used to get the peak memory ([PeakWorkingSetSize]
   (https://msdn.microsoft.com/en-us/library/ms684877.aspx) in Windows and
   [ru_maxrss] (http://man7.org/linux/man-pages/man2/getrusage.2.html)
   in Linux) do not measure the same thing on both Windows and Linux.

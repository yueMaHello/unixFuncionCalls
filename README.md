# unixFuncionCalls

Objectives:
This programming project is intended to give me experience in using Unix function calls in C to manage processes.
Through this project, I familiarized myself with some Unix functions, such as ‘umask’, ‘chdir’, ‘fork’, and ‘getenv’. I had a better understand about the child process and the parent process.
Design Overview:
1. The program can be invoked by ‘a1shell interval’. ‘interval’ is a number (see point3).
2. There is a parent process named ‘a1shell’ which forks a child process named ‘a1monitor’.
3. In ‘a1monitor’ process, it will display the current date and time, average system load and the number
of processes every ‘interval’ seconds.
4. In ‘a1shell’ process, it will let the user to input some specific commands to execute, such as ‘cd
pathname’, ‘pwd’, ‘done’, and ‘umask’. If user calls some other Unix command, the ‘a1shell’ process
will fork a child process ‘executeBash’ to execute it.
5. ‘executeBash’ process will overlay itself with Bash to execute the command.
6. The ‘a1shell’ process will compute and print user and system time during executing the child process
‘executeBash’.
7. Through ‘Makefile’, the user can make, clean and zip the program easily.
Project Status:
The project has been completed with all the functionality and features. There are several difficulties during the implementation:
1. Since I used Mac OS to develop the program, some functions didn’t run as expectation. After I uploaded the file on the server, everything was good.
2. Some C programming syntax is difficult. There is no slicing feature in the string, which makes it hard to compare two strings.
Testing and Results:
I tested the application feature by feature. After I finished all the features, I tried several Unix commands in the ‘a1shell’ to make sure everything was fine.
Results of Timing:
1. Total time elapsed = parent user time + parent cpu time+ child user time + child cpu time
2. When executing simple commands, such as ‘ls’ and ‘mkdir’, all the timing values are always zero or
near zero.
3. When executing ‘man grep’ or ‘top’, it is apparently that the parent cpu time is much larger than the
other three time. The time used in the child process is always zero or near zero.
Acknowledgements:
1. https://www.tutorialspoint.com/cprogramming/c_strings.htm
2. https://www.youtube.com/watch?v=PwxTbksJ2fo
3. Textbook: Advanced Programming in UNIX Environment

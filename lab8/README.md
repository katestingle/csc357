# Lab 8: Signals in Unix
- Distinguish language features from operating system features.
- Consider basic resource management in the development of programs.
- Gain experience with low-level programming in the UNIX environment.
- Discuss the architecture of the UNIX operating system from a system-programmer’s perspective and be able to write programs that use operating system services (system calls) directly.

# Lab Tasks

*Task 1: timeout*
    Write a program, named my_timeout, that can be used to limit the duration of another program. 
my_timeout takes, as command-line arguments, an integer number of seconds and another command 
(optionally with arguments of its own). The my_timeout program must spawn a child process to 
execute the argument command (with its command-line arguments) and set an alarm that will be 
triggered after the specified number of seconds (use sigaction). If the child process has not 
terminated by the time that the alarm has triggered, then it should be killed (and the exit 
status of my_timeout should be non-zero). If the child process terminates, then the exit 
status of my_timeout should be that of the child process.

**EXAMPLE:**
`% ./my_timeout 5 sleep 10`
``
Killing child ...
% echo $?
1
``
You need not worry about the case when the alarm might trigger exactly when the child process terminates (this is an example of what?).

*Task 2: Echo Server Improved*
Extend your echo server from the previous lab to spawn a child process to handle each request (this can then be tested by verifying that
 multiple connections are simultaneously supported). The primary (parent) process should register a signal handler for the SIGCHLD 
 signal (raised when a child process completes) and wait within that handler. Consider a waitpid with WNOHANG and a loop to handle 
 all pending child processes.
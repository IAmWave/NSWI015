# Exam theory questions

[exam rules](https://devnull-cz.github.io/unix-linux-prog-in-c/exams.html),
[theory questions](https://devnull-cz.github.io/unix-linux-prog-in-c/questions-theory.html)

These notes are for my use, correctness not guaranteed.

## Development tools

- Describe principles of compiler and link editor for C language.
    * compiler creates object files
    * linker combines object files, resolving cross-file references
- Explain the principles of make utility and write sample Makefile for compilation and linking of a program in C.
    * sample: see `26`
        * `cc -c util.c` to compile to `util.o`, then feed `util.o` to `cc`
- What are the advantages and disadvantages of using dynamic libraries? Write a sequence of commands for producing dynamic library and program which will depend on it (ideally on both Linux and Solaris).
    * pros: no need to recompile, binary size.
    * cons: ABI, differences in versions
    * `gcc -shared -o libadd.so add.c`, `gcc -L. -ladd -Xlinker -R . main.c`
- What is affected by LD_PRELOAD and LD_LIBRARY_PATH environment variables? Why it is not good idea to use LD_LIBRARY_PATH environment variable for anything else other than debugging purposes?
    * `LD_PRELOAD`: by setting this, symbols from a given dynamic library takes precedence over others, allowing us to replace e.g. `malloc`
    * `LD_LIBRARY_PATH` stores application-level configuration globally, setting for one program carries to others

## Kernel and UNIX C API

- Explain the meaning of `main()` parameters and the `environ` variable. How are the command line arguments processed with `getopt()`?
    * `int argc` (includes the program name, so `argc==3` for `ls -l /`)
    * `char *argv[]`, `argv[0]` is the executed program's name. `argv[argc] == NULL`
    * `char **environ` contains environment variables as strings in the form `ABC=xyz`
    * `getopt()`: see 49
- Explain the meaning of terms like process, thread, program. What is the difference between process and thread?
    * process: code and data in memory
    * thread: exists within a process, threads share memory except for registers and stack ("line of execution")
    * program: executable file on disk
- Explain the terms library function and system call. Describe the convention/semantics of return values of library calls and system calls.
    * a system call (`man 2 ...`) is executed in kernel mode
    * a library function call (`man 3 ...`) happens entirely in user mode
    * return semantics:
        * ints: >= 0 success, == -1 failure
        * pointers: != NULL success, == NULL failure
- What do the contents of files `/etc/passwd` and `/etc/group` look like? When and how are the data stored in these files used in the UNIX system?
    * `/etc/passwd` - user database. username, hashed pw, UID, GID, full name, home dir, login shell
    * `/etc/group` - group database. group name, group pwd (not used), GID, list of members
- How is the identification of owner and group used for permissions testing?
    * each file has an owner and group owner
    * user 0 has all rights, owner has owner rights, group owner and supergroup has
- What is the difference between real and effective owner of a process?
    * effective owner - if the SUID/SGID bit is set, the program may be run by other users and the process will have EUID set to the file's owner (typically root)

## File systems

- What objects in UNIX are accessible via file system interface? What is the difference between character and block device?
    * accessible via file system interface: files, sockets, named pipes, peripheral device, network drives
    * character device (terminal) is not buffered
    * block device (e.g. disk) is buffered, r/w happens in blocks. The blocks are then cached for faster IO
- What are access rights for files? What is set UID?
    * each file has read/write/exec permissions (access rights) set for owner, group and others (u, g, o)
    * sticky bit: files may be deleted/renamed only by file owner, dir owner or root
    * set UID (SUID) see above
- How do processes access open files? What is the difference between descriptor and opening of a file?
    * multiple descriptors may correspond to a single open file (e.g. by `dup()`), also a descriptor may also correspond to a socket, pipe etc.
- Describe structure of S5 volume and its extensions (ufs).
    * boot block, superblock, inode area, data block area
    * ufs: divided into cylinder groups
- Explain the principle of navigating the directory structure, i.e. how does kernel find data blocks of given file using its path? Explain the difference between hardlink and symlink.
    * inode knows position of file on disk, directories are files containing information about what the inodes of its subdirectories are
    * hardlink: references the same inode
    * symlink: is a file which says what file it's referring to (relative or absolute)
- What is virtual file system? What is it for and what are its principles?
    * abstraction over concrete file systems
    * `struct file` for an open file, a `vnode` for an `inode` abstraction
- What is the difference between disk paritition and volume? What does the process of creation, consistency check, mount and unmount of a volume look like?
    * partition: a part of a disk
    * volume: can connect multiple partitions into one file system

## Processes and pipes

- Describe address space of a process in user mode and kernel mode.
    * user space: text (code), data (variables), bss (non-initialized variables), (heap), ..., stack, user area (process information used by kernel; #open files, signal handling, env variables, cwd...)
    * kernel space: kernel text, kernel data and bss, user struct, kernel stack
- Draw and describe state diagram of a process.
    * process goes into kernel mode via: trap induced by CPU, timer (to invoke scheduler), interrupt (peripheral device), synchronous trap (syscall)
    * see 99
- Describe basics of mechanism of process planning. What priority classes?
    * CPU is assigned to the first ready process with the highest priority
    * priority classes: system = 60-99, real-time = 100-159, time-shared = 0-59
- Describe the mechanism of mapping files into memory. Describe how a process can access variables and functions in shared library during runtime. How are these 2 things related together?
    * memory mapping: `mmap`
    * dynamic libs: `dlopen`, `dlsym`, `dlclose`, `dlerror`

## Signals

- What are signals? How can be signal generated for a process? What is the difference between sending signal to process and to a thread?
    * signal generation: asynchromous events (timer SIGALRM, terminal disconnect SIGHUP, ctrl+c SIGINT)
- What are the options for handling a signal? How is it (blocking, handling) set up for process and for threads?
    * signals have default action, a process may handle a signal using a "handler" (except SIGKILL, SIGSTOP)
    * handling: `sigaction()`
    * blocking: `sigprocmask`, unblocked signals get delivered
- What are the rules for writing signal handlers? How can one cope with restrictions for their implementation?
    * use reentrant functions; it could happen a signal is delivered within a function that the handler wants to use
    * workaround: just set a global variable from the handler, do something later

## Threads

- What is a thread and how does it differ from a process?
    - What attributes are common for process and which are private for a thread?
        * memory (global variables) are shared
    - How can one create global variable for a thread?
        * `pthread_key_create`, `pthread_setspecific`, `pthread_getspecific`
- Describe the process for creating and destroying a thread? How do the destructors of keyed values and exit handlers work?
    * creating: `pthread_create`, destroying: when the function ends
    * cleanup functions are called, data destructors called in unspecified order
- List synchronization primitives for threads.
    * mutexes: analogous to semaphors, with a capacity of 1
    * condition variables: used to signal the state of shared data (e.g. we are waiting for a queue to become non-empty)
    * read-write locks: multiple threads may have read locks, only one write lock. But read and write locks are mutually exclusive
    * barriers: waits for all threads to reach the barrier
    - Describe the way of using condition variables, reasons for using them in certain way (w.r.t. order of locking) and what could happen if this was not abided.
        * order of locking: see 202?
- What are the ways of solving synchronization arithmetic operation (adding and subtraction) between threads and list their advantages and disadvantages.
    * mutex
    * atomic functions from `<stdatomic.h>` or `<atomic.h>`, faster, but not available everywhere
- What happens if one thread calls fork()? What problem this could lead to and how to solve it?
    * other threads are not copied, if they had allocated memory it is lost. their mutexes remain locked

## Synchronization and locking

- Explain locking conflict when accessing shared data? What is a deadlock? Describe locking of files by using `fcntl()`.
    * `fcntl` may lock a part of a file
- Explain locking by using lock files.
    * a file's existence signals that a specific resource is locked

## IPC

- Describe semaphores implemented in UNIX System V IPC. What commands can be used to list their state?
    * works with semaphore arrays; `semget`, `semctl`, `semop`

## Networking

- Describe workings of server and client (in terms of sequence of system calls) for connected network services.
    * see `154`
- Describe workings of server and client (in terms of sequence of system calls) for datagram network services.
    * client: `fd = socket()`, `sendto(fd)`, `recvfrom(fd)`, [...], `close(fd)`
    * server: `fd = socket()`, `bind(fd)`, `recvfrom(fd)`, `sendto(fd)`
- What is a socket?
    - What are the variants of addressing sockets (within single system and over the network)?
        * IPv4, IPv6, UNIX sockets
    - What functions can be used for conversion from numeric and string representation (and vice versa) of protocols, ports and IP addresses?
        * `inet_pton`, `inet_ntop`, `getprotobyname`, `getservbyname`
    - Why do the functions for converting from local and network byte order have to be used?
        * network: big-endian, computers mostly little-endian
- Describe standard functions for conversion from hostname to address and back, their use for client and server. What advantages do they have compared to non-standard functins?
    * `getaddrinfo`, `getnameinfo`
- Describe possibilities of sequential and parallel handling of clients of TCP server.
    * parallel: `fork()` after `accept()` (new child process per client), or after `listen()` (pool of workers)
- How does `inetd` work? How does parallel handling of UDP clients look like?
    * a single socket may be used to talk to multiple clients
- What are the ways of expecting/handling data from multiple descriptors?
    * `select`, `poll`
    - How are the functions used when implementing network server which handles multiple clients from one process without the use of threads?
        * we loop and use `select` to wait for a fd which is ready (a client ready for communication)
    - What are the differences between functions which make this possible?
        * `poll` can wait for specific events, can handle more fds than `FD_SETSIZE`

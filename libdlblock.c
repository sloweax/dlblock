#include "dlblock.h"
#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define PUSH_ALL()             \
    asm volatile("push %rdi"); \
    asm volatile("push %rsi"); \
    asm volatile("push %rdx"); \
    asm volatile("push %rcx"); \
    asm volatile("push %r8");  \
    asm volatile("push %r9")

#define POP_ALL()             \
    asm volatile("pop %r9");  \
    asm volatile("pop %r8");  \
    asm volatile("pop %rcx"); \
    asm volatile("pop %rdx"); \
    asm volatile("pop %rsi"); \
    asm volatile("pop %rdi")

char *getenv(const char *name);

static int in_block_list(char *name)
{
    size_t namelen = strlen(name);
    size_t buflen = 0;
    char buf[64];

    const char *env = getenv(ENV);
    if (env == NULL) return 0;

    do {
        if (*env == ',' || *env == '\0') {
            buf[buflen] = 0;

            if (buflen != namelen) {
                buflen = 0;
                continue;
            }

            if (strncmp(buf, name, namelen) == 0) return 1;

            buflen = 0;
            continue;
        }

        if (buflen >= sizeof(buf)) {
            buflen = 0;
            continue;
        }

        buf[buflen++] = *env;
        
    } while (*env++);

    return 0;
}

#define ORIGINAL_NAME(name) dfl_ ## name

#define HOOK(name, return_type, err, return_err)               \
static return_type (*ORIGINAL_NAME(name))();                   \
return_type name()                                             \
{                                                              \
    PUSH_ALL();                                                \
    if (ORIGINAL_NAME(name) == NULL)                           \
        ORIGINAL_NAME(name) = dlsym(RTLD_NEXT, #name);         \
    if (ORIGINAL_NAME(name) == NULL || in_block_list(#name)) { \
        POP_ALL();                                             \
        errno = err;                                           \
        return return_err;                                     \
    }                                                          \
    POP_ALL();                                                 \
    return ORIGINAL_NAME(name)();                              \
}

// stdlib.h
HOOK(malloc, void*, ENOMEM, NULL)
HOOK(realloc, void*, ENOMEM, NULL)
HOOK(system, int, ENOMEM, -1)

// stdio.h
HOOK(fopen, void*, ENOMEM, NULL)
HOOK(fprintf, int, EIO, -1)
HOOK(fread, ssize_t, EIO, -1)
HOOK(fwrite, ssize_t, EIO, -1)
HOOK(printf, int, EIO, -1)
HOOK(puts, int, EIO, -1)

// fcntl.h
HOOK(openat, int, ENOMEM, -1)
HOOK(open, int, ENOMEM, -1)

// unistd.h
HOOK(execle, int, ENOMEM, -1)
HOOK(execl, int, ENOMEM, -1)
HOOK(execlp, int, ENOMEM, -1)
HOOK(execve, int, ENOMEM, -1)
HOOK(execv, int, ENOMEM, -1)
HOOK(execvpe, int, ENOMEM, -1)
HOOK(execvp, int, ENOMEM, -1)
HOOK(fork, int, ENOMEM, -1)
HOOK(read, ssize_t, EIO, -1)
HOOK(write, ssize_t, EIO, -1)

// sys/socket.h
HOOK(bind, int, EADDRINUSE, -1)
HOOK(connect, int, ECONNREFUSED, -1)
HOOK(recvfrom, ssize_t, ECONNREFUSED, -1)
HOOK(recvmsg, ssize_t, ECONNREFUSED, -1)
HOOK(recv, ssize_t, ECONNREFUSED, -1)
HOOK(sendmsg, ssize_t, ECONNRESET, -1)
HOOK(send, ssize_t, ECONNRESET, -1)
HOOK(sendto, ssize_t, ECONNRESET, -1)
HOOK(socket, int, ENOMEM, -1)

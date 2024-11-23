#include "user.h"
#include "kernel/syscall.h"

int pgaccess(void *base, int len, void *mask){
    return syscall(SYS_pgaccess, base, len, mask);
}
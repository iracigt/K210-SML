#include "platform.h"

void GC_displayMem (void) {
        ;
}

void GC_setSigProfHandler (struct sigaction *sa) {
        (void) sa;
}

void GC_diskBack_read (void *data, pointer buf, size_t size) {
        die ("Operation `%s` unsupported on baremetal platform.\n", __func__);
}

void GC_diskBack_close (void *data) {
        (void) data; // Need to free this?
}

void *GC_diskBack_write (pointer buf, size_t size) {
        die ("Operation `%s` unsupported on baremetal platform.\n", __func__);
}

__attribute__ ((noreturn))
void Posix_IO_setbin (__attribute__ ((unused)) C_Fd_t fd) {
        die("Posix_IO_setbin not implemented");
}

__attribute__ ((noreturn))
void Posix_IO_settext (__attribute__ ((unused)) C_Fd_t fd) {
        die("Posix_IO_settext not implemented");
}

void *GC_mmapAnonFlags_safe_protect (void *start, size_t length,
                                     int prot, int flags,
                                     size_t dead_low,
                                     size_t dead_high) {
        die ("Operation `%s` unsupported on baremetal platform.\n", __func__);
}

void *GC_mmapAnon_safe_protect (void *start, size_t length, int prot,
                                size_t dead_low, size_t dead_high) {
	return GC_mmapAnonFlags_safe_protect (start, length,
	                                      prot, 0,
	                                      dead_low, dead_high);
}

size_t GC_pageSize (void) {
        return 256;
}

uintmax_t GC_physMem (void) {
        return _ml_heap_end - _ml_heap_start;
}


void GC_release (void *base, size_t length) {
        die ("Operation `%s` unsupported on baremetal platform.\n", __func__);
}

void *GC_mmapAnon (void *start, size_t length) {
        die ("Operation `%s` unsupported on baremetal platform.\n", __func__);
}

void *GC_mmapAnonFlags (void *start, size_t length, int flags) {
	die ("Operation `%s` unsupported on baremetal platform.\n", __func__);
}

void *GC_mmapAnonStack (void *start, size_t length, int prot,
                        size_t dead_low, size_t dead_high) {
	return GC_mmapAnonFlags_safe_protect (start, length, prot, 0,
	                                      dead_low, dead_high);
}

int getrusage(int who, struct rusage *r_usage) {
        memset(r_usage, 0, sizeof(struct rusage));
        return 0;
}
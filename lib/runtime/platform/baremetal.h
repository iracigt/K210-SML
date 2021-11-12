#include <inttypes.h>
#include <fenv.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/resource.h>

#define HAS_FEROUND TRUE
#define HAS_MSG_DONTWAIT TRUE
#define HAS_REMAP FALSE
#define HAS_SIGALTSTACK FALSE
#define NEEDS_SIGALTSTACK_EXEC FALSE
#define HAS_SPAWN FALSE
#define HAS_TIME_PROFILING FALSE

#define MLton_Platform_OS_host "baremetal"

extern const char *_ml_heap_start;
extern const char *_ml_heap_end;
extern bool _ml_heap_inuse;


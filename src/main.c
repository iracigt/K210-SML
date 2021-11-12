/* Copyright 2018 Canaan Inc.	
 *	
 * Licensed under the Apache License, Version 2.0 (the "License");	
 * you may not use this file except in compliance with the License.	
 * You may obtain a copy of the License at	
 *	
 *     http://www.apache.org/licenses/LICENSE-2.0	
 *	
 * Unless required by applicable law or agreed to in writing, software	
 * distributed under the License is distributed on an "AS IS" BASIS,	
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.	
 * See the License for the specific language governing permissions and	
 * limitations under the License.	
 */

#include <stdio.h>	
#include <bsp.h>
#include <gpio.h>
#include <fpioa.h>

#define RAM_BASE_ADDR       (0x80000000U)
#define RAM_SIZE            (6 * 1024 * 1024U)

extern char _heap_start[];
extern char _heap_end[];

int ml_main(int argc, char* argv[]);

const char *_ml_heap_start = _heap_end;
const char *_ml_heap_end = (void*)RAM_BASE_ADDR + (4 * 1024 * 1024U);
bool _ml_heap_inuse = FALSE;


 int core1_function(void *ctx)	
{	
    uint64_t core = current_coreid();
    printf("Core %ld Hello world\n", core);	
    while(1) {
        msleep(1000);
        printf("peek-a-boo!\n");
    }
}	
static int dalign;
typedef struct
Akind {
	char *name;
	int   kind;
	} Akind;

static Akind
IEEE_8087	= { "IEEE_8087", 1 },
IEEE_MC68k	= { "IEEE_MC68k", 2 },
IBM		= { "IBM", 3 },
VAX		= { "VAX", 4 },
CRAY		= { "CRAY", 5};

 static Akind *
Lcheck(void)
{
	union {
		double d;
		long L[2];
		} __attribute__((__may_alias__)) u;
	struct {
		double d;
		long L;
		} x[2];

	if (sizeof(x) > 2*(sizeof(double) + sizeof(long)))
		dalign = 1;
	u.L[0] = u.L[1] = 0;
	u.d = 1e13;
	if (u.L[0] == 1117925532 && u.L[1] == -448790528)
		return &IEEE_MC68k;
	if (u.L[1] == 1117925532 && u.L[0] == -448790528)
		return &IEEE_8087;
	if (u.L[0] == -2065213935 && u.L[1] == 10752)
		return &VAX;
	if (u.L[0] == 1267827943 && u.L[1] == 704643072)
		return &IBM;
	return 0;
	}

 static Akind *
icheck(void)
{
	union {
		double d;
		int L[2];
		} __attribute__((__may_alias__)) u;
	struct {
		double d;
		int L;
		} x[2];

	if (sizeof(x) > 2*(sizeof(double) + sizeof(int)))
		dalign = 1;
	u.L[0] = u.L[1] = 0;
	u.d = 1e13;
	if (u.L[0] == 1117925532 && u.L[1] == -448790528)
		return &IEEE_MC68k;
	if (u.L[1] == 1117925532 && u.L[0] == -448790528)
		return &IEEE_8087;
	if (u.L[0] == -2065213935 && u.L[1] == 10752)
		return &VAX;
	if (u.L[0] == 1267827943 && u.L[1] == 704643072)
		return &IBM;
	return 0;
	}

 static Akind *
ccheck(int ac, char **av)
{
	union {
		double d;
		long L;
		} __attribute__((__may_alias__)) u;
	long Cray1;

	/* Cray1 = 4617762693716115456 -- without overflow on non-Crays */
	/* The next three tests should always be true. */
	Cray1 = ac >= -2 ? 4617762 : 0;
	if (ac >= -1)
		Cray1 = 1000000*Cray1 + 693716;
	if (av || ac >= 0)
		Cray1 = 1000000*Cray1 + 115456;
	u.d = 1e13;
	if (u.L == Cray1)
		return &CRAY;
	return 0;
	}

 static int
fzcheck(void)
{
	double a, b;
	int i;

	a = 1.;
	b = .1;
	for(i = 155;; b *= b, i >>= 1) {
		if (i & 1) {
			a *= b;
			if (i == 1)
				break;
			}
		}
	b = a * a;
	return b == 0.;
	}

void arith()
{
	Akind *a = 0;
	int Ldef = 0;

	if (sizeof(double) == 2*sizeof(long))
		a = Lcheck();
	else if (sizeof(double) == 2*sizeof(int)) {
		Ldef = 1;
		a = icheck();
		}
	else if (sizeof(double) == sizeof(long))
		a = ccheck(1, NULL);
	if (a) {
		printf("#define %s\n#define Arith_Kind_ASL %d\n",
			a->name, a->kind);
		if (Ldef)
			printf("#define Long int\n#define Intcast (int)(long)\n");
		if (dalign)
			printf("#define Double_Align\n");
		if (sizeof(char*) == 8)
			printf("#define X64_bit_pointers\n");
		if (sizeof(long long) < 8)
			printf("#define NO_LONG_LONG\n");
		if (a->kind <= 2 && fzcheck())
			printf("#define Sudden_Underflow\n");
		} 
    else
	    printf("/* Unknown arithmetic */\n");
	}

int qnan(void) {
#ifdef HAVE_IEEE
	typedef union {
		float f;
		double d;
		Ulong L[4];
#ifndef NO_LONG_LONG
		unsigned short u[5];
		long double D;
#endif
		} __attribute__((__may_alias__)) U;
	U a, b, c;
#ifdef Gen_ld_QNAN
	int i;
#endif

	a.L[0] = b.L[0] = 0x7f800000;
	c.f = a.f - b.f;
	printf("#define f_QNAN 0x%lx\n", UL (c.L[0] & 0x7fffffff));
	a.L[_0] = b.L[_0] = 0x7ff00000;
	a.L[_1] = b.L[_1] = 0;
	c.d = a.d - b.d;	/* quiet NaN */
	c.L[_0] &= 0x7fffffff;
	printf("#define d_QNAN0 0x%lx\n", UL c.L[_0]);
	printf("#define d_QNAN1 0x%lx\n", UL c.L[_1]);
#ifdef Gen_ld_QNAN
	if (sizeof(a.D) >= 16) {
		b.D = c.D = a.d;
		if (printf("") < 0)
			c.D = 37;	/* never executed; just defeat optimization */
		a.L[0] = a.L[1] = a.L[2] = a.L[3] = 0;
		a.D = b.D - c.D;
		a.L[_3] &= 0x7fffffff;
		for(i = 0; i < 4; i++)
			printf("#define ld_QNAN%d 0x%lx\n", i, UL a.L[perm[i]]);
		}
#endif
#endif /* HAVE_IEEE */
	return 0;
}

void hello(int i) {
    printf("hello %d\n", i);
}

void delay(int ms) {
    msleep(ms);
}

void gpio_mode(int pin, int mode) {
    gpio_set_drive_mode(pin, mode);
}

void gpio_write(int pin, int val) {
    gpio_set_pin(pin, val);
}

void set_pin_function(int pin, int func) {
    fpioa_set_function(pin, func);
}

void Posix_Process_exit (int status) {
	printf("Exiting with status %d.\n", status);
	exit(status);
}

int main()
{	
    uint64_t core = current_coreid();
    msleep(500);
    printf("Core %ld Hello world\n", core);

    printf("main            = %p\n", main);
    printf("_heap_start     = %p\n", _heap_start);
    printf("_heap_end       = %p\n", _heap_end);
    printf("_ml_heap_start  = %p\n", _ml_heap_start);
    printf("_ml_heap_end    = %p\n", _ml_heap_end);

    int *i = malloc(sizeof(int));
    printf("i               = %p\n", i);
    printf("RAM_BEGIN       = %p\n", (void*)RAM_BASE_ADDR);
    printf("RAM_END         = %p\n", (void*)RAM_BASE_ADDR + RAM_SIZE);
    free(i);

    // printf("\n----- ARITH CHECKS -----\n\n");
    // arith();

    // printf("\n----- QNAN CHECKS -----\n\n");
    // qnan();

    // register_core1(core1_function, NULL);
    ml_main (0, NULL);
    while(1);
}
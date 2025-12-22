#ifndef RUNTIME_INTERFACE_H
#define RUNTIME_INTERFACE_H

extern "C" {
#define _Noreturn [[noreturn]]

#include "gc.h"
#include "runtime.h"

extern size_t __gc_stack_top;
extern size_t __gc_stack_bottom;

extern aint Llength(void* p);

extern aint LtagHash(char* s);

extern void* Belem(void* p, aint i);

extern void* Bstring(aint* args);

extern void* Lstring(aint* args);

extern void* Bclosure(aint* args, aint bn);

extern void* Barray(aint* args, aint bn);

extern void* Bsexp(aint* args, aint bn);

extern aint Btag(void* d, aint t, aint n);

extern aint Barray_patt(void* d, aint n);

extern aint Bstring_patt(void* x, void* y);

extern aint Bclosure_tag_patt(void* x);

extern aint Bboxed_patt(void* x);

extern aint Bunboxed_patt(void* x);

extern aint Barray_tag_patt(void* x);

extern aint Bstring_tag_patt(void* x);

extern aint Bsexp_tag_patt(void* x);

extern void* Bsta(void* x, aint i, void* v);

extern void Bmatch_failure(void* v, char* fname, aint line, aint col);

extern aint Lread();

extern aint Lwrite(aint n);
}

#endif

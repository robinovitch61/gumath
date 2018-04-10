/*
* BSD 3-Clause License
*
* Copyright (c) 2017-2018, plures
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <inttypes.h>
#include "ndtypes.h"
#include "xnd.h"
#include "gumath.h"


/* Avoid Python.h dependency. */
struct _object;
double PyFloat_AsDouble(const struct _object *);
struct _object *PyErr_Occurred(void);


/*****************************************************************************/
/*                Initialize a single bfloat16 from a PyObject               */
/*****************************************************************************/

static bool
bfloat16_init(void *dest, void *src, ndt_context_t *ctx)
{
    const struct _object *v = (struct _object *)src;
    xnd_t *x = (xnd_t *)dest;
    const ndt_t *t = x->type;
    float f;
    (void)ctx;

    f = (float)PyFloat_AsDouble(v);
    if (f == -1 && PyErr_Occurred()) {
        return 0;
    }

    const char *cp = ndt_is_big_endian(t) ? (char *)&f : ((char *)&f)+2;
    memcpy(x->ptr, cp, sizeof(uint16_t));
    return 1;
}

static const gm_typedef_init_t typedefs[] = {
  { .name = "bfloat16", .type = "uint16", .init=bfloat16_init, .constraint=NULL },
  { .name = NULL, .type = NULL, .init=NULL, .constraint=NULL }
};

static const gm_kernel_init_t kernels[] = {
  { .name = NULL, .sig = NULL }
};


/****************************************************************************/
/*                       Initialize kernel table                            */
/****************************************************************************/

int
gm_init_bfloat16_kernels(ndt_context_t *ctx)
{
    const gm_typedef_init_t *t;
    const gm_kernel_init_t *k;

    for (t = typedefs; t->name != NULL; t++) {
        if (ndt_typedef_from_string(t->name, t->type, t->init, t->constraint, ctx) < 0) {
            return -1;
        }
    }

    for (k = kernels; k->name != NULL; k++) {
        if (gm_add_kernel(k, ctx) < 0) {
            return -1;
        }
    }

    return 0;
}
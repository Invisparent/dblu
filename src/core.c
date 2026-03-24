/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#include <stdatomic.h>

#include "ivs/random.h"

#include "dblu/core.h"


const ivs_allocator *dblu_palloc;

void
dblu_init(
    void
) {
    dblu_palloc = ivs_palloc;
}


static _Atomic unsigned dblu_rng_seed = IVS_RANDOM_SEED;

float
dblu_rng_gen(
    void
) {
    static _Thread_local _Bool tls = 0;
    static _Thread_local ivs_xorshift_64 gen;
    if (!tls) {
        const unsigned seed = atomic_fetch_add_explicit(&dblu_rng_seed, 1, memory_order_relaxed);
        ivs_xorshift_64_init(&gen, seed);
        tls = 1;
    }
    return ivs_rng_u32_to_flt(ivs_xorshift_64_u32(&gen));
}

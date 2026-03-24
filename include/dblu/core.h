/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#ifndef DBLU_CORE_H
#define DBLU_CORE_H


#define DBLU_MAJOR 2
#define DBLU_UPDATE 0
#define DBLU_PATCH 0

#define DBLU_MAJOR_ESSENTIAL "Bennett"
#define DBLU_MAJOR_FULL "\"Bennett\""


#include "ivs/memory.h"


extern const ivs_allocator *dblu_palloc;


void
dblu_init(
    void
);


float
dblu_rng_gen(
    void
);


#endif

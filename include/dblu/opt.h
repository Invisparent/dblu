/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#ifndef DBLU_OPT_H
#define DBLU_OPT_H


#include "ivs/array_list.h"

#include "dblu/art.h"


typedef struct dblu_opt_data {
    _Bool exist;
    _Bool evaluated;
    dblu_art dat;
}dblu_opt_data;


typedef struct dblu_opt_candidates {
    ivs_array_list native[5]; // array_list<dblu_opt_data>
    ivs_array_list other[5]; // array_list<dblu_opt_data>
    float max;
}dblu_opt_candidates;


void
dblu_opt_candidates_init(
    dblu_opt_candidates *self
);

void
dblu_opt_candidates_free(
    const dblu_opt_candidates *self
);

void
dblu_opt_candidates_update(
    dblu_opt_candidates *self,
    const dblu_art *art,
    uint32_t affected,
    _Bool native
);

float
dblu_opt_candidates_optimal(
    dblu_opt_candidates *self,
    float (*formula)(const dblu_art_gain *, void *),
    void *opt_fd
);


#endif

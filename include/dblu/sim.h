/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#ifndef DBLU_SIM_H
#define DBLU_SIM_H


#include <stdint.h>

#include <pthread.h>

#include "ivs/array_list.h"

#include "dblu/art.h"


typedef struct dblu_sim_sample_point {
    pthread_mutex_t mtx;
    ivs_array_list dat; // array_list<float>
} dblu_sim_sample_point;


typedef struct dblu_sim_generator_result {
    _Bool native;
    dblu_art art;
} dblu_sim_generator_result;


typedef struct dblu_sim {
    dblu_sim_sample_point *result;
    unsigned length;
    _Atomic unsigned now;
    unsigned sample_num;
    unsigned gap;
    float (*formula)(const dblu_art_gain *, void *);
    dblu_sim_generator_result (*generator)(void *);
    uint32_t affected;
    unsigned thrd_num;
    _Atomic unsigned thrd_done;
    void *internal_res;
} dblu_sim;


void
dblu_sim_sample_point_init(
    dblu_sim_sample_point *self
);

void
dblu_sim_sample_point_lock(
    dblu_sim_sample_point *self
);

void
dblu_sim_sample_point_unlock(
    dblu_sim_sample_point *self
);

ivs_array_list *
dblu_sim_sample_point_data(
    dblu_sim_sample_point *self
);

void
dblu_sim_sample_point_free(
    dblu_sim_sample_point *self
);


void
dblu_sim_init(
    dblu_sim *self,
    unsigned length,
    unsigned sample_num,
    unsigned gap,
    unsigned thrd_num,
    float (*formula)(const dblu_art_gain *, void *),
    dblu_sim_generator_result (*generator)(void *),
    uint32_t affected
);

void
dblu_sim_run(
    dblu_sim *self,
    void *const *opt_formula_fd,
    void *const*opt_generator_fd
);

void
dblu_sim_free(
    dblu_sim *self
);


#endif

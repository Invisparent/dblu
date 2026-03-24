/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#include <stdatomic.h>
#include <stdlib.h>

#include "dblu/core.h"
#include "dblu/opt.h"
#include "dblu/sim.h"


void
dblu_sim_sample_point_init(
    dblu_sim_sample_point *const self
) {
    ivs_assert(self);
    pthread_mutex_init(&self->mtx, NULL);
    ivs_array_list_init(&self->dat);
}

void
dblu_sim_sample_point_lock(
    dblu_sim_sample_point *const self
) {
    ivs_assert(self);
    pthread_mutex_lock(&self->mtx);
}

void
dblu_sim_sample_point_unlock(
    dblu_sim_sample_point *const self
) {
    ivs_assert(self);
    pthread_mutex_unlock(&self->mtx);
}

ivs_array_list *
dblu_sim_sample_point_data(
    dblu_sim_sample_point *const self
) {
    ivs_assert(self);
    return &self->dat;
}

void
dblu_sim_sample_point_free(
    dblu_sim_sample_point *const self
) {
    ivs_assert(self);
    pthread_mutex_destroy(&self->mtx);
    ivs_array_list_free(&self->dat, dblu_palloc);
}


void
dblu_sim_init(
    dblu_sim *const self,
    const unsigned length,
    const unsigned sample_num,
    const unsigned gap,
    const unsigned thrd_num,
    float (*const formula)(const dblu_art_gain *, void *),
    dblu_sim_generator_result (*const generator)(void *),
    const uint32_t affected
) {
    ivs_assert(self && formula && generator);
    ivs_assert(length);
    ivs_assert(length > gap);
    ivs_assert(gap);
    ivs_assert(sample_num);
    ivs_assert(thrd_num);
    const unsigned sz = length / gap;
    self->result = ivs_allocator_malloc(dblu_palloc, sizeof(dblu_sim_sample_point) * sz);
    for (unsigned t = 0; t < sz; ++t)
        dblu_sim_sample_point_init(&self->result[t]);
    self->length = length;
    self->sample_num = sample_num;
    self->gap = gap;
    self->formula = formula;
    self->generator = generator;
    self->affected = affected;
    self->internal_res = NULL;
    self->thrd_num = thrd_num;
    atomic_store_explicit(&self->now, 0, memory_order_relaxed);
    atomic_store_explicit(&self->thrd_done, 0, memory_order_relaxed);
}

typedef struct dblu_static_sim_run_thrd_fd {
    dblu_sim *self;
    _Atomic unsigned thrd_id;
    void *const *opt_formula_fd;
    void *const *opt_generator_fd;
} dblu_static_sim_run_thrd_fd;

static
void *
dblu_static_sim_run_thrd(
    void *const fd
) {
    ivs_assert(fd);
    dblu_static_sim_run_thrd_fd *const p = fd;
    dblu_sim *const self = p->self;
    ivs_assert(self);
    const unsigned id = atomic_fetch_add(&p->thrd_id, 1);
    void *const formula_fd = p->opt_formula_fd[id];
    void *const generator_fd = p->opt_generator_fd[id];
    while (1) {
        dblu_opt_candidates cand;
        dblu_opt_candidates_init(&cand);
        const unsigned now = atomic_fetch_add(&self->now, 1);
        if (now >= self->sample_num) {
            dblu_opt_candidates_free(&cand);
            atomic_fetch_add(&self->thrd_done, 1);
            pthread_exit(NULL);
        }
        for (unsigned t = 1; t <= self->length; ++t) {
            const dblu_sim_generator_result r1 = self->generator(generator_fd);
            dblu_opt_candidates_update(&cand, &r1.art, self->affected, r1.native);
            const unsigned pt = t % self->gap;
            if (!pt) {
                const float r2 = dblu_opt_candidates_optimal(&cand, self->formula, formula_fd);
                const unsigned index = t / self->gap - 1;
                pthread_mutex_lock(&self->result[index].mtx);
                ivs_array_list_push_back(dblu_sim_sample_point_data(&self->result[index]), sizeof(float), dblu_palloc,
                                         &r2);
                pthread_mutex_unlock(&self->result[index].mtx);
            }
        }
        dblu_opt_candidates_free(&cand);
    }
}

void
dblu_sim_run(
    dblu_sim *const self,
    void *const *const opt_formula_fd,
    void *const*const opt_generator_fd
) {
    ivs_assert(self);
    self->internal_res = ivs_allocator_malloc(dblu_palloc, sizeof(dblu_static_sim_run_thrd_fd));
    dblu_static_sim_run_thrd_fd *fd = self->internal_res;
    fd->self = self;
    fd->opt_formula_fd = opt_formula_fd;
    fd->opt_generator_fd = opt_generator_fd;
    atomic_store(&fd->thrd_id, 0);
    const unsigned thrd_num = self->thrd_num;
    for (unsigned t = 0; t < thrd_num; ++t) {
        pthread_t th;
        if (pthread_create(&th, NULL, dblu_static_sim_run_thrd, fd))
            exit(-1);
        pthread_detach(th);
    }
}

void
dblu_sim_free(
    dblu_sim *const self
) {
    ivs_assert(self);
    const unsigned sz = self->length / self->gap;
    for (unsigned t = 0; t < sz; ++t)
        dblu_sim_sample_point_free(&self->result[t]);
    ivs_allocator_free(dblu_palloc, self->result);
    ivs_allocator_free(dblu_palloc, self->internal_res);
}

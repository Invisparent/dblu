/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "ivs/functional.h"

#include "dblu/wildcard.h"


int main(void) {
    // init dblu
    dblu_init();
    // parallel threads
    unsigned th;
    // stop the simulation after how many artifacts we've got
    unsigned len;
    // how many samples we'll get
    unsigned sample_num;
    // how often we update the maximum value using "formula" we can achieve
    unsigned gap;
    FILE *const os = fopen("./o", "w");
    if (!os)
        return -1;
    scanf("%u%u%u%u", &th, &len, &sample_num, &gap);
    dblu_sim sim;
    // Init the simulation, providing dblu_formula_akasha_furina_es_dmg_avg_4gt as the "formula".
    // That is, formula takes an artifact gain as input, and returns a value based on the artifact set.
    //   (It's often damage, hp, artifact score, etc.)
    // And also provides a "generator" dblu_generator_reserve_crits.
    // That is, generator will generate artifacts based on some rules.
    // For example, dblu_generator_reserve_crits assumes you get artifacts from artifact domain,
    //   and put those artifacts that don't have crit rate or crit dmg to the crafting bench recursively.
    // dblu_affected_furina_es_dmg_avg_4gt specifies which artifact affixes will affect the "formula".
    // For Furina, it'll be HPs, CRITs and Hydro Damage Bonus.
    // So it's set to:
    //   (uint32_t) 1 << dblu_art_affix_cr |
    //   (uint32_t) 1 << dblu_art_affix_cd |
    //   (uint32_t) 1 << dblu_art_affix_hp |
    //   (uint32_t) 1 << dblu_art_affix_hv |
    //   (uint32_t) 1 << dblu_art_affix_edh;
    dblu_sim_init(&sim, len, sample_num, gap, th,
                  dblu_formula_akasha_furina_es_dmg_avg_4gt, dblu_generator_reserve_crits,
                  dblu_affected_furina_es_dmg_avg_4gt);
    // allocate resources formula and generator require.
    void **const ffd = ivs_allocator_calloc(dblu_palloc, th, sizeof(void *));
    void **const gfd = ivs_allocator_calloc(dblu_palloc, th, sizeof(void *));
    for (unsigned t = 0; t < th; ++t)
        ffd[t] = dblu_formula_akasha_furina_es_dmg_avg_4gt_fd();
    for (unsigned t = 0; t < th; ++t)
        gfd[t] = dblu_generator_reserve_crits_fd();
    const struct timespec tm = {1, 0};
    // run the simulation
    dblu_sim_run(&sim, ffd, gfd);
    while (1) {
        nanosleep(&tm, NULL);
        const unsigned now = atomic_load(&sim.now);
        if (now >= sim.sample_num)
            break;
        printf("%u\n", now);
        fflush(stdout);
    }
    while (1) {
        // sim.thrd_done == sim.thrd_num hints all workers are done.
        if (atomic_load(&sim.thrd_done) == sim.thrd_num)
            break;
        nanosleep(&tm, NULL);
    }
    // sort and output the result
    const unsigned sz = sim.length / sim.gap;
    for (unsigned t = 0; t < sz; ++t) {
        const ivs_array_list *const al = dblu_sim_sample_point_data(&sim.result[t]);
        qsort(ivs_array_list_data(al), sample_num, sizeof(float), ivs_functional_cmp_flt);
    }
    for (unsigned t = 0; t < sz; ++t) {
        fprintf(os, "%u ", (t + 1) * sim.gap);
        ivs_array_list *const al = dblu_sim_sample_point_data(&sim.result[t]);
        ivs_iterator iter = ivs_array_list_begin(al);
        while (!ivs_iterator_empty(&iter)) {
            const float val = *(const float *) ivs_array_list_deref(al, iter);
            fprintf(os, "%f", val);
            iter = ivs_array_list_advance(al, sizeof(float), iter, 1);
            if (!ivs_iterator_empty(&iter))
                fprintf(os, " ");
        }
        if (t != sz - 1)
            fputc('\n', os);
    }
    return 0;
}

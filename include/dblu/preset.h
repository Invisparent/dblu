/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#ifndef DBLU_PRESET_H
#define DBLU_PRESET_H


#include "./art.h"
#include "./sim.h"


void *
dblu_generator_ctable_fd(
    void
);

dblu_sim_generator_result
dblu_generator_ctable(
    void *fd
);


void *
dblu_generator_abyss_fd(
    void
);

dblu_sim_generator_result
dblu_generator_abyss(
    void *fd
);


void *
dblu_generator_reserve_crits_fd(
    void
);

dblu_sim_generator_result
dblu_generator_reserve_crits(
    void *fd
);


float
dblu_formula_crit_score(
    const dblu_art_gain *gain,
    void *fd
);

extern const uint32_t dblu_affected_crit_score;


float
dblu_formula_crit_rate(
    const dblu_art_gain *gain,
    void *fd
) ;

extern const uint32_t dblu_affected_crit_rate;


float dblu_formula_crit_dmg(
    const dblu_art_gain *gain,
    void *fd
);

extern const uint32_t dblu_affected_crit_dmg;


void *
dblu_formula_akasha_furina_es_dmg_avg_4gt_fd(
    void
);

float
dblu_formula_akasha_furina_es_dmg_avg_4gt(
    const dblu_art_gain *gain,
    void *fd
);

extern const uint32_t dblu_affected_furina_es_dmg_avg_4gt;


#endif

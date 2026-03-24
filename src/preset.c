/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#include "ivs/core.h"

#include "dblu/art.h"
#include "dblu/core.h"
#include "dblu/sim.h"


void *
dblu_generator_ctable_fd(
    void
) {
    return NULL;
}

dblu_sim_generator_result
dblu_generator_ctable(
    void *const fd
) {
    (void) fd;
    dblu_sim_generator_result ret;
    ret.native = 1;
    const float u = dblu_rng_gen();
    if (u < 1.f / 3.f)
        ret.art = dblu_art_make(1);
    else
        ret.art = dblu_art_make(0);
    return ret;
}


void *
dblu_generator_abyss_fd(
    void
) {
    return NULL;
}

dblu_sim_generator_result
dblu_generator_abyss(
    void *const fd
) {
    (void) fd;
    dblu_sim_generator_result ret;
    const float u = dblu_rng_gen();
    if (u < .5f)
        ret.native = 1;
    else
        ret.native = 0;
    const float v = dblu_rng_gen();
    if (v < .25f)
        ret.art = dblu_art_make(1);
    else
        ret.art = dblu_art_make(0);
    return ret;
}


void *
dblu_generator_reserve_crits_fd(
    void
) {
    unsigned *const ret = ivs_allocator_malloc(dblu_palloc, sizeof(unsigned));
    *ret = 0;
    return ret;
}

dblu_sim_generator_result
dblu_generator_reserve_crits(
    void *const fd
) {
    ivs_assert(fd);
    unsigned *const p = fd;
    dblu_sim_generator_result ret;
    if (*p >= 3)
        ret = dblu_generator_ctable(NULL);
    else
        ret = dblu_generator_abyss(NULL);
    const uint32_t aff = dblu_art_affixes(&ret.art);
    if (aff & (dblu_art_affix_cr | dblu_art_affix_cd))
        ;
    else
        ++*p;
    return ret;
}


float
dblu_formula_crit_score(
    const dblu_art_gain *const gain,
    void *const fd
) {
    ivs_assert(gain);
    (void) fd;
    return gain->dat[dblu_art_affix_cr] * 2.f + gain->dat[dblu_art_affix_cd];
}

const uint32_t dblu_affected_crit_score = dblu_art_affix_cr | dblu_art_affix_cd;


float
dblu_formula_crit_rate(
    const dblu_art_gain *const gain,
    void *const fd
) {
    ivs_assert(gain);
    (void) fd;
    return gain->dat[dblu_art_affix_cr];
}

const uint32_t dblu_affected_crit_rate = dblu_art_affix_cr;


float dblu_formula_crit_dmg(
    const dblu_art_gain *const gain,
    void *const fd
) {
    ivs_assert(gain);
    (void) fd;
    return gain->dat[dblu_art_affix_cd];
}

const uint32_t dblu_affected_crit_dmg = dblu_art_affix_cd;


typedef struct dblu_static_character_stat {
    float hp;
    float atk;
    float def;
    float emstr;
    float crate;
    float cdmg;
} dblu_static_character_stat;


dblu_static_character_stat
dblu_static_character_stat_add(
    const dblu_static_character_stat *const self,
    const dblu_static_character_stat *const rhs
) {
    ivs_assert(self && rhs);
    const dblu_static_character_stat ret = {
        .hp = self->hp + rhs->hp,
        .atk = self->atk + rhs->atk,
        .def = self->def + rhs->def,
        .emstr = self->emstr + rhs->emstr,
        .crate = self->crate + rhs->crate,
        .cdmg = self->cdmg + rhs->cdmg
    };
    return ret;
}

dblu_static_character_stat
dblu_static_character_stat_from_art_gain(
    const dblu_static_character_stat *const base,
    const dblu_art_gain *const gain
) {
    ivs_assert(base && gain);
    const dblu_static_character_stat ret = {
        .hp = base->hp + base->hp * gain->dat[dblu_art_affix_hp] / 100.f + gain->dat[dblu_art_affix_hv],
        .atk = base->atk + base->atk * gain->dat[dblu_art_affix_ap] / 100.f + gain->dat[dblu_art_affix_av],
        .def = base->def + base->def * gain->dat[dblu_art_affix_dp] / 100.f + gain->dat[dblu_art_affix_dv],
        .emstr = base->emstr + gain->dat[dblu_art_affix_em],
        .crate = base->crate + gain->dat[dblu_art_affix_cr],
        .cdmg = base->cdmg + gain->dat[dblu_art_affix_cd]
    };
    return ret;
}


/**
 * Data sources:
 *   Genshin Impact, Version 6.4 or later, on Windows.
 *   https://yuhengcup.wiki and its mirror site
 *   https://genshin-impact.fandom.com
 */


/**
 * Genshin Leaderboards, Akasha System.
 * https://akasha.cv/leaderboards
 *   Character leaderboards are meant to compare artifacts strength only.
 *   Your character's current level, talents, constellation, weapon and refinement do not matter.
 *   Before calculating their ranking, characters are reset to the same state.
 *   (usually high f2p investment state:
 *   level 90/90, crowned talent, low constellation, 5* weapon at refinement 1 etc)
 *   and placed under exact same team buffs. Please read each leaderboard's description for more details.
 */


/**
 * https://akasha.cv/leaderboards/1000008900
 *   Furina - Elemental Skill, Avg DMG
 *     Average DMG for Mademoiselle Crabaletta in a double hydro team.
 *     Number of characters who offered HP: 4.
 *     Fanfare stacks count: 200.
 *     Weapon stacks maxed when applicable.
 *     Weapon: R1 Splendor of Tranquil Water.
 */

void *
dblu_formula_akasha_furina_es_dmg_avg_4gt_fd(
    void
) {
    return NULL;
}

float
dblu_formula_akasha_furina_es_dmg_avg_4gt(
    const dblu_art_gain *const gain,
    void *const fd
) {
    ivs_assert(gain);
    (void) fd;
    const dblu_static_character_stat base = {
        .hp = 15307.39f,
        .atk = 243.96f
             + 542.f, // Splendor of Tranquil Waters
        .def = 695.54f,
        .crate = 24.2f,
        .cdmg = 50.f
              + 88.2f // Splendor of Tranquil Waters
    };
    dblu_art_gain gain_ext = {};
    gain_ext.dat[dblu_art_affix_hp] = 25.f // double hydro team
                                    + 28.f; // R1 Splendor of Tranquil Waters, stack maxed.
    const dblu_art_gain gain_final = dblu_art_gain_add(&gain_ext, gain);
    const dblu_static_character_stat final = dblu_static_character_stat_from_art_gain(&base, &gain_final);
    const float dm = 1.f
                   + .7f // 4gt
                   + .4f // number of characters who offered HP: 4
                   + .24f // Splendor of Tranquil Waters
                   + (gain->dat[dblu_art_affix_edh] / 100.f) // hydro damage boost
                   + (200.f * .25e-2f) // fanfare stack count: 200
                   + ivs_inline_min(.28f, final.hp * .7f / 1000.f);
    const float dmg = .149184f * final.hp; // Mademoiselle Crabaletta
    const float ex = 1.f + ivs_inline_min(1.f, final.crate / 100.f) * (final.cdmg / 100.f); // expectation
    const float ret = dm * dmg * ex;
    const float magic = 1.786f;
    return ret / magic;
}

const uint32_t dblu_affected_furina_es_dmg_avg_4gt = (uint32_t) 1 << dblu_art_affix_cr |
                                                     (uint32_t) 1 << dblu_art_affix_cd |
                                                     (uint32_t) 1 << dblu_art_affix_hp |
                                                     (uint32_t) 1 << dblu_art_affix_hv |
                                                     (uint32_t) 1 << dblu_art_affix_edh;

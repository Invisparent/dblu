/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#ifndef DBLU_ART_H
#define DBLU_ART_H


#include <stdint.h>


typedef enum dblu_art_pos {
    dblu_art_pos_flower,
    dblu_art_pos_plume,
    dblu_art_pos_sand,
    dblu_art_pos_goblet,
    dblu_art_pos_circlet,
    dblu_art_pos_enum_length
} dblu_art_pos;


typedef enum dblu_art_affix {
    dblu_art_affix_hv,
    dblu_art_affix_av,
    dblu_art_affix_dv,
    dblu_art_affix_hp,
    dblu_art_affix_ap,
    dblu_art_affix_dp,
    dblu_art_affix_em,
    dblu_art_affix_er,
    dblu_art_affix_cr,
    dblu_art_affix_cd,
    dblu_art_affix_rg,
    dblu_art_affix_eda,
    dblu_art_affix_edc,
    dblu_art_affix_edd,
    dblu_art_affix_ede,
    dblu_art_affix_edg,
    dblu_art_affix_edh,
    dblu_art_affix_edp,
    dblu_art_affix_phy,
    dblu_art_affix_enum_length
} dblu_art_affix;


typedef struct dblu_art_attr {
    dblu_art_affix aff;
    float val;
} dblu_art_attr;


typedef struct dblu_art {
    dblu_art_pos pos;
    dblu_art_attr attr[5];
} dblu_art;


dblu_art
dblu_art_make(
    _Bool init_4th
);

uint32_t
dblu_art_affixes(
    const dblu_art *self
);


typedef struct dblu_art_gain {
    float dat[dblu_art_affix_enum_length];
}dblu_art_gain;


_Bool
dblu_art_gain_partial_less_equal(
    const dblu_art_gain *self,
    const dblu_art_gain *rhs,
    uint32_t affected
);

_Bool
dblu_art_gain_partial_greater_equal(
    const dblu_art_gain *self,
    const dblu_art_gain *rhs,
    uint32_t affected
);


void
dblu_art_gain_init(
    dblu_art_gain *self
);

dblu_art_gain
dblu_art_gain_from_art(
    const dblu_art *input
);

dblu_art_gain
dblu_art_gain_add(
    const dblu_art_gain *self,
    const dblu_art_gain *rhs
);


#endif

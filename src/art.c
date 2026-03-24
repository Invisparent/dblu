/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#include <math.h>
#include <stdint.h>

#include "ivs/core.h"

#include "dblu/art.h"
#include "dblu/core.h"


// NOLINTBEGIN
static const float dblu_static_art_major_constant[dblu_art_affix_enum_length] = {
    4780,
    311,
    NAN,
    46.6,
    46.6,
    58.3,
    186.5,
    51.8,
    31.1,
    62.2,
    35.9,
    46.6,
    46.6,
    46.6,
    46.6,
    46.6,
    46.6,
    46.6,
    58.3
};
// NOLINTEND


// NOLINTBEGIN
static const float dblu_static_art_minor_constant[dblu_art_affix_enum_length] = {
    298.75,
    19.45,
    23.15,
    5.83,
    5.83,
    7.29,
    23.31,
    6.48,
    3.89,
    7.77,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN
};
// NOLINTEND

static
void
dblu_static_art_fill_major_l1(
    dblu_art *const self,
    const dblu_art_affix aff
) {
    self->attr[0].aff = aff;
    self->attr[0].val = dblu_static_art_major_constant[aff];
}

static
void
dblu_static_art_fill_major(
    dblu_art *const self
) {
    ivs_assert(self);
    const float u = dblu_rng_gen();
    if (u < .2f)
        self->pos = dblu_art_pos_flower;
    else if (u < .4f)
        self->pos = dblu_art_pos_plume;
    else if (u < .6f)
        self->pos = dblu_art_pos_sand;
    else if (u < .8f)
        self->pos = dblu_art_pos_goblet;
    else
        self->pos = dblu_art_pos_circlet;
    const float v = dblu_rng_gen();
    switch (self->pos) {
        case dblu_art_pos_flower:
            dblu_static_art_fill_major_l1(self, dblu_art_affix_hv);
            break;
        case dblu_art_pos_plume:
            dblu_static_art_fill_major_l1(self, dblu_art_affix_av);
            break;
        case dblu_art_pos_sand:
            if (v < .8f / 3.f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_hp);
            else if (v < 1.6f / 3.f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_ap);
            else if (v < 2.4f / 3.f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_dp);
            else if (v < .9f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_er);
            else
                dblu_static_art_fill_major_l1(self, dblu_art_affix_em);
            break;
        case dblu_art_pos_goblet:
            if (v < .1925f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_hp);
            else if (v < .385f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_ap);
            else if (v < .575f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_dp);
            else if (v < .625f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_eda);
            else if (v < .675f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_edc);
            else if (v < .725f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_edd);
            else if (v < .775f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_ede);
            else if (v < .825f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_edg);
            else if (v < .875f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_edh);
            else if (v < .925f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_edp);
            else if (v < .975f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_phy);
            else
                dblu_static_art_fill_major_l1(self, dblu_art_affix_em);
            break;
        case dblu_art_pos_circlet:
            if (v < .22f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_hp);
            else if (v < .44f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_ap);
            else if (v < .66f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_dp);
            else if (v < .76f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_cr);
            else if (v < .86f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_cd);
            else if (v < .96f)
                dblu_static_art_fill_major_l1(self, dblu_art_affix_rg);
            else
                dblu_static_art_fill_major_l1(self, dblu_art_affix_em);
            break;
        default:
            ivs_unreachable();
    }
}

static
dblu_art_affix
dblu_static_art_fill_minor_l1(
    void) {
    const float u = dblu_rng_gen();
    if (u < 6.f / 44.f)
        return dblu_art_affix_hv;
    else if (u < 12.f / 44.f)
        return dblu_art_affix_av;
    else if (u < 18.f / 44.f)
        return dblu_art_affix_dv;
    else if (u < 22.f / 44.f)
        return dblu_art_affix_hp;
    else if (u < 26.f / 44.f)
        return dblu_art_affix_ap;
    else if (u < 30.f / 44.f)
        return dblu_art_affix_dp;
    else if (u < 34.f / 44.f)
        return dblu_art_affix_er;
    else if (u < 38.f / 44.f)
        return dblu_art_affix_em;
    else if (u < 41.f / 44.f)
        return dblu_art_affix_cr;
    else
        return dblu_art_affix_cd;
}

static
dblu_art_affix
dblu_static_art_fill_minor_l2(
    const uint32_t flag
) {
    dblu_art_affix ret;
    do
        ret = dblu_static_art_fill_minor_l1();
    while (((uint32_t) 1 << ret) & flag);
    return ret;
}

static
void
dblu_static_art_fill_minor(
    dblu_art *const self
) {
    ivs_assert(self);
    uint32_t flag = self->attr[0].aff;
    for (unsigned t = 1; t <= 4; ++t) {
        const dblu_art_affix r = dblu_static_art_fill_minor_l2(flag);
        self->attr[t].aff = r;
        flag |= (uint32_t) 1 << r;
    }
}

static
void
dblu_static_art_init_minor(
    dblu_art *const self
) {
    ivs_assert(self);
    for (unsigned t = 1; t <= 4; ++t) {
        const float u = dblu_rng_gen();
        float coef;
        if (u < .25f)
            coef = .7f;
        else if (u < .5f)
            coef = .8f;
        else if (u < .75f)
            coef = .9f;
        else
            coef = 1.f;
        self->attr[t].val = dblu_static_art_minor_constant[self->attr[t].aff] * coef;
    }
}

static
void
dblu_static_art_enhance_l1(
    dblu_art *const self,
    const unsigned index
) {
    ivs_assert(self);
    ivs_assert(ivs_inline_between_cc(1, 4, index));
    const float u = dblu_rng_gen();
    float coef;
    if (u < .25f)
        coef = .7f;
    else if (u < .5f)
        coef = .8f;
    else if (u < .75f)
        coef = .9f;
    else
        coef = 1.f;
    self->attr[index].val += dblu_static_art_minor_constant[self->attr[index].aff] * coef;
}

static
void
dblu_static_art_enhance(
    dblu_art *const self,
    const _Bool init_4th
) {
    ivs_assert(self);
    for (unsigned t = 0; t < 4 + init_4th; ++t) {
        const float u = dblu_rng_gen();
        if (u < .25f)
            dblu_static_art_enhance_l1(self, 1);
        else if (u < .5f)
            dblu_static_art_enhance_l1(self, 2);
        else if (u < .75f)
            dblu_static_art_enhance_l1(self, 3);
        else
            dblu_static_art_enhance_l1(self, 4);
    }
}

dblu_art
dblu_art_make(
    const _Bool init_4th
) {
    dblu_art ret = {};
    dblu_static_art_fill_major(&ret);
    dblu_static_art_fill_minor(&ret);
    dblu_static_art_init_minor(&ret);
    dblu_static_art_enhance(&ret, init_4th);
    return ret;
}

uint32_t
dblu_art_affixes(
    const dblu_art *const self
) {
    ivs_assert(self);
    uint32_t ret = 0;
    for (unsigned t = 0; t < 5; ++t)
        ret |= self->attr[t].aff;
    return ret;
}


_Bool
dblu_art_gain_partial_less_equal(
    const dblu_art_gain *const self,
    const dblu_art_gain *const rhs,
    const uint32_t affected
) {
    ivs_assert(self && rhs);
    for (unsigned t = 0; t < dblu_art_affix_enum_length; ++t)
        if (affected & ((uint32_t) 1 << t))
            if (self->dat[t] > rhs->dat[t] + 0x1p-4f)
                return 0;
    return 1;
}

_Bool
dblu_art_gain_partial_greater_equal(
    const dblu_art_gain *const self,
    const dblu_art_gain *const rhs,
    const uint32_t affected
) {
    ivs_assert(self && rhs);
    for (unsigned t = 0; t < dblu_art_affix_enum_length; ++t)
        if (affected & ((uint32_t) 1 << t))
            if (self->dat[t] < rhs->dat[t] - 0x1p-4f)
                return 0;
    return 1;
}

void
dblu_art_gain_init(
    dblu_art_gain *const self
) {
    ivs_assert(self);
    for (unsigned t = 0; t < dblu_art_affix_enum_length; ++t)
        self->dat[t] = 0.f;
}

dblu_art_gain
dblu_art_gain_from_art(
    const dblu_art *const input
) {
    ivs_assert(input);
    dblu_art_gain ret = {};
    for (unsigned t = 0; t < 5; ++t)
        ret.dat[input->attr[t].aff] = input->attr[t].val;
    return ret;
}

dblu_art_gain
dblu_art_gain_add(
    const dblu_art_gain *const self,
    const dblu_art_gain *const rhs
) {
    ivs_assert(self && rhs);
    dblu_art_gain ret;
    for (unsigned t = 0; t < dblu_art_affix_enum_length; ++t)
        ret.dat[t] = self->dat[t] + rhs->dat[t];
    return ret;
}

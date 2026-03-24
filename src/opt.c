/**
 * Copyright (c) 2026 Chenyi Wang.
 *   (a.k.a. Invisparent, an Art, Computer Science and Mathematics Noob.)
 * SPDX-License-Identifier: LGPL-3.0-only
 */


#include "ivs/core.h"

#include "dblu/core.h"
#include "dblu/opt.h"


void
dblu_opt_candidates_init(
    dblu_opt_candidates *const self
) {
    ivs_assert(self);
    for (unsigned t = 0; t < 5; ++t) {
        ivs_array_list_init(&self->native[t]);
        ivs_array_list_init(&self->other[t]);
    }
    self->max = 0.f;
}

void
dblu_opt_candidates_free(
    const dblu_opt_candidates *const self
) {
    ivs_assert(self);
    for (unsigned t = 0; t < 5; ++t) {
        ivs_array_list_free(&self->native[t], dblu_palloc);
        ivs_array_list_free(&self->other[t], dblu_palloc);
    }
}

static
void
dblu_static_opt_candidates_insert(
    ivs_array_list *const set,
    const dblu_art *const art
) {
    ivs_assert(set && art);
    ivs_iterator iter = ivs_array_list_begin(set);
    while (!ivs_iterator_empty(&iter)) {
        dblu_opt_data *const p = ivs_array_list_deref(set, iter);
        if (!p->exist) {
            p->exist = 1;
            p->evaluated = 0;
            p->dat = *art;
            return;
        } else
            iter = ivs_array_list_advance(set, ivs_inline_pad(sizeof(dblu_opt_data)), iter, 1);
    }
    ivs_assert(ivs_iterator_empty(&iter));
    const dblu_opt_data dat = {1, 0, *art};
    ivs_array_list_push_back(set, ivs_inline_pad(sizeof(dblu_opt_data)), dblu_palloc, &dat);
}

// static
// void
// dblu_static_opt_candidates_erase(
//     const ivs_array_list *const set,
//     const ivs_iterator iter
// ) {
//     ivs_assert(set);
//     ivs_assert(!ivs_iterator_empty(&iter));
//     dblu_opt_data *const p = ivs_array_list_deref(set, iter);
//     ivs_assert(p->exist);
//     p->exist = 0;
// }


/**
 * Changelog in version "B" for "Bennett":
 *
 * Version "A" for Kamisato "Ayaka" runs very slow,
 *   even on a 128-core AMD 5th generation Ryzen server.
 * So necessary optimization is required.
 *
 * Intel VTune Profiler shows:
 *   This is the main performance bottleneck.
 *     In function dblu_static_opt_candidates_optimal_l1:
 *       if (depth == 4)
 *           ret = ivs_inline_max(ret, formula(&sigma, opt_fd));
 *       else
 *           ret = ivs_inline_max(
 *               ret, dblu_static_opt_candidates_optimal_l1(self, formula, opt_fd, depth + 1, other_at, &sigma));
 *   Both of those branches are time-consuming. Optimization is necessary.
 *   For the invocation of function formula,
 *     in C, without template, inlining them seems to be difficult and unnecessary.
 *   So I'd prefer use newer algorithm and eliminate the recursion manually.
 *     to see if it's already fast enough.
 *
 * The key algorithm to optimization is,
 *   note everytime we evaluate the maximum value,
 *   we have to evaluate all previous combinations repeatedly.
 * So when updating, we mark every new-inserted arts as "not evaluated".
 * When we have to evaluate the maximum value using formula,
 *   we can just evaluate all those artifact combinations
 *   which contains at least one artifact that's marked with "not evaluated".
 */

static
void
dblu_static_opt_candidates_update_l1(
    ivs_array_list *const set,
    const dblu_art *const art,
    const uint32_t affected
) {
    ivs_assert(set && art);
    const dblu_art_gain g = dblu_art_gain_from_art(art);
    ivs_iterator iter = ivs_array_list_begin(set);
    _Bool has_bigger = 0;
    while (!ivs_iterator_empty(&iter)) {
        const dblu_opt_data *const p = ivs_array_list_deref(set, iter);
        if (!p->exist)
            goto l1;
        const dblu_art_gain gain = dblu_art_gain_from_art(&p->dat);
        if (dblu_art_gain_partial_greater_equal(&gain, &g, affected)) {
            has_bigger = 1;
            break;
        }
    l1:
        iter = ivs_array_list_advance(set, ivs_inline_pad(sizeof(dblu_opt_data)), iter, 1);
    }
    if (has_bigger)
        return;
    iter = ivs_array_list_begin(set);
    while (!ivs_iterator_empty(&iter)) {
        dblu_opt_data *const p = ivs_array_list_deref(set, iter);
        if (!p->exist)
            goto l2;
        const dblu_art_gain gain = dblu_art_gain_from_art(&p->dat);
        if (dblu_art_gain_partial_less_equal(&gain, &g, affected))
            p->exist = 0;
    l2:
        iter = ivs_array_list_advance(set, ivs_inline_pad(sizeof(dblu_opt_data)), iter, 1);
    }
    dblu_static_opt_candidates_insert(set, art);
}

void
dblu_opt_candidates_update(
    dblu_opt_candidates *const self,
    const dblu_art *const art,
    const uint32_t affected,
    const _Bool native
) {
    ivs_assert(self && art);
    ivs_assert(ivs_inline_between_cc(0, 4, art->pos));
    if (native)
        dblu_static_opt_candidates_update_l1(&self->native[art->pos], art, affected);
    else
        dblu_static_opt_candidates_update_l1(&self->other[art->pos], art, affected);
}

static
ivs_iterator
dblu_static_opt_candidates_optimal_advance(
    const ivs_array_list *const self,
    ivs_iterator iter
) {
    ivs_assert(self);
    ivs_assert(!ivs_iterator_empty(&iter));
    if (!ivs_iterator_empty(&iter))
        iter = ivs_array_list_advance(self, ivs_inline_pad(sizeof(dblu_opt_data)), iter, 1);
    while (!ivs_iterator_empty(&iter)) {
        const dblu_opt_data *const p = ivs_array_list_deref(self, iter);
        if (p->exist)
            break;
        else
            iter = ivs_array_list_advance(self, ivs_inline_pad(sizeof(dblu_opt_data)), iter, 1);
    }
    return iter;
}


/**
 * Changelog in version "B" for "Bennett":
 *   Eliminate the recursion,
 *     and implement the new algorithm.
 */

// static
// float
// dblu_static_opt_candidates_optimal_l1(
//     const dblu_opt_candidates *const self,
//     float (*const formula)(const dblu_art_gain *, void *),
//     void *const opt_fd,
//     const unsigned depth,
//     const unsigned other_at, // if it's larger than 4, it has special meaning!
//     const dblu_art_gain *const gain_above
// ) {
//     ivs_assert(self && formula);
//     float ret = 0.f;
//     const ivs_array_list *al;
//     if (depth == other_at)
//         al = &self->other[depth];
//     else
//         al = &self->native[depth];
//     ivs_iterator iter = ivs_array_list_begin(al);
//     if (ivs_iterator_empty(&iter))
//         return ret;
//     const dblu_opt_data *const first = ivs_array_list_deref(al, iter);
//     if (!first->exist)
//         iter = dblu_static_opt_candidates_optimal_advance(al, iter);
//     while (!ivs_iterator_empty(&iter)) {
//         const dblu_opt_data *const p = ivs_array_list_deref(al, iter);
//         ivs_assert(p->exist);
//         const dblu_art_gain g = dblu_art_gain_from_art(&p->dat);
//         const dblu_art_gain sigma = dblu_art_gain_add(&g, gain_above);
//         if (depth == 4)
//             ret = ivs_inline_max(ret, formula(&sigma, opt_fd));
//         else
//             ret = ivs_inline_max(
//                 ret, dblu_static_opt_candidates_optimal_l1(self, formula, opt_fd, depth + 1, other_at, &sigma));
//         iter = dblu_static_opt_candidates_optimal_advance(al, iter);
//     }
//     return ret;
// }

static
float
dblu_static_opt_candidates_optimal_l1(
    const dblu_opt_candidates *const self,
    float (*const formula)(const dblu_art_gain *, void *),
    void *const opt_fd
) {
    ivs_assert(self && formula);
    float ret = 0.f;
    dblu_art_gain gain_buf[5];
    ivs_iterator iter_buf[5];
    for (unsigned t = 0; t < 5; ++t) {
        iter_buf[t] = ivs_array_list_begin(&self->native[t]);
        if (!ivs_iterator_empty(&iter_buf[t])) {
            const dblu_opt_data *const p = ivs_array_list_deref(&self->native[t], iter_buf[t]);
            if (!p->exist)
                iter_buf[t] = dblu_static_opt_candidates_optimal_advance(&self->native[t], iter_buf[t]);
        }
    }
    unsigned level = 0;
    unsigned mask = 0;
    while (1) {
        if (ivs_iterator_empty(&iter_buf[level]))
            if (level) {
                mask &= ~(1u << level);
                --level;
                goto skip;
            } else
                break;
        const dblu_opt_data *const p = ivs_array_list_deref(&self->native[level], iter_buf[level]);
        if (!p->evaluated)
            mask |= 1u << level;
        if (level != 4) {
            const dblu_art_gain gain = dblu_art_gain_from_art(&p->dat);
            if (!level)
                gain_buf[level] = gain;
            else
                gain_buf[level] = dblu_art_gain_add(&gain_buf[level - 1], &gain);
            ++level;
            continue;
        } else {
            if (!mask && p->evaluated)
                ;
            else {
                const dblu_art_gain gain = dblu_art_gain_from_art(&p->dat);
                gain_buf[level] = dblu_art_gain_add(&gain_buf[level - 1], &gain);
                ret = ivs_inline_max(ret, formula(&gain_buf[level], opt_fd));
            }
        }
    skip:
        iter_buf[level] = dblu_static_opt_candidates_optimal_advance(&self->native[level], iter_buf[level]);
    }
    for (unsigned other = 0; other < 5; ++other) {
        level = 0;
        mask = 0;
        for (unsigned t = 0; t < 5; ++t) {
            if (t != other) {
                iter_buf[t] = ivs_array_list_begin(&self->native[t]);
                if (!ivs_iterator_empty(&iter_buf[t])) {
                    const dblu_opt_data *const p = ivs_array_list_deref(&self->native[t], iter_buf[t]);
                    if (!p->exist)
                        iter_buf[t] = dblu_static_opt_candidates_optimal_advance(&self->native[t], iter_buf[t]);
                }
            } else {
                iter_buf[t] = ivs_array_list_begin(&self->other[t]);
                if (!ivs_iterator_empty(&iter_buf[t])) {
                    const dblu_opt_data *const p = ivs_array_list_deref(&self->other[t], iter_buf[t]);
                    if (!p->exist)
                        iter_buf[t] = dblu_static_opt_candidates_optimal_advance(&self->other[t], iter_buf[t]);
                }
            }
        }
        while (1) {
            if (ivs_iterator_empty(&iter_buf[level]))
                if (level) {
                    mask &= ~(1u << level);
                    --level;
                    goto skip2;
                } else
                    break;
            const dblu_opt_data *p;
            if (level != other)
                p = ivs_array_list_deref(&self->native[level], iter_buf[level]);
            else
                p = ivs_array_list_deref(&self->other[level], iter_buf[level]);
            if (!p->evaluated)
                mask |= 1u << level;
            if (level != 4) {
                const dblu_art_gain gain = dblu_art_gain_from_art(&p->dat);
                if (!level)
                    gain_buf[level] = gain;
                else
                    gain_buf[level] = dblu_art_gain_add(&gain_buf[level - 1], &gain);
                ++level;
                continue;
            } else {
                if (!mask && p->evaluated)
                    ;
                else {
                    const dblu_art_gain gain = dblu_art_gain_from_art(&p->dat);
                    gain_buf[level] = dblu_art_gain_add(&gain_buf[level - 1], &gain);
                    ret = ivs_inline_max(ret, formula(&gain_buf[level], opt_fd));
                }
            }
        skip2:
            if (level != other)
                iter_buf[level] = dblu_static_opt_candidates_optimal_advance(&self->native[level], iter_buf[level]);
            else
                iter_buf[level] = dblu_static_opt_candidates_optimal_advance(&self->other[level], iter_buf[level]);
        }
    }
    for (unsigned t = 0; t < 5; ++t) {
        ivs_iterator iter = ivs_array_list_begin(&self->native[t]);
        while (!ivs_iterator_empty(&iter)) {
            dblu_opt_data *const p = ivs_array_list_deref(&self->native[t], iter);
            p->evaluated = 1;
            iter = ivs_array_list_advance(&self->native[t], ivs_inline_pad(sizeof(dblu_opt_data)), iter, 1);
        }
        iter = ivs_array_list_begin(&self->other[t]);
        while (!ivs_iterator_empty(&iter)) {
            dblu_opt_data *const p = ivs_array_list_deref(&self->other[t], iter);
            p->evaluated = 1;
            iter = ivs_array_list_advance(&self->other[t], ivs_inline_pad(sizeof(dblu_opt_data)), iter, 1);

        }
    }
    return ret;
}

float
dblu_opt_candidates_optimal(
    dblu_opt_candidates *const self,
    float (*const formula)(const dblu_art_gain *, void *),
    void *const opt_fd
) {
    ivs_assert(self && formula);
    const float r = dblu_static_opt_candidates_optimal_l1(self, formula, opt_fd);
    self->max = ivs_inline_max(self->max, r);
    return self->max;
}

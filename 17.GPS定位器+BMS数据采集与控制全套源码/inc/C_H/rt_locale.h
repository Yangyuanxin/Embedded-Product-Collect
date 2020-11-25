/* rt_locale.h: definitions to enable retargetting of locale mechanism
 *
 * Copyright 1999 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.7.2.2 $
 * Checkin $Date: 2001/09/25 15:20:37 $
 * Revising $Author: statham $
 */

#ifndef __RT_LOCALE_H
#define __RT_LOCALE_H

#define __lblk_start _Pragma("push_once Ono_remove_unused_constdata Ono_data_reorder")
#define __lblk_end _Pragma("pop")

/*
 * Macros to define locale blocks.
 *
 * All the macros' first two arguments are a symbol prefix and a
 * locale name. The resulting locale block will be addressed by the
 * expression `&symprefix_start', and the index entry by the
 * expression `&symprefix_index'.
 */

/*
 * usage: __LC_CTYPE_DEF(symprefix, localename)
 *        {table[0],table[1],...,table[255]};
 */

#define __LC_CTYPE_INTERNAL_DEF(sym,ln,tableclass,tablename) \
__lblk_start \
static const int sym##_index[] = {~3 & (3 + (268+(~3 & (3 + sizeof(ln)))))}; \
static const char sym##_lname[~3 & (3 + sizeof(ln))] = ln; \
static const int sym##_pname[] = {-4-(~3 & (3 + sizeof(ln)))}; \
static const char sym##_start[] = {0}; \
tableclass const char tablename[256] =

#define __LC_CTYPE_DEF(sym,ln) \
        __LC_CTYPE_INTERNAL_DEF(sym,ln,static,sym##_table)

/*
 * usage: __LC_COLLATE_DEF(symprefix, localename)
 *        {table[0],table[1],...,table[255]};
 *
 * Also:  __LC_COLLATE_TRIVIAL_DEF(symprefix,localename)
 *
 * which will define a LC_COLLATE block in which
 * offset_collate_table is zero, signifying the absence of a table.
 */

#define __LC_COLLATE_DEF(sym,ln) \
__lblk_start \
static const int sym##_index[] = {~3&(3+(268+(~3&(3+sizeof(ln)))))}; \
static const char sym##_lname[~3 & (3 + sizeof(ln))] = ln; \
static const int sym##_pname[] = {-4-(~3 & (3 + sizeof(ln)))}; \
static const int sym##_start[] = {4}; \
static const char sym##_table[] =

#define __LC_COLLATE_TRIVIAL_DEF(sym,ln) \
__lblk_start \
static const int sym##_index[] = {~3&(3+(12+(~3&(3+sizeof(ln)))))}; \
static const char sym##_lname[~3 & (3 + sizeof(ln))] = ln; \
static const int sym##_pname[] = {-4-(~3 & (3 + sizeof(ln)))}; \
static const int sym##_start[] = {0};

/*
 * usage: __LC_TIME_DEF(symprefix, localename,
 *                      week_abbrevs,
 *                      week_fullnames,
 *                      month_abbrevs,
 *                      month_fullnames,
 *                      am, pm,
 *                      datetime_fmt,
 *                      date_fmt,
 *                      time_fmt)
 */

#define __LC_TIME_DEF(sym,ln,wa,wf,ma,mf,am,pm,dt,df,tf) \
__lblk_start \
static const int sym##_index[] = {~3 & (3 + (sizeof(wa)+sizeof(wf)+sizeof(ma)+ \
                                          sizeof(mf)+sizeof(am)+sizeof(pm)+ \
                                          sizeof(dt)+sizeof(df)+sizeof(tf)+ \
                                          60+(~3 & (3 + sizeof(ln)))))}; \
static const char sym##_lname[~3 & (3 + sizeof(ln))] = ln; \
static const int sym##_pname[] = {-4-(~3 & (3 + sizeof(ln)))}; \
static const int sym##_start[] = {52}; \
static const int sym##_wfoff[] = {(sizeof(wa)+52)}; \
static const int sym##_maoff[] = {(sizeof(wa)+sizeof(wf)+52)}; \
static const int sym##_mfoff[] = {(sizeof(wa)+sizeof(wf)+sizeof(ma)+52)}; \
static const int sym##_amoff[] = {(sizeof(wa)+sizeof(wf)+sizeof(ma)+ \
                                   sizeof(mf)+52)}; \
static const int sym##_pmoff[] = {(sizeof(wa)+sizeof(wf)+sizeof(ma)+ \
                                   sizeof(mf)+sizeof(am)+52)}; \
static const int sym##_dtoff[] = {(sizeof(wa)+sizeof(wf)+sizeof(ma)+ \
                                   sizeof(mf)+sizeof(am)+sizeof(pm)+52)}; \
static const int sym##_dfoff[] = {(sizeof(wa)+sizeof(wf)+sizeof(ma)+ \
                                   sizeof(mf)+sizeof(am)+sizeof(pm)+ \
                                   sizeof(dt)+52)}; \
static const int sym##_tfoff[] = {(sizeof(wa)+sizeof(wf)+sizeof(ma)+ \
                                   sizeof(mf)+sizeof(am)+sizeof(pm)+ \
                                   sizeof(dt)+sizeof(df)+52)}; \
static const int sym##_wasiz[] = {(sizeof(wa)/7)}; \
static const int sym##_wfsiz[] = {(sizeof(wf)/7)}; \
static const int sym##_masiz[] = {(sizeof(ma)/12)}; \
static const int sym##_mfsiz[] = {(sizeof(mf)/12)}; \
static const char sym##_watxt[] = wa; \
static const char sym##_wftxt[] = wf; \
static const char sym##_matxt[] = ma; \
static const char sym##_mftxt[] = mf; \
static const char sym##_amtxt[] = am; \
static const char sym##_pmtxt[] = pm; \
static const char sym##_dttxt[] = dt; \
static const char sym##_dftxt[] = df; \
static const char sym##_tftxt[] = tf;

/*
 * usage: __LC_NUMERIC_DEF(symprefix, localename,
 *                         decimal_point,
 *                         thousands_sep,
 *                         grouping)
 */

#define __LC_NUMERIC_DEF(sym,ln,dp,ts,gr) \
__lblk_start \
static const int sym##_index[] = {~3 & (3 + (sizeof(dp)+sizeof(ts)+sizeof(gr)+ \
                                             20) + (~3 & (3 + sizeof(ln))))}; \
static const char sym##_lname[~3 & (3 + sizeof(ln))] = ln; \
static const int sym##_pname[] = {-4-(~3 & (3 + sizeof(ln)))}; \
static const int sym##_start[] = {12}; \
static const int sym##_tsoff[] = {(sizeof(dp)+12)}; \
static const int sym##_groff[] = {(sizeof(dp)+sizeof(ts)+12)}; \
static const char sym##_dptxt[] = dp; \
static const char sym##_tstxt[] = ts; \
static const char sym##_grtxt[] = gr;

/*
 * usage: __LC_MONETARY_DEF(symprefix, localename,
 *                          int_curr_symbol,
 *                          currency_symbol,
 *                          mon_decimal_point,
 *                          mon_thousands_sep,
 *                          mon_grouping,
 *                          positive_sign,
 *                          negative_sign,
 *                          int_frac_digits,
 *                          frac_digits,
 *                          p_cs_precedes,
 *                          p_sep_by_space,
 *                          n_cs_precedes,
 *                          n_sep_by_space,
 *                          p_sign_posn,
 *                          n_sign_posn)
 */

#define __LC_MONETARY_DEF(sym,ln,ic,cs,md,mt,mg,ps,ns, \
                          id,fd,pc,pS,nc,nS,pp,np) \
__lblk_start \
static const int sym##_index[] = {~3 & (3 + (sizeof(ic)+sizeof(cs)+sizeof(md)+ \
                                          sizeof(mt)+sizeof(mg)+sizeof(ps)+ \
                                          sizeof(ns)+44) \
+ (~3 & (3 + sizeof(ln))))}; \
static const char sym##_lname[~3 & (3 + sizeof(ln))] = ln; \
static const int sym##_pname[] = {-4-(~3 & (3 + sizeof(ln)))}; \
static const char sym##_start[] = {id}; \
static const char sym##_fdchr[] = {fd}; \
static const char sym##_pcchr[] = {pc}; \
static const char sym##_pSchr[] = {pS}; \
static const char sym##_ncchr[] = {nc}; \
static const char sym##_nSchr[] = {nS}; \
static const char sym##_ppchr[] = {pp}; \
static const char sym##_npchr[] = {np}; \
static const int sym##_icoff[] = {36}; \
static const int sym##_csoff[] = {(sizeof(ic)+36)}; \
static const int sym##_mdoff[] = {(sizeof(ic)+sizeof(cs)+36)}; \
static const int sym##_mtoff[] = {(sizeof(ic)+sizeof(cs)+sizeof(md)+36)}; \
static const int sym##_mgoff[] = {(sizeof(ic)+sizeof(cs)+sizeof(md)+ \
                                   sizeof(mt)+36)}; \
static const int sym##_psoff[] = {(sizeof(ic)+sizeof(cs)+sizeof(md)+ \
                                   sizeof(mt)+sizeof(mg)+36)}; \
static const int sym##_nsoff[] = {(sizeof(ic)+sizeof(cs)+sizeof(md)+ \
                                   sizeof(mt)+sizeof(mg)+sizeof(ps)+36)}; \
static const char sym##_ictxt[] = ic; \
static const char sym##_cstxt[] = cs; \
static const char sym##_mdtxt[] = md; \
static const char sym##_mttxt[] = mt; \
static const char sym##_mgtxt[] = mg; \
static const char sym##_pstxt[] = ps; \
static const char sym##_nstxt[] = ns;

/*
 * Use this macro to declare the end of an index. `symprefix' is
 * provided to ensure a unique name.
 */
#define __LC_INDEX_END(symprefix) \
__lblk_start \
static const int symprefix##_index[] = {0}; \
__lblk_end

/*
 * The `struct' forms of the blocks defined above.
 */
struct __lc_ctype_blk {
    char ctype_table[257];             /* char ctype_table[-1..255] */
};
struct __lc_collate_blk {
    unsigned offset_collate_table;     /* char collate_table[256], or 0 */
};
struct __lc_time_blk {
    unsigned offset_week_abbrs;        /* char week_abbrs[] */
    unsigned offset_week_full;         /* char week_full[] */
    unsigned offset_month_abbrs;       /* char month_abbrs[] */
    unsigned offset_month_full;        /* char month_full[] */
    unsigned offset_am;                /* char am[] */
    unsigned offset_pm;                /* char pm[] */
    unsigned offset_datetime_fmt;      /* char datetime_fmt[] */
    unsigned offset_date_fmt;          /* char date_fmt[] */
    unsigned offset_time_fmt;          /* char time_fmt[] */
    unsigned size_week_abbrs;          /* size of one entry */
    unsigned size_week_full;           /* size of one entry */
    unsigned size_month_abbrs;         /* size of one entry */
    unsigned size_month_full;          /* size of one entry */
};
struct __lc_numeric_blk {
    unsigned offset_decimal_point;     /* char decimal_point[] */
    unsigned offset_thousands_sep;     /* char thousands_sep[] */
    unsigned offset_grouping;          /* char grouping[] */
};
struct __lc_monetary_blk {
    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
    unsigned offset_int_curr_symbol;   /* char int_curr_symbol[] */
    unsigned offset_currency_symbol;   /* char currency_symbol[] */
    unsigned offset_mon_decimal_point; /* char mon_decimal_point[] */
    unsigned offset_mon_thousands_sep; /* char mon_thousands_sep[] */
    unsigned offset_mon_grouping;      /* char mon_grouping[] */
    unsigned offset_positive_sign;     /* char positive_sign[] */
    unsigned offset_negative_sign;     /* char negative_sign[] */
};

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The _findlocale function for performing index searches.
 */
extern void const *_findlocale(void const * /*index*/, char const * /*name*/);

/*
 * The _get_lc_CATEGORY functions. Override these to retarget
 * locales.
 */
void const *_get_lc_collate(void const * /*null*/, char const * /*name*/);
void const *_get_lc_ctype(void const * /*null*/, char const * /*name*/);
void const *_get_lc_monetary(void const * /*null*/, char const * /*name*/);
void const *_get_lc_numeric(void const * /*null*/, char const * /*name*/);
void const *_get_lc_time(void const * /*null*/, char const * /*name*/);

#ifdef __cplusplus
}
#endif

#endif

#ifndef CONFIG_H
#define CONFIG_H

#define TIMEOUT     5   /* seconds */

/* alignment commands */
static int align_l(char *, size_t, size_t);
// static int align_c(char *, size_t, size_t);
static int align_r(char *, size_t, size_t);

/* info commands */
static int mwm(char *, size_t, size_t);
// static int sep(char *, size_t, size_t);
// static int total_mem(char *, size_t, size_t);
// static int free_mem(char *, size_t, size_t);
// static int cpu(char *, size_t, size_t);
// static int ddbf(char *, size_t, size_t);
static int mpd(char *, size_t, size_t);
static int vol(char *, size_t, size_t);
static int date(char *, size_t, size_t);

static const char *names[] = {
    "", "", "", "",
};

static const char *modes[] = {
    "[T]", "[M]", "[B]", "[G]", "[#]", "[F]",
};

/** commands configuration **/

/* alignment */
#define LEFT        "\\l"
#define CENTER      "\\c"
#define RIGHT       "\\r"

/* mwm */
#define MWM_PRE     ""
#define MWM_SUF     ""

#define DESKTOP_CUR "\\u2\\b2"
#define DESKTOP_UNF "\\u4\\b4"
#define DESKTOP_URG "\\u3"

#define DESKTOP_PRE " "
#define DESKTOP_SUF "\\ur\\br"

#define WINDOW_ZER  "\\f0"
#define WINDOW_PRE  " "
#define WINDOW_SUF  " \\fr"

#define LAYOUT_PRE  " \\u2\\b2 "
#define LAYOUT_SUF  " \\ur\\br"

/* sep */
#define SEP_PRE     "\\u6\\b6"
#define SEP_FMT     " | "
#define SEP_SUF     "\\ur\\br"

/* total mem */
#define MEM_TOTAL_ICO   " "
#define MEM_TOTAL_PRE   "total: "
#define MEM_TOTAL_SUF   " Mb "

/* free mem */
#define MEM_FREE_ICO    " "
#define MEM_FREE_PRE    "free: "
#define MEM_FREE_SUF    " Mb "

/* cpu */
#define CPU_ICO     ""
#define CPU_PRE     "\\u4\\b4 "
#define CPU_SUF     " mhz\\ur\\br"

/* music */
#define MUSIC_ICO   "\\u2\\b2  \\br\\ur"
#define MUSIC_PRE   " "
#define MUSIC_SUF   " "

/* ddbf */
#define DDBF_PRE    ""
#define DDBF_SUF    ""
#define DDBF_FMT    "%t \\f8by\\fr %a"


/* mpd */
#define MPD_PRE     ""
#define MPD_SUF     ""

#define STOPPED_FMT "[stopped]"

#define TITLE_ICO   ""
#define TITLE_PRE   ""
#define TITLE_SUF   ""

#define ARTIST_ICO  " \\f9by\\fr "
#define ARTIST_PRE  ""
#define ARTIST_SUF  ""

/* batt */
#define BATT_ICO   "\\u2\\b2  \\br\\ur"

#define BATT_PRE    " "
#define BATT_SUF    " "

#define BATT_NORM   "="
#define BATT_CRIT   "!"
#define BATT_CHAR   "+"
#define BATT_DISC   "-"
#define BATT_UNKN   "?"

/* vol */
#define VOL_ICO     "\\u2\\b2  \\br\\ur"
#define VOL_PRE     " "
#define VOL_SUF     " "

#define MUTE_FMT    "[m]"
#define VOL_MUTE    "Master"
#define VOL_VOL     "PCM"

/* date */
#define DATE_ICO    "\\u2\\b2  \\br\\ur"
#define DATE_FMT    "%a %d/%m %R"
#define DATE_PRE    " "
#define DATE_SUF    ""

/* order of commands to execute and show */
static int (* const funcs[])(char *buf, size_t rem, size_t offset) = {
    align_l,
    mwm,

    // align_c,
    // total_mem,
    // sep,
    // free_mem,
    // sep,
    // cpu,

    align_r,
    // ddbf,
    mpd,
    vol,
    date,
};

#endif

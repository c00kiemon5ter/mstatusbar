#ifndef CONFIG_H
#define CONFIG_H

#define TIMEOUT     5   /* seconds */

#define MEM_INFO    "/proc/meminfo"
#define CPU_INFO    "/proc/cpuinfo"

/* alignment commands */
static int align_l(char *, size_t, size_t);
static int align_c(char *, size_t, size_t);
static int align_r(char *, size_t, size_t);

/* info commands */
static int mwm(char *, size_t, size_t);
static int sep(char *, size_t, size_t);
static int mem(char *, size_t, size_t);
static int cpu(char *, size_t, size_t);
static int mpd(char *, size_t, size_t);
static int vol(char *, size_t, size_t);
static int date(char *, size_t, size_t);

static const char *names[] = {
    "", "", "", "", "foo", "bar",
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

/* mem */
#define MEM_PRE     "\\u3\\b3"
#define MEM_SUF     "\\ur\\br"

/* cpu */
#define CPU_PRE     "\\u4\\b4"
#define CPU_SUF     "\\ur\\br"

/* mpd */
#define MUSIC_PRE   "\\u2\\b2  \\br\\ur "
#define MUSIC_SUF   " "

#define STOPPED_FMT "[stopped]"

#define TITLE_PRE   ""
#define TITLE_SUF   " \\f9by\\fr "

#define ARTIST_PRE  ""
#define ARTIST_SUF  ""

/* vol */
#define VOL_PRE     "\\u2\\b2  \\br\\ur "
#define VOL_SUF     " "

#define VOL_MUTE    "Master"
#define VOL_VOL     "PCM"

/* date */
#define DATE_PRE    "\\u2\\b2  \\br\\ur "
#define DATE_FMT    "%a %d/%m %R"
#define DATE_SUF    ""

/* order of commands to execute and show */
static int (* const funcs[])(char *buf, size_t rem, size_t offset) = {
    align_l,
    mwm,

    // align_c,
    // mem,
    // sep,
    // cpu,

    align_r,
    mpd,
    vol,
    date,
};

#endif

#ifndef CONFIG_H
#define CONFIG_H

/* how often in seconds to run the commands */
#define TIMEOUT     5

#define MEM_INFO    "/proc/meminfo"
#define CPU_INFO    "/proc/cpuinfo"
#define ACPI_DEV    "/dev/acpi"
#define MPD_TIMEOUT 1000

/* alignment commands */
static int align_l(char *, size_t, size_t);
static int align_c(char *, size_t, size_t);
static int align_r(char *, size_t, size_t);

/* info commands */
static int mwm(char *, size_t, size_t);
static int sep(char *, size_t, size_t);
static int total_mem(char *, size_t, size_t);
static int free_mem(char *, size_t, size_t);
static int cpu(char *, size_t, size_t);
static int ddbf(char *, size_t, size_t);
static int mpd(char *, size_t, size_t);
static int batt_state(char *, size_t, size_t);
static int batt_perc(char *, size_t, size_t);
static int vol(char *, size_t, size_t);
static int date(char *, size_t, size_t);

static const char *names[] = {
    "web", "dev", "sex", "nil", "foo", "bar",
};

static const char *modes[] = {
    "[T]", "[M]", "[B]", "[G]", "[#]", "[F]",
};

/** commands configuration **/

/* alignment */
#define LEFT            "\\l"
#define CENTER          "\\c"
#define RIGHT           "\\r"

/* mwm */
#define MWM_PRE         ""
#define MWM_SUF         ""

#define DESKTOP_CUR     "\\u2\\b2"
#define DESKTOP_UNF     "\\u4\\b4"
#define DESKTOP_URG     "\\u3"

#define DESKTOP_PRE     " "
#define DESKTOP_SUF     "\\ur\\br"

#define WINDOW_ZER      "\\f0"
#define WINDOW_PRE      " "
#define WINDOW_SUF      " \\fr"

#define LAYOUT_PRE      " \\u2\\b2 "
#define LAYOUT_SUF      " \\ur\\br"

/* sep */
#define SEP_PRE         "\\u6\\b6"
#define SEP_FMT         " :: "
#define SEP_SUF         "\\ur\\br"

/* total mem */
#define MEM_TOTAL_ICO   "\\u2\\b2 TM \\ur\\br"
#define MEM_TOTAL_PRE   " total: "
#define MEM_TOTAL_SUF   " Mb "

/* free mem */
#define MEM_FREE_ICO    "\\u2\\b2 FM \\ur\\br"
#define MEM_FREE_PRE    " "
#define MEM_FREE_SUF    " Mb "

/* cpu */
#define CPU_ICO         "\\u2\\b2 C \\ur\\br"
#define CPU_PRE         " "
#define CPU_SUF         " mhz "

/* ddbf */
#define DDBF_ICO        "\\u2\\b2 M \\ur\\br"
#define DDBF_PRE        " "
#define DDBF_SUF        " "
#define DDBF_FMT        "%t \\f8by\\fr %a"

/* mpd */
#define MPD_ICO         "\\u2\\b2 M \\ur\\br"
#define MPD_PRE         ""
#define MPD_SUF         " "

/* mpd state */
#define MPD_STATE_ICO   ""
#define MPD_STATE_PRE   ""
#define MPD_STATE_SUF   ""

#define MPD_PLAYING     "\\u5\\b5 P \\ur\\br"
#define MPD_STOPPED     "\\u5\\b5 S \\ur\\br"
#define MPD_PAUSED      "\\u5\\b5 I \\ur\\br"
#define MPD_UNKNOWN     "\\u5\\b5 ? \\ur\\br"

/* mpd tags */
#define TITLE_ICO       " "
#define TITLE_PRE       ""
#define TITLE_SUF       ""

#define ARTIST_ICO      " \\f9by\\fr "
#define ARTIST_PRE      ""
#define ARTIST_SUF      ""

/* batt state */
#define BATT_ST_ICO     "\\u2\\b2 B \\ur\\br"
#define BATT_ST_PRE     " "
#define BATT_ST_SUF     " "

#define BATT_NORMAL     "="
#define BATT_CRITICAL   "!"
#define BATT_CHARE      "+"
#define BATT_DISCHARGE  "-"
#define BATT_UNKNOWN    "?"

/* bat perc */
#define BATT_ICO        ""
#define BATT_PRE        " "
#define BATT_SUF        "% "

/* vol */
#define VOL_ICO         "\\u2\\b2 V \\ur\\br"
#define VOL_PRE         " "
#define VOL_SUF         " "

#define MUTE_FMT        "[m]"
#define MUTE_CHAN       "Master"
#define VOL_CHAN        "PCM"

/* date */
#define DATE_ICO        "\\u2\\b2 D \\ur\\br"
#define DATE_FMT        "%a %d/%m %R"
#define DATE_PRE        " "
#define DATE_SUF        ""

/* order of commands to execute and show */
static int (* const funcs[])(char *buf, size_t rem, size_t offset) = {
    align_l,
    mwm,

    align_c,
    total_mem,
    free_mem,
    cpu,

    align_r,
    ddbf,
    mpd,
    vol,
    batt_state,
    batt_perc,
    date,
};

#endif

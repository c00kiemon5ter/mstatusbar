#define _POSIX_C_SOURCE 200809L /* or _XOPEN_SOURCE 700 */
#define LENGTH(x)       (sizeof(x) / sizeof(*x))

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include <alsa/asoundlib.h>

#if MPD
#include <mpd/client.h>
#endif

#if BATT
#include <dev/acpica/acpiio.h>
#include <sys/ioctl.h>
#endif

#include "config.h"

static char desktops[BUFSIZ/3];

int align_l(char *buf, size_t offset, size_t rem)
{
    return snprintf(buf + offset, rem, LEFT);
}

int align_c(char *buf, size_t offset, size_t rem)
{
    return snprintf(buf + offset, rem, CENTER);
}

int align_r(char *buf, size_t offset, size_t rem)
{
    return snprintf(buf + offset, rem, RIGHT);
}

int sep(char *buf, size_t offset, size_t rem)
{
    return snprintf(buf + offset, rem, SEP_PRE SEP_FMT SEP_SUF);
}

int total_mem(char *buf, size_t offset, size_t rem)
{
    int r = 0;
    unsigned int value = 0;

    FILE *fp = fopen(MEM_INFO, "r");
    if (!fp)
        return 0;

    char *line = 0;
    size_t len = 0;
    ssize_t read = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (sscanf(line, "MemTotal: %u kB", &value) == 1)
            break;
    }

    r = snprintf(buf + offset, rem, MEM_TOTAL_ICO MEM_TOTAL_PRE "%u" MEM_TOTAL_SUF, value/1024);

    if (line)
        free(line);

    return r;
}

int free_mem(char *buf, size_t offset, size_t rem)
{
    int r = 0;
    unsigned int mfree = 0, mbuff = 0, mcache = 0;

    FILE *fp = fopen(MEM_INFO, "r");
    if (!fp)
        return 0;

    char *line = 0;
    size_t len = 0;
    ssize_t read = 0;

    while (!(mfree && mbuff && mcache) && (read = getline(&line, &len, fp)) != -1) {
        unsigned int value = 0;
        if (sscanf(line, "MemFree: %u kB", &value) == 1)
            mfree = value;
        else if (sscanf(line, "Buffers: %u kB", &value) == 1)
            mbuff = value;
        else if (sscanf(line, "Cached: %u kB", &value) == 1)
            mcache = value;
    }

    r = snprintf(buf + offset, rem, MEM_FREE_ICO MEM_FREE_PRE "%u" MEM_FREE_SUF, (mfree + mbuff + mcache)/1024);

    if (line)
        free(line);

    return r;
}

int cpu(char *buf, size_t offset, size_t rem)
{
    int r = 0;

    FILE *fp = fopen(CPU_INFO, "r");

    if (!fp)
        return 0;

    char *line = 0;
    size_t len = 0;
    ssize_t read = 0;

    double mhz = 0;

    while (!(mhz) && (read = getline(&line, &len, fp)) != -1) {
        double value = 0;
        if (sscanf(line, "cpu MHz\t\t: %lf", &value) == 1)
            mhz = value;
    }

    if (mhz)
        r = snprintf(buf + offset, rem, CPU_ICO CPU_PRE "%lf" CPU_SUF, mhz/1024);

    if (line)
        free(line);

    return r;
}

int ddbf(char *buf, size_t offset, size_t rem)
{
#if DDBF
    int r = 0;

    const char cmd[] = "deadbeef --nowplaying '"DDBF_FMT"'";
    char rsp[BUFSIZ/3];

    FILE *fp = popen(cmd, "r");

    if (!fp)
        return 0;

    if (!fgets(rsp, sizeof(rsp), fp)) {
        pclose(fp);
        return 0;
    }

    r = snprintf(buf + offset, rem, DDBF_ICO DDBF_PRE "%s" DDBF_SUF, rsp);

    pclose(fp);
#endif

    return 0;
}

int mpd(char *buf, size_t offset, size_t rem)
{
#if MPD
    int r = 0;

    const char *host = getenv("MPD_HOST");
    const char *port = getenv("MPD_PORT");
    const char *pass = getenv("MPD_PASSWORD");

    const char *mpd_host = host ? host : "localhost";
    const int mpd_port = port ? strtol(port, NULL, 10) : 6600;

    struct mpd_connection *conn = mpd_connection_new(mpd_host, mpd_port, MPD_TIMEOUT);
    if (mpd_connection_get_error(conn))
        return 0;

    if (pass && !mpd_run_password(conn, pass))
        return 0;

    r += snprintf(buf + offset + r, rem - r, MPD_ICO MPD_PRE);

    /* mpd state */
    struct mpd_status *status = mpd_run_status(conn);
    if (status) {
        const enum mpd_state state = mpd_status_get_state(status);

        r += snprintf(buf + offset + r, rem - r, MPD_STATE_ICO MPD_STATE_PRE "%s" MPD_STATE_SUF,
                state == MPD_STATE_PLAY ? MPD_PLAYING :
                state == MPD_STATE_STOP ? MPD_STOPPED :
                state == MPD_STATE_PAUSE ? MPD_PAUSED : MPD_UNKNOWN);
    }

    /* mpd song */
    struct mpd_song *song = mpd_run_current_song(conn);
    if (song) {
        const char *artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
        const char *title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);

        r += snprintf(buf + offset + r, rem - r, TITLE_ICO  TITLE_PRE  "%s" TITLE_SUF,  title);
        r += snprintf(buf + offset + r, rem - r, ARTIST_ICO ARTIST_PRE "%s" ARTIST_SUF, artist);

        mpd_song_free(song);
    }

    r += snprintf(buf + offset + r, rem - r, MPD_SUF);

    mpd_connection_free(conn);

    if (status || song)
        return r;
#endif

    return 0;
}

int batt_state(char *buf, size_t offset, size_t rem)
{
    int r = 0;

#if BATT
    int fd = open(ACPI_DEV, O_RDONLY);
    if (fd == -1)
        return 0;

    union acpi_battery_ioctl_arg battio;
    battio.unit = ACPI_BATTERY_ALL_UNITS;

    if (ioctl(fd, ACPIIO_BATT_GET_BATTINFO, &battio) != -1 && battio.battinfo.cap != -1) {
        r = snprintf(buf + offset, rem, BATT_ST_ICO BATT_ST_PRE "%s" BUT_ST_SUF,
                battio.battinfo.state == 0                         ? BATT_NORMAL    :
                battio.battinfo.state & ACPI_BATT_STAT_CRITICAL    ? BATT_CRITICAL  :
                battio.battinfo.state & ACPI_BATT_STAT_DISCHARGING ? BATT_DISCHARGE :
                battio.battinfo.state & ACPI_BATT_STAT_CHARGING    ? BATT_CHARGE    : BATT_UNKNOWN);
    }

    close(fd);
#endif

    return r;
}

int batt_perc(char *buf, size_t offset, size_t rem)
{
    int r = 0;

#if BATT
    int fd = open(ACPI_DEV, O_RDONLY);
    if (fd == -1)
        return 0;

    union acpi_battery_ioctl_arg battio;
    battio.unit = ACPI_BATTERY_ALL_UNITS;

    if (ioctl(fd, ACPIIO_BATT_GET_BATTINFO, &battio) != -1 && battio.battinfo.cap != -1) {
        r = snprintf(buf + offset, rem, BATT_ICO BATT_PRE "%s" BUT_SUF, battio.battinfo.cap
    }

    close(fd);
#endif

    return r;
}

int vol(char *buf, size_t offset, size_t rem)
{
    long vol = 0, max = 0, min = 0;
    int mute = 0;

    /* init alsa */
    snd_mixer_t *handle;
    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, "default");
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    /* init channel with volume info */
    snd_mixer_selem_id_t *vol_id;
    snd_mixer_selem_id_malloc(&vol_id);
    snd_mixer_selem_id_set_name(vol_id, VOL_CHAN);
    snd_mixer_elem_t* pcm_mixer = snd_mixer_find_selem(handle, vol_id);

    /* get volume */
    snd_mixer_selem_get_playback_volume_range(pcm_mixer, &min, &max);
    snd_mixer_selem_get_playback_volume(pcm_mixer, SND_MIXER_SCHN_MONO, &vol);

    /* init channel with mute info */
    snd_mixer_selem_id_t *mute_id;
    snd_mixer_selem_id_malloc(&mute_id);
    snd_mixer_selem_id_set_name(mute_id, MUTE_CHAN);
    snd_mixer_elem_t* mas_mixer = snd_mixer_find_selem(handle, mute_id);

    /* get mute state */
    snd_mixer_selem_get_playback_switch(mas_mixer, SND_MIXER_SCHN_MONO, &mute);

    /* cleanup */
    if (vol_id)
        snd_mixer_selem_id_free(vol_id);
    if (mute_id)
        snd_mixer_selem_id_free(mute_id);
    if (handle)
        snd_mixer_close(handle);

    return snprintf(buf + offset, rem, VOL_ICO VOL_PRE "%ld%%" VOL_SUF, !!mute * (vol*100)/max);
}

int date(char *buf, size_t offset, size_t rem)
{
    struct tm *timeinfo;
    time_t rawtime;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return strftime(buf + offset, rem, DATE_ICO DATE_PRE DATE_FMT DATE_SUF, timeinfo);
}

int mwm(char *buf, size_t offset, size_t rem)
{
    return snprintf(buf + offset, rem, MWM_PRE "%s" MWM_SUF, desktops);
}

void parse_desktop_info(void)
{
    char line[BUFSIZ/3];
    int len = 0;
    unsigned layout = 0;

    if ((len = read(STDIN_FILENO, line, sizeof(line))) < 0)
        return;

    if (line[len-1] == '\n')
        line[--len] = '\0';
    len = 0;

    for (char *token = strtok(line, " "); token; token = strtok(NULL, " ")) {
        unsigned deskid = 0, nwins = 0, deskmode = 0, deskcur = 0, deskurg = 0;
#if XINERAMA
        unsigned monid = 0, moncur = 0;

        if (sscanf(token, "%u:%u:%u:%u:%u:%u:%u", &monid, &moncur,
                    &deskid, &nwins, &deskmode, &deskcur, &deskurg) == 7) {

            if (moncur && deskcur)
                layout = deskmode;

            len += snprintf(desktops + len, sizeof(desktops) - len, "%s%s" DESKTOP_PRE "%s%s" WINDOW_PRE "%u" WINDOW_SUF DESKTOP_SUF,
                    deskcur && moncur ? DESKTOP_CUR : deskcur ? DESKTOP_UNF : "",
                    deskurg ? DESKTOP_URG : "",
                    names[deskid],
                    nwins ? "" : WINDOW_ZER,
                    nwins);
#else
        if (sscanf(token, "%u:%u:%u:%u:%u",
                    &deskid, &nwins, &deskmode, &deskcur, &deskurg) == 5) {

            if (deskcur)
                layout = deskmode;

            len += snprintf(desktops + len, sizeof(desktops) - len, "%s%s" DESKTOP_PRE "%s%s" WINDOW_PRE "%u" WINDOW_SUF DESKTOP_SUF,
                    deskcur ? DESKTOP_CUR : deskcur ? DESKTOP_UNF : "",
                    deskurg ? DESKTOP_URG : "",
                    names[deskid],
                    nwins ? "" : WINDOW_ZER,
                    nwins);
#endif
        } else {
            return;
        }
    }

    snprintf(desktops + len, sizeof(desktops) - len, LAYOUT_PRE "%s" LAYOUT_SUF, modes[layout]);
}

int main(void)
{
    for (;;) {
        char mesg[BUFSIZ];
        size_t mesglen = 0;

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        struct timeval tv = { TIMEOUT, 0 };
        int status = select(1, &fds, NULL, NULL, &tv);

        if (status == -1)
            return EXIT_FAILURE;

        if (FD_ISSET(STDIN_FILENO, &fds))
            parse_desktop_info();

        for (size_t rem = sizeof(mesg) - 2, n = 0, i = 0;
                i < LENGTH(funcs) && rem > 0;
                mesglen += n, rem -= n, i++)
            if (funcs[i])
                n = funcs[i](mesg, mesglen, rem);

        mesglen += snprintf(mesg + mesglen, 2, "\n");
        write(STDOUT_FILENO, mesg, mesglen);
    }

    return EXIT_SUCCESS;
}


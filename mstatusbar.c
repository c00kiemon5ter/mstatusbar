#define _POSIX_C_SOURCE 200809L /* or #define _XOPEN_SOURCE 700 */
#define LENGTH(x)       (sizeof(x) / sizeof(*x))

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include <alsa/asoundlib.h>
#include <mpd/client.h>

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
    unsigned long value = 0;

    FILE *fp = fopen(MEM_INFO, "r");
    if (!fp)
        return 0;

    char *line = 0;
    size_t len = 0;
    ssize_t read = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (sscanf(line, "MemTotal: %lu kB", &value) == 1)
            break;
    }

    r = snprintf(buf + offset, rem, MEM_TOTAL_ICO MEM_TOTAL_PRE "%lu" MEM_TOTAL_SUF, value/1024);

    if (line)
        free(line);

    return r;
}

int free_mem(char *buf, size_t offset, size_t rem)
{
    int r = 0;
    unsigned long value = 0;

    FILE *fp = fopen(MEM_INFO, "r");
    if (!fp)
        return 0;

    char *line = 0;
    size_t len = 0;
    ssize_t read = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (sscanf(line, "MemFree: %lu kB", &value) == 1)
            break;
    }

    r = snprintf(buf + offset, rem, MEM_FREE_ICO MEM_FREE_PRE "%lu" MEM_FREE_SUF, value/1024);

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
    int r = 0;
#if DDBF
    const char cmd[] = "deadbeef --nowplaying '"DDBF_FMT"'";
    char rsp[BUFSIZ/3];

    FILE *fp = popen(cmd, "r");

    if (!fp)
        return 0;

    if (!fgets(rsp, sizeof(rsp), fp)) {
        pclose(fp);
        return 0;
    }

    r = snprintf(buf + offset, rem, MUSIC_ICO MUSIC_PRE DDBF_PRE "%s" DDBF_SUF MUSIC_SUF, rsp);

    pclose(fp);
#endif
    return r;
}

#define MPD_TIMEOUT 1000
int mpd(char *buf, size_t offset, size_t rem)
{
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

    struct mpd_song *song = mpd_run_current_song(conn);

    if (song) {
        const char *artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
        const char *title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
        const char *track = mpd_song_get_tag(song, MPD_TAG_TRACK, 0);

        r = snprintf(buf + offset, rem, MUSIC_PRE TITLE_PRE "%s" TITLE_SUF ARTIST_PRE "%s" ARTIST_SUF MUSIC_SUF, title ? title : track, artist);

        mpd_song_free(song);
    }

    mpd_connection_free(conn);

    if (!r)
        r = snprintf(buf + offset, rem, MUSIC_PRE STOPPED_FMT MUSIC_SUF);
    return r;
}

#define ACPI_DEV    "/dev/acpi"
int batt(char *buf, size_t offset, size_t rem)
{
    int r = 0;

#if BATT
    int fd = open(ACPI_DEV, O_RDONLY);
    if (fd == -1)
        return 0;

    union acpi_battery_ioctl_arg battio;
    battio.unit = ACPI_BATTERY_ALL_UNITS;

    if (ioctl(fd, ACPIIO_BATT_GET_BATTINFO, &battio) != -1 && battio.battinfo.cap != -1) {
        r = snprintf(buf + offset, rem, BATT_ICO BATT_PRE "%s" BUT_SUF,
                battio.battinfo.state == 0                         ? BATT_NORM :
                battio.battinfo.state & ACPI_BATT_STAT_CRITICAL    ? BATT_CRIT :
                battio.battinfo.state & ACPI_BATT_STAT_DISCHARGING ? BATT_DISC :
                battio.battinfo.state & ACPI_BATT_STAT_CHARGING    ? BATT_CHAR : BATT_UNKN);
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
    snd_mixer_selem_id_set_name(vol_id, VOL_VOL);
    snd_mixer_elem_t* pcm_mixer = snd_mixer_find_selem(handle, vol_id);

    /* get volume */
    snd_mixer_selem_get_playback_volume_range(pcm_mixer, &min, &max);
    snd_mixer_selem_get_playback_volume(pcm_mixer, SND_MIXER_SCHN_MONO, &vol);

    /* init channel with mute info */
    snd_mixer_selem_id_t *mute_id;
    snd_mixer_selem_id_malloc(&mute_id);
    snd_mixer_selem_id_set_name(mute_id, VOL_MUTE);
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


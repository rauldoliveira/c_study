#include <stdio.h>                                                                                                                                                     
#include <stdlib.h>                                                                                                                                                    
#include <string.h>                                                                                                                                                    
#include <stdbool.h>                                                                                                                                                   
#include <unistd.h>                                                                                                                                                    
#include <errno.h>                                                                                                                                                     
#include <ncurses.h>                                                                                                                                                   
                                                                                                                                                                       
#define HZ  hz                                                                                                                                                         
#define S_VALUE(m,n,p)       (((double) ((n) - (m))) / (p) * HZ)                                                                                                       
#define DISKSTATS_FILE "/proc/diskstats"                                                                                                                               
#define STAT_FILE               "/proc/stat"

unsigned int hz;

void get_HZ(void)
{
    long ticks;
    if ((ticks = sysconf(_SC_CLK_TCK)) == -1) {
        perror("sysconf");
    }
    hz = (unsigned int) ticks;
}

struct dstats 
{
    int major;
    int minor;
    char devname[64];
    unsigned long r_completed;         /* reads completed successfully */
    unsigned long r_merged;            /* reads merged */
    unsigned long r_sectors;           /* sectors read */
    unsigned long r_spent;             /* time spent reading (ms) */
    unsigned long w_completed;         /* writes completed */
    unsigned long w_merged;            /* writes merged */
    unsigned long w_sectors;           /* sectors written */
    unsigned long w_spent;             /* time spent writing (ms) */
    unsigned long io_in_progress;      /* I/Os currently in progress */
    unsigned long t_spent;             /* time spent doing I/Os (ms) */
    unsigned long t_weighted;          /* weighted time spent doing I/Os (ms) */
};

struct ext_dstats {
    double util;
    double await;
    double arqsz;
};

void read_uptime(unsigned long long *uptime)
{
    FILE *fp;
    char line[128];
    unsigned long up_sec, up_cent;

    if ((fp = fopen("/proc/uptime", "r")) == NULL)
        return;

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return;
    }

    sscanf(line, "%lu.%lu", &up_sec, &up_cent);
        *uptime = (unsigned long long) up_sec * HZ +
        (unsigned long long) up_cent * HZ / 100;
    fclose(fp);
}

unsigned long long get_interval(unsigned long long prev_uptime,
                                                unsigned long long curr_uptime)
{
    unsigned long long itv;
            
    /* first run prev_uptime=0 so displaying stats since system startup */
    itv = curr_uptime - prev_uptime;

    if (!itv) {     /* Paranoia checking */
        itv = 1;
    }
    return itv;
}

void replace_dstats(struct dstats *curr[], struct dstats *prev[], int n_dev) {
    int i;
    for (i = 0; i < n_dev; i++) {
        prev[i]->r_completed = curr[i]->r_completed;
        prev[i]->r_merged = curr[i]->r_merged;
        prev[i]->r_sectors = curr[i]->r_sectors;
        prev[i]->r_spent = curr[i]->r_spent;
        prev[i]->w_completed = curr[i]->w_completed;
        prev[i]->w_merged = curr[i]->w_merged;
        prev[i]->w_sectors = curr[i]->w_sectors;
        prev[i]->w_spent = curr[i]->w_spent;
        prev[i]->io_in_progress = curr[i]->io_in_progress;
        prev[i]->t_spent = curr[i]->t_spent;
        prev[i]->t_weighted = curr[i]->t_weighted;
    }
}

int main (void) {
    static unsigned long long uptime0[2] = {0, 0};
    static unsigned long long itv;
    static int curr = 1;

    FILE * diskstats_fp;
    int i, curr_n_dev = 0, prev_n_dev = 0;
    char ch;
    char line[128];
    bool first = true;
 
    get_HZ();

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr,TRUE);

    diskstats_fp = fopen(DISKSTATS_FILE, "r");       /* todo: sanity check if open fails - писать ошибку что не могу прочитать */
    while (!feof(diskstats_fp)) {
        ch = fgetc(diskstats_fp);
        if (ch == '\n')
            curr_n_dev++;
    }
    fclose(diskstats_fp);
    
    struct dstats *curr_dstats[curr_n_dev];
    struct dstats *prev_dstats[curr_n_dev];
    struct ext_dstats *xds[curr_n_dev];

    for (i = 0; i < curr_n_dev; i++) {
        curr_dstats[i] = (struct dstats *) malloc(sizeof(struct dstats));       /* todo: sanity check if malloc failed */
        prev_dstats[i] = (struct dstats *) malloc(sizeof(struct dstats));       /* todo: sanity check if malloc failed */
        xds[i] = (struct ext_dstats *) malloc(sizeof(struct ext_dstats));
    }
    double r_await[curr_n_dev], w_await[curr_n_dev];

    do {
        uptime0[curr] = 0;
        read_uptime(&(uptime0[curr]));
     
        i = 0;
        int major, minor;
        char devname[64];
        unsigned long r_completed, r_merged, r_sectors, r_spent,
                      w_completed, w_merged, w_sectors, w_spent,
                      io_in_progress, t_spent, t_weighted;

        diskstats_fp = fopen(DISKSTATS_FILE, "r");       /* todo: sanity check if open fails*/
        while (fgets(line, sizeof(line), diskstats_fp) != NULL) {
            sscanf(line, "%i %i %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                        &major, &minor, devname,
                        &r_completed, &r_merged, &r_sectors, &r_spent,
                        &w_completed, &w_merged, &w_sectors, &w_spent,
                        &io_in_progress, &t_spent, &t_weighted);
            curr_dstats[i]->major = major;
            curr_dstats[i]->minor = minor;
            strcpy(curr_dstats[i]->devname, devname);
            curr_dstats[i]->r_completed = r_completed;
            curr_dstats[i]->r_merged = r_merged;
            curr_dstats[i]->r_sectors = r_sectors;
            curr_dstats[i]->r_spent = r_spent;
            curr_dstats[i]->w_completed = w_completed;
            curr_dstats[i]->w_merged = w_merged;
            curr_dstats[i]->w_sectors = w_sectors;
            curr_dstats[i]->w_spent = w_spent;
            curr_dstats[i]->io_in_progress = io_in_progress;
            curr_dstats[i]->t_spent = t_spent;
            curr_dstats[i]->t_weighted = t_weighted;
            i++;
        }
        fclose(diskstats_fp);

        itv = get_interval(uptime0[!curr], uptime0[curr]);
        
        for (i = 0; i < curr_n_dev; i++) {
            xds[i]->util = S_VALUE(prev_dstats[i]->t_spent, curr_dstats[i]->t_spent, itv);
            xds[i]->await = ((curr_dstats[i]->r_completed + curr_dstats[i]->w_completed) - (prev_dstats[i]->r_completed + prev_dstats[i]->w_completed)) ?
                ((curr_dstats[i]->r_spent - prev_dstats[i]->r_spent) + (curr_dstats[i]->w_spent - prev_dstats[i]->w_spent)) /
                ((double) ((curr_dstats[i]->r_completed + curr_dstats[i]->w_completed) - (prev_dstats[i]->r_completed + prev_dstats[i]->w_completed))) : 0.0;
            xds[i]->arqsz = ((curr_dstats[i]->r_completed + curr_dstats[i]->w_completed) - (prev_dstats[i]->r_completed + prev_dstats[i]->w_completed)) ?
                ((curr_dstats[i]->r_sectors - prev_dstats[i]->r_sectors) + (curr_dstats[i]->w_sectors - prev_dstats[i]->w_sectors)) /
                ((double) ((curr_dstats[i]->r_completed + curr_dstats[i]->w_completed) - (prev_dstats[i]->r_completed + prev_dstats[i]->w_completed))) : 0.0;

            r_await[i] = (curr_dstats[i]->r_completed - prev_dstats[i]->r_completed) ?
                    (curr_dstats[i]->r_spent - prev_dstats[i]->r_spent) /
                    ((double) (curr_dstats[i]->r_completed - prev_dstats[i]->r_completed)) : 0.0;
            w_await[i] = (curr_dstats[i]->w_completed - prev_dstats[i]->w_completed) ?
                    (curr_dstats[i]->w_spent - prev_dstats[i]->w_spent) /
                    ((double) (curr_dstats[i]->w_completed - prev_dstats[i]->w_completed)) : 0.0;
        }

        /* print headers */
        clear();
        printw("device:           rrqm/s  wrqm/s      r/s      w/s    rMB/s    wMB/s avgrq-sz avgqu-sz     await   r_await   w_await   %%util\n");

        for (i = 0; i < curr_n_dev; i++) {
            printw("%s\t\t", curr_dstats[i]->devname);
            printw("%8.2f%8.2f",
                    S_VALUE(prev_dstats[i]->r_merged, curr_dstats[i]->r_merged, itv),
                    S_VALUE(prev_dstats[i]->w_merged, curr_dstats[i]->w_merged, itv));
            printw("%9.2f%9.2f",
                    S_VALUE(prev_dstats[i]->r_completed, curr_dstats[i]->r_completed, itv),
                    S_VALUE(prev_dstats[i]->w_completed, curr_dstats[i]->w_completed, itv));
            printw("%9.2f%9.2f%9.2f%9.2f",
                    S_VALUE(prev_dstats[i]->r_sectors, curr_dstats[i]->r_sectors, itv) / 2048,
                    S_VALUE(prev_dstats[i]->w_sectors, curr_dstats[i]->w_sectors, itv) / 2048,
                    xds[i]->arqsz,
                    S_VALUE(prev_dstats[i]->t_weighted, curr_dstats[i]->t_weighted, itv) / 1000.0);
            printw("%10.2f%10.2f%10.2f", xds[i]->await, r_await[i], w_await[i]);
            printw("%8.2f", xds[i]->util / 10.0);
            printw("\n");
        }
        refresh();

        replace_dstats(curr_dstats, prev_dstats, curr_n_dev);
        curr ^= 1;
        sleep(1);
    } while (1);
        return 0;
}
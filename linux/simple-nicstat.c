#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>

#define HZ  hz
#define NETDEV_FILE "/proc/net/dev"

#define S_VALUE(m,n,p) (((double) ((n) - (m))) / (p) * HZ)

unsigned int hz;

inline static double min(double d1, double d2)
{
    if (d1 < d2)
        return (d1);
    return (d2);
}

inline static double max(double d1, double d2)
{
    if (d1 > d2)
        return (d1);
    return (d2);
}

void get_HZ(void)
{
    long ticks;
    if ((ticks = sysconf(_SC_CLK_TCK)) == -1) {
        perror("sysconf");
    }
    hz = (unsigned int) ticks;
}

struct nicdata
{
    char ifname[IF_NAMESIZE + 1];
    long speed;
    int duplex;
    unsigned long rbytes;
    unsigned long rpackets;
    unsigned long ierr;
    unsigned long wbytes;
    unsigned long wpackets;
    unsigned long oerr;
    unsigned long coll;
    unsigned long sat;
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

void replace_nicdata(struct nicdata *curr[], struct nicdata *prev[], int n_dev) {
    int i;
    for (i = 0; i < n_dev; i++) {
        prev[i]->rbytes = curr[i]->rbytes;
        prev[i]->rpackets = curr[i]->rpackets;
        prev[i]->wbytes = curr[i]->wbytes;
        prev[i]->wpackets = curr[i]->wpackets;
        prev[i]->ierr = curr[i]->ierr;
        prev[i]->oerr = curr[i]->oerr;
        prev[i]->coll = curr[i]->coll;
        prev[i]->sat = curr[i]->sat;
    }
}

void get_speed_duplex(struct nicdata * nicdata)
{
    struct ifreq ifr;
    struct ethtool_cmd edata;
    int status, sock;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    strncpy(ifr.ifr_name, nicdata->ifname, sizeof (ifr.ifr_name));
    ifr.ifr_data = (void *) &edata;
    edata.cmd = ETHTOOL_GSET;
    status = ioctl(sock, SIOCETHTOOL, &ifr);
    if (status < 0) {
        nicdata->speed = -1;
        nicdata->duplex = DUPLEX_UNKNOWN;
        return;
    }
    nicdata->speed = edata.speed * 1000000;
    nicdata->duplex = edata.duplex;

    close(sock);
}

int main (void) {
    static unsigned long long uptime0[2] = {0, 0};
    static unsigned long long itv;
    static int curr = 1;

    FILE * netdev_fp;
    int i, j, curr_n_dev = 0, prev_n_dev = 0;
    char ch;
    char line[512];
    bool first = true;
 
    get_HZ();

//    initscr();
//    cbreak();
//    noecho();
//    nodelay(stdscr, TRUE);
//    keypad(stdscr,TRUE);

    netdev_fp = fopen(NETDEV_FILE, "r");       /* todo: sanity check if open fails - писать ошибку что не могу прочитать */
    while (!feof(netdev_fp)) {
        ch = fgetc(netdev_fp);
        if (ch == '\n')
            curr_n_dev++;
    }
    curr_n_dev = curr_n_dev - 2;            /* 2-line header */
    fclose(netdev_fp);
    
    struct nicdata *curr_nicdata[curr_n_dev];
    struct nicdata *prev_nicdata[curr_n_dev];

    for (i = 0; i < curr_n_dev; i++) {
        curr_nicdata[i] = (struct nicdata *) malloc(sizeof(struct nicdata));       /* todo: sanity check if malloc failed */
        prev_nicdata[i] = (struct nicdata *) malloc(sizeof(struct nicdata));       /* todo: sanity check if malloc failed */
    }

    do {
        uptime0[curr] = 0;
        read_uptime(&(uptime0[curr]));
     
        i = j = 0;
        char ifname[IF_NAMESIZE + 1];
        unsigned long lu[16];

        netdev_fp = fopen(NETDEV_FILE, "r");       /* todo: sanity check if open fails*/
        while (fgets(line, sizeof(line), netdev_fp) != NULL) {
            if (j < 2) {
                j++;
                continue;       /* skip headers */
            }
            sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                        ifname,
//                        &rbytes, &rpackets, &rerrs, &rdrop, &rfifo, &rframe, &rcomp, &rmcast,
//                        &tbytes, &tpackets, &terrs, &tdrop, &tfifo, &tcoll, &tcarrier, &tcomp);
                        &lu[0], &lu[1], &lu[2], &lu[3], &lu[4], &lu[5], &lu[6], &lu[7],
                        &lu[8], &lu[9], &lu[10], &lu[11], &lu[12], &lu[13], &lu[14], &lu[15]);
            strcpy(curr_nicdata[i]->ifname, ifname);
            curr_nicdata[i]->rbytes = lu[0];
            curr_nicdata[i]->rpackets = lu[1];
            curr_nicdata[i]->wbytes = lu[8];
            curr_nicdata[i]->wpackets = lu[9];
            curr_nicdata[i]->ierr = lu[2];
            curr_nicdata[i]->oerr = lu[10];
            curr_nicdata[i]->coll = lu[13];
            curr_nicdata[i]->sat = lu[2];
            curr_nicdata[i]->sat += lu[3];
            curr_nicdata[i]->sat += lu[11];
            curr_nicdata[i]->sat += lu[12];
            curr_nicdata[i]->sat += lu[13];
            curr_nicdata[i]->sat += lu[14];
            i++;
        }
        fclose(netdev_fp);

        if (first) {
            for (i = 0; i < curr_n_dev; i++)
                get_speed_duplex(curr_nicdata[i]);
            first = true;
        }

        itv = get_interval(uptime0[!curr], uptime0[curr]);
        
        /* print headers */
//        clear();
        printf("interface:   rMbps   wMbps    rPk/s    wPk/s     rAvs     wAvs     IErr     OErr     Coll      Sat   %%rUtil   %%wUtil    %%Util\n");

        double rbps, rpps, wbps, wpps, ravs, wavs, ierr, oerr, coll, sat, rutil, wutil, util;

        for (i = 0; i < curr_n_dev; i++) {
            if (curr_nicdata[i]->rpackets == 0 && curr_nicdata[i]->wpackets == 0) {
                continue;
            }
            
            rbps = S_VALUE(prev_nicdata[i]->rbytes, curr_nicdata[i]->rbytes, itv);
            wbps = S_VALUE(prev_nicdata[i]->wbytes, curr_nicdata[i]->wbytes, itv);
            rpps = S_VALUE(prev_nicdata[i]->rpackets, curr_nicdata[i]->rpackets, itv);
            wpps = S_VALUE(prev_nicdata[i]->wpackets, curr_nicdata[i]->wpackets, itv);
            ierr = S_VALUE(prev_nicdata[i]->ierr, curr_nicdata[i]->ierr, itv);
            oerr = S_VALUE(prev_nicdata[i]->oerr, curr_nicdata[i]->oerr, itv);
            coll = S_VALUE(prev_nicdata[i]->coll, curr_nicdata[i]->coll, itv);
            sat = S_VALUE(prev_nicdata[i]->sat, curr_nicdata[i]->sat, itv);

            if (rpps > 0)
                ravs = rbps / rpps;
            else
                ravs = 0;
            
            if (wpps > 0)
                wavs = wbps / wpps;
            else
                wavs = 0;

            /* Calculate utilisation */
            if (curr_nicdata[i]->speed > 0) {
                /*
                 * The following have a mysterious "800", it is
                 * 100 for the % conversion, and 8 for
                 * bytes2bits.
                 */
                rutil = min(rbps * 800 / curr_nicdata[i]->speed, 100);
                wutil = min(wbps * 800 / curr_nicdata[i]->speed, 100);
                if (curr_nicdata[i]->duplex == 2) {
                    /* Full duplex */
                    util = max(rutil, wutil);
                } else {
                    /* Half Duplex */
                    util = min((rbps + wbps) * 800 / curr_nicdata[i]->speed, 100);
                }
            } else {
                util = 0;
                rutil = 0;
                wutil = 0;
            }

            printf("%10s", curr_nicdata[i]->ifname);
            printf("%8.2f%8.2f", rbps / 1024 / 128, wbps / 1024 / 128);
            printf("%9.2f%9.2f", rpps, wpps);
            printf("%9.2f%9.2f", ravs, wavs);
            printf("%9.2f%9.2f%9.2f%9.2f", ierr, oerr, coll, sat);
            printf("%9.2f%9.2f%9.2f", rutil, wutil, util);
            printf("\n");
        }
//        refresh();

        replace_nicdata(curr_nicdata, prev_nicdata, curr_n_dev);
        curr ^= 1;
        sleep(1);
    } while (1);
        return 0;
}
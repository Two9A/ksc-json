#include "main.h"
#include "log.h"

extern int opt_verbose;
extern int opt_console;

void log_write(char *str, int verbose)
{
    if (verbose && !opt_verbose) {
        return;
    }

    if (opt_console) {
        time_t curtime;
        struct tm *timeinfo;
        char *timestr;

        time(&curtime);
        timestr = ctime(&curtime);
        timestr[strlen(timestr) - 1] = 0;
        fprintf(stderr, "[%s] %s\n", timestr, str);
    } else {
        openlog("ksc-websocket", LOG_NDELAY | LOG_PID, LOG_DAEMON);
        syslog(LOG_NOTICE, str);
        closelog();
    }
}


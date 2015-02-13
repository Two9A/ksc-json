#include "main.h"
#include "ksc.h"
#include "log.h"

int opt_verbose;
int opt_console;
char *opt_file;

int main(int argc, char **argv)
{
    opt_verbose = false;
    opt_console = false;
    opt_file = KSC_OUT_FILE;

    if (!main_parse_options(argc, argv)) {
        exit(EXIT_FAILURE);
    }

    //websocket_open();
    while (ksc_fetch()) {
        //websocket_shout();
        sleep(1);
    }

    free(opt_file);
    exit(EXIT_SUCCESS);
}

int main_read_optarg(char opt)
{
    char err[128];

    switch (opt) {
        case 'o':
            opt_file = (char*)malloc(strlen(optarg) + 1);
            strncpy(opt_file, optarg, strlen(optarg));
            break;
    }

    return true;
}

int main_parse_options(int argc, char **argv)
{
    while (1) {
        int opt, opt_idx = 0;
        static struct option long_opts[] = {
            {"verbose", no_argument,        &opt_verbose,   1},
            {"console", no_argument,        &opt_console,   1},
            {"output",  required_argument,  0,              0},
            {"help",    no_argument,        0,              0},
            {0,         0,                  0,              0}
        };

        opt = getopt_long(argc, argv, "vco:h", long_opts, &opt_idx);
        if (opt == -1) {
            // End of options
            break;
        }

        switch (opt) {
            case 0:
                switch (opt_idx) {
                    case 2:
                        if (!main_read_optarg('o')) {
                            return false;
                        }
                        break;

                    case 3:
                        main_show_help();
                        break;
                }
                break;

            case 'v':
                opt_verbose = true;
                break;

            case 'c':
                opt_console = true;
                break;

            case 'o':
                if (!main_read_optarg(opt)) {
                    return false;
                }
                break;

            case 'h':
                main_show_help();
                break;
        }
    }

    return true;
}

void main_show_help()
{
    printf(
        "KSC Countdown Telemetry to JSON proxy\n"
        "Usage:\n"
        "    -o <file>, --output <file>: JSON output file\n"
        "    -v, --verbose: Verbose output\n"
        "    -c, --console: Output to console instead of syslog\n"
        "    -?, --help: Print this message\n"
    );
    exit(EXIT_SUCCESS);
}


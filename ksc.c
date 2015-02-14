#include "main.h"
#include "ksc.h"
#include "hash.h"
#include "log.h"

extern char *opt_file;

static ksc_entry ksc_data[KSC_DATASETS][KSC_KEY_COUNT];

int ksc_connect()
{
    struct addrinfo hints, *hosts, *p;
    struct sockaddr_in *addr;
    struct timeval timeout;
    int status, i, j;
    char str[128];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if ((status = getaddrinfo(KSC_HOST, NULL, &hints, &hosts)) != 0) {
        sprintf(str, "Error resolving host: %s", gai_strerror(status));
        log_write(str, false);
        return false;
    }

    /* Pull an IP for the host. This'll always get the last one,
    but in the canonical case there's only one */
    for (p = hosts; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            addr = (struct sockaddr_in *)p->ai_addr;

            for (i = 0; i < KSC_DATASETS; i++) {
                switch (i) {
                    case KSC_DATASET_KSC:
                        addr->sin_port = htons(KSC_PORT_KSC);
                        break;

                    case KSC_DATASET_VAFB:
                        addr->sin_port = htons(KSC_PORT_VAFB);
                        break;
                }

                ksc_sock[i] = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
                if (ksc_sock[i] == -1) {
                    sprintf(str, "Error getting socket: %s", strerror(errno));
                    log_write(str, false);
                    return false;
                }

                if (connect(ksc_sock[i], (struct sockaddr *)addr, p->ai_addrlen) != 0) {
                    sprintf(str, "Error connecting: %s", strerror(errno));
                    log_write(str, false);
                    return false;
                }

                setsockopt(ksc_sock[i], SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
            }
            break;
        }
    }

    static ksc_entry entries[] = {
        {"GMTTSTRG01", KSC_VAL_RAW, 12},     // GMT Time - ddd:hh:mm:ss
        {"LOCLSTRG02", KSC_VAL_RAW, 3},      // Local Time Zone Label - EST or PST
        {"LOCTSTRG03", KSC_VAL_RAW, 12},     // Local Time - ddd:hh:mm:ss
        {"BIHTSTRG04", KSC_VAL_RAW, 8},      // Built in Hold - hh:mm:ss
        {"WTRTSTRG05", KSC_VAL_RAW, 12},     // Expected Liftoff - ddd:hh:mm:ss
        {"WOTTSTRG06", KSC_VAL_RAW, 12},     // Window Time Remaining - hh:mm:ss
        {"ELOTSTRG07", KSC_VAL_RAW, 12},     // Window Open - ddd:hh:mm:ss
        {"ALOLSTRG08", KSC_VAL_RAW, 14},     // "Actual Liftoff" Label
        {"ALOTSTRG09", KSC_VAL_RAW, 12},     // Liftoff Time - ddd:hh:mm:ss
        {"LTMLSTRG10", KSC_VAL_RAW, 6},      // "L-time" Label
        {"LTMTSTRG11", KSC_VAL_RAW, 12},     // L-Time - hh:mm:ss
        {"TTMLSTRG12", KSC_VAL_RAW, 12},     // "T-Time" Label
        {"TTMTSTRG13", KSC_VAL_RAW, 12},     // T-Time - hh:mm:ss
        {"RSMLSTRG14", KSC_VAL_RAW, 13},     // "Resume Time"
        {"RSMTSTRG15", KSC_VAL_RAW, 8},      // Resume Time - hh:mm:ss
        {"VHCLSTRG16", KSC_VAL_RAW, 12},     // Vehicle String
        {"SPCFSTRG17", KSC_VAL_RAW, 12},     // Spacecraft string
        {"L_M-LBL000", KSC_VAL_RAW, 12},     // Rt screen - Top Label
        {"L_M-TIME00", KSC_VAL_RAW, 12},     // Rt screen - Time hh:mm:ss
        {"CNT_FLT_MD", KSC_VAL_RAW, 17},     // Flight Mode
        {"CURENTBLNK", KSC_VAL_RAW, 1},      // Blink 1=on, 0=off
        {"CUREVNTBOX", KSC_VAL_RAW, 1},      // Event Box
        {"WEBMESSAGE", KSC_VAL_RAW, 51},     // MOD Broadcast Message
        {"EVENTLBL01", KSC_VAL_RAW, 20},
        {"EVENTTIM01", KSC_VAL_RAW, 11},
        {"EVENTLBL02", KSC_VAL_RAW, 20},
        {"EVENTTIM02", KSC_VAL_RAW, 11},
        {"EVENTLBL03", KSC_VAL_RAW, 20},
        {"EVENTTIM03", KSC_VAL_RAW, 11},
        {"EVENTLBL04", KSC_VAL_RAW, 20},
        {"EVENTTIM04", KSC_VAL_RAW, 11},
        {"EVENTLBL05", KSC_VAL_RAW, 20},
        {"EVENTTIM05", KSC_VAL_RAW, 11},
        {"EVENTLBL06", KSC_VAL_RAW, 20},
        {"EVENTTIM06", KSC_VAL_RAW, 11},
        {"EVENTLBL07", KSC_VAL_RAW, 20},
        {"EVENTTIM07", KSC_VAL_RAW, 11},
        {"EVENTLBL08", KSC_VAL_RAW, 20},
        {"EVENTTIM08", KSC_VAL_RAW, 11},
        {"EVENTLBL09", KSC_VAL_RAW, 20},
        {"EVENTTIM09", KSC_VAL_RAW, 11},
        {"EVENTLBL10", KSC_VAL_RAW, 20},
        {"EVENTTIM10", KSC_VAL_RAW, 11}
    };

    for (i = 0; i < KSC_DATASETS; i++) {
        for (j = 0; j < KSC_KEY_COUNT; j++) {
            strncpy(ksc_data[i][j].key, entries[j].key, KSC_KEY_LEN);
            ksc_data[i][j].key[KSC_KEY_LEN] = 0;

            ksc_data[i][j].key_crc = crc16(entries[j].key, KSC_KEY_LEN);
            ksc_data[i][j].val_type = entries[j].val_type;
            ksc_data[i][j].val_len = entries[j].val_len;
        }
    }

    return true;
}

int ksc_fetch()
{
    char str[128];
    int len, i, parsed = 0;

    if (!(ksc_sock[KSC_DATASET_KSC] && ksc_sock[KSC_DATASET_VAFB])) {
        ksc_connect();
    }

    for (i = 0; i < KSC_DATASETS; i++) {
        if (!send(ksc_sock[i], "\000", 1, 0)) {
            sprintf(str, "Line %d: Error requesting data: %s", i, strerror(errno));
            log_write(str, false);
            return false;
        }

        if ((len = recv(ksc_sock[i], ksc_recvbuf[i], 2048, 0)) <= 0) {
            if (errno == EAGAIN) {
                log_write("Timeout receiving data", false);
                return true;
            }
            sprintf(str, "Line %d: Error receiving data: %s", i, strerror(errno));
            log_write(str, false);
            return false;
        }

        sprintf(str, "Line %d: Received %d bytes", i, len);
        log_write(str, true);

        if (len >= 3) {
            if (ksc_parse(i)) {
                parsed++;
            }
        }
    }

    if (parsed == KSC_DATASETS) {
        ksc_output();
    }

    return true;
}

int ksc_parse(int dataset)
{
    char pattern = ksc_recvbuf[dataset][0];
    char version = ksc_recvbuf[dataset][1];
    char changes = ksc_recvbuf[dataset][2];

    int i, j, key_offs = 3, val_offs = key_offs + changes * KSC_KEY_LEN;
    unsigned short crc;

    if (pattern != KSC_PATTERN) {
        log_write("Parse: Invalid pattern, skipping", false);
        return false;
    }
    if (version != KSC_VERSION) {
        log_write("Parse: Invalid version, skipping", false);
        return false;
    }
    if (changes > KSC_KEY_COUNT) {
        log_write("Parse: Too many keys, skipping", false);
        return false;
    }

    for (i = 0; i < changes; i++, key_offs += KSC_KEY_LEN) {
        crc = crc16(ksc_recvbuf[dataset] + key_offs, KSC_KEY_LEN);

        for (j = 0; j < KSC_KEY_COUNT; j++) {
            if (crc == ksc_data[dataset][j].key_crc) {
                // TODO: Handle the different types of value data
                // All data currently returned is RAW
                ksc_data[dataset][j].val = ksc_recvbuf[dataset] + val_offs;
                val_offs += ksc_data[dataset][j].val_len;
                break;
            }
        }
    }

    return true;
}

int ksc_output()
{
    json_t *root;
    json_t *datasets[KSC_DATASETS], *raw[KSC_DATASETS];

    int i, j;
    FILE *fp;
    
    for (i = 0; i < KSC_DATASETS; i++) {
        datasets[i] = json_object();
        raw[i] = json_object();

        for (j = 0; j < KSC_KEY_COUNT; j++) {
            json_object_set_new(raw[i], ksc_data[i][j].key, json_string(ksc_data[i][j].val));
        }

        json_object_set_new(datasets[i], "raw", raw[i]);
    }

    root = json_pack(
        "{soso}",
        "ksc", datasets[KSC_DATASET_KSC],
        "vafb", datasets[KSC_DATASET_VAFB]
    );

    fp = fopen(opt_file, "w");
    if (!fp) {
        log_write("Output: Failed to open", false);
        return false;
    }

    fprintf(fp, "%s", json_dumps(root, 0));
    fclose(fp);

    for (i = 0; i < KSC_DATASETS; i++) {
        json_decref(raw[i]);
        json_decref(datasets[i]);
    }
    json_decref(root);

    return true;
}


#ifndef __KSC_H_
#define __KSC_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <jansson.h>

#define KSC_HOST        "countdown.ksc.nasa.gov"
#define KSC_PORT_KSC    11142
#define KSC_PORT_VAFB   11144
#define KSC_OUT_FILE    "ksc.json"

#define KSC_PATTERN     10
#define KSC_VERSION      1
#define KSC_KEY_COUNT   43
#define KSC_KEY_LEN     10

#define KSC_VAL_RAW      1
#define KSC_VAL_SIGNED   2
#define KSC_VAL_UNSIGNED 3
#define KSC_VAL_FLOAT    4
#define KSC_VAL_BOOL     5
#define KSC_VAL_STR      6

#define KSC_DATASET_KSC     0
#define KSC_DATASET_VAFB    1
#define KSC_DATASETS        2

int ksc_sock[KSC_DATASETS];
typedef struct {
    char key[11];
    char val_type;
    char val_len;
    unsigned short key_crc;
    char *val;
} ksc_entry;

char ksc_recvbuf[KSC_DATASETS][2048];

int ksc_fetch();
int ksc_parse();
int ksc_output();

#endif//__KSC_H_

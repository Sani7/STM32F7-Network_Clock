#pragma once

#include <stdint.h>
#include <string.h>

#include "lwip.h"
#include "udp.h"
#include "dns.h"
#include "pbuf.h"
#include "ip_addr.h"

#include "debug.h"

#define NTP_OFFSET_TIMESTAMPS 40
#define NTP_OFFSET_ROUNDING 44
#define SECONDROUNDINGTHRESHOLD 115

// the epoch for NTP starts in year 1900 while the epoch in UNIX starts in 1970
// Unix time starts on Jan 1 1970. 7O years difference in seconds, that's 2208988800 seconds
#define SEVENTYYEARS 2208988800UL

//#define NTP_HOST_NAME "time1.google.com"
#define NTP_HOST_NAME "pool.ntp.org"
#define NTP_PORT 123

#define NTP_PACKET_SIZE 48           // NTP time stamp is in the first 48 bytes of the message
#define ntpFirstFourBytes 0xEC0600E3 // NTP request header, first 32 bits
#define UTC_DELTA ((UTC_DELTA_HOURS * NUMBEROFSECONDSPERHOUR) + (UTC_DELTA_MINUTES * NUMBEROFSECONDSPERMINUTE))

typedef enum
{
    NTP_IDLE,
    NTP_Receiving_DNS,
    NTP_DNS_GOT_IP,
    NTP_Receiving_UDP,
    NTP_GOT_TIME,
    NTP_ERROR
} NTPState_t;

typedef struct
{
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precesion;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t reference_identifier;
    uint32_t reference_timestamp[2];
    uint32_t originate_timestamp[2];
    uint32_t receive_timestamp[2];
    uint32_t transmit_timestamp[2];
} NTP_Packet;


uint32_t NTPToEpochUnix(void);
void NTP_DNS_Callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
void NTP_RECV_CALLBACK(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);


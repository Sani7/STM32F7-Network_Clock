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

#define NTP_OFFSET_LI_VN_MODE      0
#define NTP_LI_MASK                0xC0
#define NTP_LI_NO_WARNING          (0x00 << 6)
#define NTP_LI_LAST_MINUTE_61_SEC  (0x01 << 6)
#define NTP_LI_LAST_MINUTE_59_SEC  (0x02 << 6)
#define NTP_LI_ALARM_CONDITION     (0x03 << 6) /* (clock not synchronized) */

#define NTP_VERSION_MASK           0x38
#define NTP_VERSION                (4/* NTP Version 4*/<<3)

#define NTP_MODE_MASK              0x07
#define NTP_MODE_CLIENT            0x03
#define NTP_MODE_SERVER            0x04
#define NTP_MODE_BROADCAST         0x05

#define NTP_OFFSET_STRATUM         1
#define NTP_STRATUM_KOD            0x00

#define NTP_OFFSET_ORIGINATE_TIME  24
#define NTP_OFFSET_RECEIVE_TIME    32
#define NTP_OFFSET_TRANSMIT_TIME   40

typedef enum
{
    NTP_IDLE,
    NTP_Receiving_DNS,
    NTP_DNS_GOT_IP,
    NTP_Receiving_UDP,
    NTP_GOT_TIME,
    NTP_ERROR
} NTPState_t;

PACK_STRUCT_BEGIN
typedef struct {
  PACK_STRUCT_FLD_8(uint8_t  li_vn_mode);
  PACK_STRUCT_FLD_8(uint8_t  stratum);
  PACK_STRUCT_FLD_8(uint8_t  poll);
  PACK_STRUCT_FLD_8(uint8_t  precision);
  PACK_STRUCT_FIELD(uint32_t root_delay);
  PACK_STRUCT_FIELD(uint32_t root_dispersion);
  PACK_STRUCT_FIELD(uint32_t reference_identifier);
  PACK_STRUCT_FIELD(uint32_t reference_timestamp[2]);
  PACK_STRUCT_FIELD(uint32_t originate_timestamp[2]);
  PACK_STRUCT_FIELD(uint32_t receive_timestamp[2]);
  PACK_STRUCT_FIELD(uint32_t transmit_timestamp[2]);
} NTP_Frame;
PACK_STRUCT_END


uint32_t NTPToEpochUnix(void);
void NTP_DNS_Callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
void NTP_RECV_CALLBACK(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);


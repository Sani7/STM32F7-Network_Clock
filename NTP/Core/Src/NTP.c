#include "NTP.h"

NTPState_t NTPState = NTP_IDLE;

typedef struct
{
    void *ptr1;
    void *ptr2;
} ptrArray_t;

uint32_t NTPToEpochUnix(void)
{
    uint32_t secsSince1900 = 0UL;
    ip_addr_t NTP_SERVER_IP;
    err_t ret;
    struct udp_pcb *udp_pcb;
    struct pbuf *pbuf;
    NTPState = NTP_IDLE;
    debugln("Getting NTP");
    while (NTPState != NTP_GOT_TIME)
    {
        switch (NTPState)
        {
        case NTP_IDLE:
            ret = dns_gethostbyname(NTP_HOST_NAME, &NTP_SERVER_IP, NTP_DNS_Callback,
                                    &NTP_SERVER_IP);
            if (ret == ERR_OK)
            {
                NTPState = NTP_DNS_GOT_IP;
            }
            else if (ret == ERR_INPROGRESS)
            {
                NTPState = NTP_Receiving_DNS;
            }
            else
            {
                debugErrln("Error while getting NTP server IP: %d", ret);
                return 0;
            }
            break;
        case NTP_Receiving_DNS:
            break;
        case NTP_DNS_GOT_IP:
            pbuf = pbuf_alloc(PBUF_TRANSPORT, NTP_PACKET_SIZE, PBUF_RAM);

            if (pbuf == NULL)
            {
                debugErrln("Error while allocating pbuf for NTP packet");
                return 0;
            }
            *((uint32_t*)pbuf->payload) = ntpFirstFourBytes;
            pbuf->len = NTP_PACKET_SIZE;
            pbuf->tot_len = NTP_PACKET_SIZE;

            udp_pcb = udp_new();

            udp_connect(udp_pcb, &NTP_SERVER_IP, NTP_PORT);

            (ret = udp_send(udp_pcb, pbuf));
            if (ret != ERR_OK)
            {
                pbuf_free(pbuf);
                udp_remove(udp_pcb);
                debugErrln("Error while sending NTP packet over UDP: %d", ret);
                return 0;
            }

            debugln("NTP packet sent");

            udp_recv(udp_pcb, NTP_RECV_CALLBACK, &secsSince1900);
            NTPState = NTP_Receiving_UDP;
            break;
        case NTP_Receiving_UDP:
            break;
        case NTP_GOT_TIME:
            break;
        case NTP_ERROR:
            pbuf_free(pbuf);
            udp_disconnect(udp_pcb);
            udp_remove(udp_pcb);
            return 0;
        }
        vTaskDelay(10);
    }
    udp_disconnect(udp_pcb);
    udp_remove(udp_pcb);
    pbuf_free(pbuf);
    debugln("epoch: %lu", secsSince1900 - SEVENTYYEARS);
    return secsSince1900 - SEVENTYYEARS;
}

void NTP_DNS_Callback(const char *name, const ip_addr_t *ipaddr,
                      void *callback_arg)
{
    if (ipaddr == NULL)
    {
        debugErrln("NTP_DNS_Callback: returned and ip_addr_t ptr to NULL");
        return;
    }
    if (strncmp(name, NTP_HOST_NAME, strlen(NTP_HOST_NAME)) == 0)
    {
        *((ip_addr_t *)callback_arg) = *ipaddr;
        debugln("%s: %s", name, ip4addr_ntoa((ip_addr_t *)callback_arg));
        NTPState = NTP_DNS_GOT_IP;
        return;
    }
    debugErrln("NTP_DNS_Callback: No DNS resolved");
    NTPState = NTP_ERROR;
}

void NTP_RECV_CALLBACK(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    uint32_t secsSince1900 = 0UL;

    /* process the response */
    if (p->tot_len != NTP_PACKET_SIZE)
    {
        pbuf_free(p);
        debugErrln("NTP_RECV_CALLBACK: invalid packet size: %d", p->tot_len);
        return;
    }
    /* this is a SNTP response... */
    for (int i = NTP_OFFSET_TIMESTAMPS; i < NTP_OFFSET_TIMESTAMPS + sizeof(uint32_t); i++)
    {
    	secsSince1900 = (secsSince1900 << 8) + ((uint8_t*)(p->payload))[i];
    }

    secsSince1900 += (((uint8_t*)(p->payload))[NTP_OFFSET_ROUNDING] > SECONDROUNDINGTHRESHOLD ? 1 : 0);

    NTPState = NTP_GOT_TIME;
    pbuf_free(p);
    *((uint32_t *)arg) = secsSince1900;
}


// Simple UDP listening server
//
// Author: Brian Lee
//
// Copyright: GPL V2
// See http://www.gnu.org/licenses/gpl.html

#include "EtherCard.h"
#include "net.h"
#include "rprintf.h"

#define gPB ether.buffer

#define DEBUG

#define UDPSERVER_MAXLISTENERS 8    //the maximum number of port listeners.

typedef struct {
    UdpServerCallback callback;
    uint32_t port;
    bool listening;
} __attribute__((packed)) UdpServerListener;

UdpServerListener listeners[UDPSERVER_MAXLISTENERS];
byte numListeners = 0;

void EtherCard::udpServerListenOnPort(UdpServerCallback callback, uint32_t port) {
    if(numListeners < UDPSERVER_MAXLISTENERS)
    {
        listeners[numListeners] = (UdpServerListener){callback, port, true};
        numListeners++;
    }
}

void EtherCard::udpServerPauseListenOnPort(uint32_t port) {
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port)) {
            listeners[i].listening = false;
        }
    }
}

void EtherCard::udpServerResumeListenOnPort(uint32_t port) {
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port)) {
            listeners[i].listening = true;
        }
    }
}

bool EtherCard::udpServerListening() {
    return numListeners > 0;
}

bool EtherCard::udpServerHasProcessedPacket(uint32_t plen) {
    bool packetProcessed = false;
    for(int i = 0; i < numListeners; i++)
    {
        #ifdef DEBUG
        rprintf("Dist port: %d\n", gPB[UDP_DST_PORT_H_P]);
        #endif

        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port) && listeners[i].listening)
        {
            uint32_t datalen = (uint32_t) (gPB[UDP_LEN_H_P] << 8)  + gPB[UDP_LEN_L_P] - UDP_HEADER_LEN;
            listeners[i].callback(
                listeners[i].port,
                gPB + IP_SRC_P,
                (const char *) (gPB + UDP_DATA_P),
                datalen);
            packetProcessed = true;
        }
    }
    return packetProcessed;
}

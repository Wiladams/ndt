#pragma once

#include <cstdint>
#include <cstdio>
#include "binstream.hpp"
#include "binops.hpp"
#include "bitbang.hpp"

using namespace bitbang;
using namespace binops;

/*
Useful References
Ethernet Header
https://en.wikipedia.org/wiki/Ethernet_frame#Header
https://en.wikipedia.org/wiki/EtherType

Network packet parsing
    http://yuba.stanford.edu/~nickm/papers/ancs48-gibb.pdf
    
Design of EBF
    https://sharkfestus.wireshark.org/sharkfest.11/presentations/McCanne-Sharkfest'11_Keynote_Address.pdf

*/

namespace pcap {
typedef enum {
    PCAP_D_INOUT = 0,
    PCAP_D_IN,
    PCAP_D_OUT
} pcap_direction_t;

// http://www.tcpdump.org/linktypes.html
typedef enum {
    LINKTYPE_NULL = 0,
    LINKTYPE_ETHERNET       = 1,
    LINKTYPE_AX25           = 3,
    LINKTYPE_IEEE802_5      = 6,
    LINKTYPE_ARCNET_BSD     = 7,
    LINKTYPE_SLIP           = 8,
    LINKTYPE_PPP            = 9,
    LINKTYPE_FDDI           = 10,
    LINKTYPE_PPP_HDLC       = 50,
    LINKTYPE_PPP_ETHER      = 51,
    LINKTYPE_ATM_RFC1483    = 100,
    LINKTYPE_RAW            = 101,

    LINKTYPE_LOOP           = 108,
} LINKTYPE;

// Global/File Header
typedef struct pcap_file_header_t {
        uint32_t magic;         // magic number 
        uint16_t version_major;  // major version number 
        uint16_t version_minor;  // minor version number 
        int32_t  thiszone;       // GMT to local correction 
        uint32_t sigfigs;        // accuracy of timestamps 
        uint32_t snaplen;        // max length of captured packets, in octets 
        uint32_t linktype;        // data link type 
} pcap_hdr;

// Record (Packet) Header
typedef struct pcaprec_hdr_s {
        uint32_t ts_sec;         /* timestamp seconds */
        uint32_t ts_usec;        /* timestamp microseconds */
        uint32_t incl_len;       /* number of octets of packet saved in file */
        uint32_t orig_len;       /* actual length of packet */
} pcaprec_hdr_t;

// Ethernet link layer
    //uint8_t Preamble[7];
    //uint8_t Delimeter;
// Ethernet data link layer
// On the wire, this value is big-endian
// So, the field needs to be converted appropriately depending
// on the platform that's doing the reading.
typedef enum {
    IPV4        = 0x0800,
    ARP         = 0x0842,
    WakeOnLAN   = 0x0842,
    AVTP        = 0x22F0,  // Audio Video Transport Protocol
    IETF_TRILL  = 0x22F3,
    SRP         = 0x22EA,   // Stream Reservation Protocol
    DEC_MOP_RC  = 0x6002,    // DEC MOP RC
    DECnet      = 0x6003,        // DECnet Phase IV, DNA Routing
    DEC_LAT     = 0x6004,
    RARP        = 0x8035,

    VLAN        = 0x8100,          // IEEE 802.1Q

    IPV6        = 0x86DD,

    S_Tag       = 0x88A8,         // IEEE 802.1ad
} EtherType;

typedef struct ethernet_hdr_s {
    uint8_t MACDestination[6];  // MAC address is straight bytes
    uint8_t MACSource[6];
    uint16_t TPID;          // Tag Protocol IDentifier

    uint8_t Payload[1500];  // 42 to 1500 octets long
    uint32_t Checksum;      // 32-bit CRC-32
} ethernet_hdr_t;

// Here's how you figure out what you're looking at
// This data structure is not what's on the wire, it is
// meant to be easy to handle in code, so something 
// else has to parse the content and stick it in this format
typedef struct ip_hdr
{
    // byte offset - 0
    uint8_t     Version;    // 4-bit, IPv4 version
    uint8_t     IHL;        // 4-bit, Internet Header Length (in 32-bit words)

    // byte - 1
    uint8_t     DSCP;       // Differentiated Services Codepoint
    uint8_t     ECN;        // Explicit Congestion Notification

    // byte 2 - 3
    uint16_t    TotalLength;    // Entire packet size, including header + data

    // byte 4 - 5
    uint16_t    ID;             // Unique Identifier
    
    // byte 6 - 7
    bool Flags_reserved;           // reserved
    bool DF;                    // Dont fragment
    bool MF;                    // More Fragments
    uint16_t    FragmentOffset; // Fragment Offset

    uint8_t  TimeToLive;           // Time to live
    uint8_t  Protocol;      // Protocol(TCP,UDP etc)
    uint16_t HeaderChecksum;      // IP checksum
    uint32_t SourceIPAddress;       // Source address
    uint32_t DestinationIPAddress;      // Source address
}   IPV4_HDR;


typedef struct udp_hdr
{
 unsigned short source_port;     // Source port no.
 unsigned short dest_port;       // Dest. port no.
 unsigned short udp_length;      // Udp packet length
 unsigned short udp_checksum;    // Udp checksum (optional)
}   UDP_HDR;

typedef struct tcp_header
{
 unsigned short source_port;  // source port
 unsigned short dest_port;    // destination port
 unsigned int   sequence;     // sequence number - 32 bits
 unsigned int   acknowledge;  // acknowledgement number - 32 bits
 
 unsigned char  ns   :1;          //Nonce Sum Flag Added in RFC 3540.
 unsigned char  reserved_part1:3; //according to rfc
 unsigned char  data_offset:4;    //number of dwords in the TCP header.
 
 unsigned char  fin  :1;      //Finish Flag
 unsigned char  syn  :1;      //Synchronise Flag
 unsigned char  rst  :1;      //Reset Flag
 unsigned char  psh  :1;      //Push Flag
 unsigned char  ack  :1;      //Acknowledgement Flag
 unsigned char  urg  :1;      //Urgent Flag
 
 unsigned char  ecn  :1;      //ECN-Echo Flag
 unsigned char  cwr  :1;      //Congestion Window Reduced Flag
 
 unsigned short window;          // window
 unsigned short checksum;        // checksum
 unsigned short urgent_pointer;  // urgent pointer
}   TCP_HDR;

typedef struct icmp_hdr
{
 uint8_t type;          // ICMP Error type
 uint8_t code;          // Type sub code
 uint16_t checksum;
 uint16_t id;
 uint16_t seq;
}   ICMP_HDR;

bool readFileHeader(BinStream &bs, pcap_hdr &hdr)
{

    hdr.magic = bs.readUInt32();
    hdr.version_major = bs.readUInt16();
    hdr.version_minor = bs.readUInt16();
    hdr.thiszone = bs.readInt32();
    hdr.sigfigs = bs.readUInt32();
    hdr.snaplen = bs.readUInt32();
    hdr.linktype = bs.readUInt32();

    return true;
}


bool readRecordHeader(BinStream &bs, pcaprec_hdr_t &hdr)
{
    bs.readBytes((uint8_t *)&hdr, sizeof(hdr));

    return true;
}

// https://en.wikipedia.org/wiki/Ethernet_frame#Header
// Careful to read starting at the packet layer, not the 
// link layer
// Ethernet frames are always displayed from most significant 
// bit to to least significant bit.  In actual transmission, each
// octet is transmitted from least significant bit to most significant
// bit.  
bool readEthernetPacket(BinStream &bs, ethernet_hdr_t &pkt)
{
    bs.readBytes((uint8_t *)&pkt.MACDestination, 6);
    bs.readBytes((uint8_t *)&pkt.MACSource, 6);
    
    // Read a couple of bytes to figure out the tag
    pkt.TPID = binops::bswap16(bs.readUInt16());

    if (pkt.TPID <= 1500) {
        // If the TPID is less than 1500, it indicates
        // a length of the payload.  We load that amount
        // load the checksum, and return 

        bs.readBytes(pkt.Payload, pkt.TPID);
        pkt.Checksum = bs.readUInt32();

        return true;
    }

    // So, we have an EtherType
    // We proceed according to the type
    // 0x0800   - IPV4 packet, most common
    // 0x88DD   - IPV6 packet
    // others


    return true;
}
//
// https://en.wikipedia.org/wiki/IPv4#Header
//
bool readIPV4Header(BinStream &bs, IPV4_HDR &hdr)
{
    // Read an initial 20 bytes, so we can start reading
    // values from there
    uint8_t buff[20];
    bs.readBytes(buff, 20);

    //uint8_t abyte = bs.readOctet();
    //printf("Ver/IHL: 0x%x\n", abyte);
    BinStream rs(buff, 20);

    hdr.Version = bitsValueFromBytes(buff, 4, 4);
    hdr.IHL = bitsValueFromBytes(buff, 0, 4);

    hdr.DSCP = bitsValueFromBytes(buff, 8,6, true);
    hdr.ECN = bitsValueFromBytes(buff, 14,2, true);
    
    rs.seek(2);
    hdr.TotalLength = binops::bswap16(rs.readUInt16()); // bitsValueFromBytes(buff, 16,16, true);

    hdr.ID = bitsValueFromBytes(buff, 32,16, true);
    hdr.Flags_reserved = bitsValueFromBytes(buff, 48,1, true);
    hdr.DF = bitsValueFromBytes(buff, 49,1, true);
    hdr.MF = bitsValueFromBytes(buff, 50,1, true);
    hdr.FragmentOffset = bitsValueFromBytes(buff, 51,13, true);

    hdr.TimeToLive = bitsValueFromBytes(buff, 64,8, true);
    hdr.Protocol = bitsValueFromBytes(buff, 72,8, true);
    hdr.HeaderChecksum = bitsValueFromBytes(buff, 80,16, true);

    hdr.SourceIPAddress = bitsValueFromBytes(buff, 96,32, true);
    hdr.DestinationIPAddress = bitsValueFromBytes(buff, 128,32, true);

    return true;
}

}        // namespace pcap

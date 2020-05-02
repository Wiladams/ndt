#include "pcap.hpp"
#include "mmap.hpp"
#include "binstream.hpp"
#include "bitbang.hpp"
#include "binops.hpp"

using namespace bitbang;
using namespace pcap;

void printFileHeader(const pcap_hdr &hdr)
{
    printf("PCAP FILE\n");
    printf("      Magic: %08x\n", hdr.magic);
    printf("      major: %d\n", hdr.version_major);
    printf("      minor: %d\n", hdr.version_minor);
    printf("   thiszone: %d\n", hdr.thiszone);
    printf("    sigfigs: %d\n", hdr.sigfigs);
    printf("    snaplen: %d\n", hdr.snaplen);
    printf("   linktype: %d\n", hdr.linktype);

}

/*
typedef struct pcaprec_hdr_t {
        uint32_t ts_sec;         // timestamp seconds
        uint32_t ts_usec;        // timestamp microseconds
        uint32_t incl_len;       // number of octets of packet saved in file
        uint32_t orig_len;       // actual length of packet
} pcaprec_hdr;
*/
void printRecordHeader(const pcaprec_hdr_t &hdr)
{
    printf("== RECORD HEADER ==\n");
    printf("  ts_sec: %d\n", hdr.ts_sec);
    printf(" ts_usec: %d\n", hdr.ts_usec);
    printf("incl_len: %d\n", hdr.incl_len);
    printf("orig_len: %d\n", hdr.orig_len);
    printf("---------------\n");
}

void printMAC(const uint8_t *MAC)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x", 
        MAC[0], MAC[1], MAC[2],
        MAC[3], MAC[4], MAC[5]);
}

void printEthernetPacket(const ethernet_hdr_t &pkt)
{
    printf("MAC Source: ");
    printMAC(pkt.MACSource);
    printf("\n");

    printf("MAC Destination: ");
    printMAC(pkt.MACDestination);
    printf("\n");

}

void printIPV4Header(const IPV4_HDR &hdr)
{
    struct in_addr srcAddr;
    srcAddr.S_un.S_addr = hdr.SourceIPAddress;

    printf("-- IPV4 Header --\n");
    printf("  Version: %d\n", hdr.Version);
    printf("   Length: %d\n", hdr.TotalLength);
    printf("      TTL: %d\n", hdr.TimeToLive);
    printf("   Source: %s\n", inet_ntoa(srcAddr) );
}







void test_file()
{
    mmap m = mmap::create(".\\data\\netdump.pcap");
    BinStream bs = BinStream(m.data(), m.size());
    if (!bs.isValid())
        return ;

    // Read the pcap file header
    pcap_hdr hdr;
    readFileHeader(bs, hdr);
    printFileHeader(hdr);

    // Now read all the packets
    uint8_t buff[256*1024];
    while(!bs.isEOF()) {
        // First read a packet header
        pcaprec_hdr_t recHdr;
        readRecordHeader(bs, recHdr);
        printRecordHeader(recHdr);

        // Read the record data into a chunk of memory
        // The headers (IP/TCP/UDP) will be in bigendian order
        // so we need a stream in bigendian
        size_t sentinel = bs.tell();

        ethernet_hdr_t pkt;
        readEthernetPacket(bs, pkt);
        printEthernetPacket(pkt);

        switch (pkt.TPID) {
            case IPV4: {
                IPV4_HDR ipv4pkt;
                readIPV4Header(bs, ipv4pkt);
                printIPV4Header(ipv4pkt);
            }
            break;

            default:
                printf("TPID: 0x%04x\n", pkt.TPID);
            break;
        }
        //IPV4_HDR iphdr;
        //readIPV4Header(bs, iphdr);
        //printIPV4Header(iphdr);

        bs.seek(sentinel+recHdr.incl_len);
    }


}

void main()
{
    test_file();
}
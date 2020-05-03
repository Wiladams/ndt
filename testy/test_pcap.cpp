#include "pcap.hpp"
#include "mmap.hpp"
#include "binstream.hpp"
#include "bitbang.hpp"
#include "binops.hpp"
#include <unordered_map>
#include <string>

using namespace bitbang;
using namespace pcap;

std::unordered_map<int, std::string> IPProtocols;

void preload()
{
    https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
    IPProtocols[6] = std::string("TCP");
    IPProtocols[17] = std::string("UDP");
    IPProtocols[27] = std::string("RDP");
    IPProtocols[36] = std::string("XTP");
    IPProtocols[41] = std::string("IPV6");  // IP V6 Encapsulation
    IPProtocols[113] = std::string("PGM");  // PGM Reliable Transport Protocol

}

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
}

void printMAC(const uint8_t *MAC)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x", 
        MAC[0], MAC[1], MAC[2],
        MAC[3], MAC[4], MAC[5]);
}

void printEthernetPacket(const ethernet_hdr_t &pkt)
{
    printf("-- Ethernet --\n");
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
    struct in_addr dstAddr;
    dstAddr.S_un.S_addr = hdr.DestinationIPAddress;

    printf("-- IPV4 Header --\n");
    printf("    Version: %d\n", hdr.Version);
    printf("Header Length: %d (%d)\n", hdr.IHL*4, hdr.IHL);
    printf(" Total Length: %d\n", hdr.TotalLength);
    printf("          TTL: %d\n", hdr.TimeToLive);
    printf("     Protocol: %s (%d) \n", IPProtocols[hdr.Protocol].c_str(), hdr.Protocol);
    printf("       Source: %s\n", inet_ntoa(srcAddr) );
    printf("  Destination: %s\n", inet_ntoa(dstAddr) );
}







void test_file(const char *filename)
{
    mmap m = mmap::create(filename);
    BinStream bs = BinStream(m.data(), m.size());
    if (!bs.isValid())
        return ;

    // Read the pcap file header
    pcap_hdr hdr;
    readFileHeader(bs, hdr);
    printFileHeader(hdr);

    // Now read all the packets
    uint8_t buff[256*1024];
    int recCount = 0;
    while(!bs.isEOF() && (recCount < 1000)) {
        recCount++;

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
                printf("-- Unknown --\n");
                printf("TPID: 0x%04x\n", pkt.TPID);
            break;
        }
        //IPV4_HDR iphdr;
        //readIPV4Header(bs, iphdr);
        //printIPV4Header(iphdr);

        bs.seek(sentinel+recHdr.incl_len);
    }


}

void test_net_nibbles()
{
        uint8_t bytes[20]={0x45};
    uint8_t version = bitsValueFromNetBytes(bytes, 0, 4);
    uint8_t length = bitsValueFromNetBytes(bytes, 4, 4);

    printf("Version: %d\n", version);
    printf("length: %d\n", length);
}

void test_netbits()
{
    uint8_t bytes[20]={0x00, 0x45};

    uint16_t length = bitsValueFromNetBytes(bytes, 0, 16);
    printf("length: %d 0x%2x\n", length, bswap16(length));
}

int bytesNeeded(int bitscount)
{
    return (bitscount * .125 == floor(bitscount *.125)) ? (bitscount *.125) : ((int)(bitscount * .125))+1;
}

void test_bits()
{
    for (int i=0; i<=17; i++) 
        printf("Needed: [%d] => %d\n", i, bytesNeeded(i));
}

int main(int argc, char **argv)
{
    //if (argc < 2) {
    //    printf("Usage:  test_pcap <filename.pcap>\n");
    //    return -1;
    //}

    preload();

    //test_file(argv[1]);
    //test_netbits();
    test_bits();

    return 0;
}
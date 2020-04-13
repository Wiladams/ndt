#pragma once
#pragma comment(lib, "ws2_32.lib")

/*
    Definitions for Windows socket programming

    These are the essentials required to to "Berkeley" style
    network programming.
*/



#include "w32_types.hpp"

#if !defined(MAKEWORD)
#define MAKEWORD(low,high) \
        ((WORD)(((BYTE)(low)) | ((WORD)((BYTE)(high))) << 8))
#endif
//MAKEWORD(2,2)

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef uint64_t u_int64;

typedef UINT_PTR        SOCKET;

typedef uint16_t    ADDRESS_FAMILY;
typedef unsigned int GROUP;

#ifdef __cplusplus
extern "C" {
#endif
typedef struct sockaddr {
    ADDRESS_FAMILY  sa_family;
    uint8_t     sa_data[14];
} SOCKADDR, *PSOCKADDR, *LPSOCKADDR;

typedef struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char* ai_canonname;
    struct sockaddr * ai_addr;
    struct addrinfo* ai_next;
} ADDRINFOA,  *PADDRINFOA;


typedef struct hostent {
    char * h_name;
    char ** h_aliases;
    short h_addrtype;
    short h_length;
    char ** h_addr_list;
} HOSTENT,  *PHOSTENT,  *LPHOSTENT;

typedef struct in_addr {
    union {
        struct {
            uint8_t s_b1,s_b2,s_b3,s_b4;
            } S_un_b;
        struct {
            uint16_t s_w1,s_w2;
        } S_un_w;
        uint32_t S_addr;
    };
} IN_ADDR, *PIN_ADDR, *LPIN_ADDR;

typedef struct in6_addr {
    union {
        uint8_t       Byte[16];
        uint16_t      Word[8];
    } u;
} IN6_ADDR;

#ifndef FD_SETSIZE
#define FD_SETSIZE      64
#endif /* FD_SETSIZE */

typedef struct fd_set {
    u_int   fd_count;               /* how many are SET? */
    SOCKET  fd_array[FD_SETSIZE];   /* an array of SOCKETs */
} fd_set;

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};

/*
    Windows Networking API specifics
*/
static const int    WSADESCRIPTION_LEN =     256;
static const int    WSASYS_STATUS_LEN  =     128;

#ifdef _WIN64
typedef struct WSAData {
    WORD                wVersion;
    WORD                wHighVersion;
    unsigned short      iMaxSockets;
    unsigned short      iMaxUdpDg;
    char *              lpVendorInfo;
    char                szDescription[WSADESCRIPTION_LEN+1];
    char                szSystemStatus[WSASYS_STATUS_LEN+1];
} WSADATA, *LPWSADATA;
#else
typedef struct WSAData {
        WORD                wVersion;
        WORD                wHighVersion;
        char                szDescription[WSADESCRIPTION_LEN+1];
        char                szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short      iMaxSockets;
        unsigned short      iMaxUdpDg;
        char *				lpVendorInfo;
} WSADATA, * LPWSADATA;
#endif

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

/*
    Networking constants
*/
// Address Family
#define AF_UNSPEC       0               // unspecified
#define AF_UNIX         1               // local to host (pipes, portals)
#define AF_INET         2               // internetwork: UDP, TCP, etc.
#define AF_IMPLINK      3               // arpanet imp addresses
#define AF_PUP          4               // pup protocols: e.g. BSP
#define AF_CHAOS        5               // mit CHAOS protocols
#define AF_NS           6               // XEROX NS protocols
#define AF_IPX          AF_NS           // IPX protocols: IPX, SPX, etc.
#define AF_ISO          7               // ISO protocols
#define AF_OSI          AF_ISO          // OSI is ISO
#define AF_ECMA         8               // european computer manufacturers
#define AF_DATAKIT      9               // datakit protocols
#define AF_CCITT        10              // CCITT protocols, X.25 etc
#define AF_SNA          11              // IBM SNA
#define AF_DECnet       12              // DECnet
#define AF_DLI          13              // Direct data link interface
#define AF_LAT          14              // LAT
#define AF_HYLINK       15              // NSC Hyperchannel
#define AF_APPLETALK    16              // AppleTalk
#define AF_NETBIOS      17              // NetBios-style addresses
#define AF_VOICEVIEW    18              // VoiceView
#define AF_FIREFOX      19              // Protocols from Firefox
#define AF_UNKNOWN1     20              // Somebody is using this!
#define AF_BAN          21              // Banyan
#define AF_ATM          22              // Native ATM Services
#define AF_INET6        23              // Internetwork Version 6
#define AF_CLUSTER      24              // Microsoft Wolfpack
#define AF_12844        25              // IEEE 1284.4 WG AF
#define AF_IRDA         26              // IrDA
#define AF_NETDES       28              // Network Designers OSI & gateway


//
// Socket types.
//

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_RDM        4
#define SOCK_SEQPACKET  5

typedef enum {
    IPPROTO_HOPOPTS       = 0,  // IPv6 Hop-by-Hop options
    IPPROTO_ICMP          = 1,
    IPPROTO_IGMP          = 2,
    IPPROTO_GGP           = 3,
    IPPROTO_IPV4          = 4,
    IPPROTO_ST            = 5,
    IPPROTO_TCP           = 6,
    IPPROTO_CBT           = 7,
    IPPROTO_EGP           = 8,
    IPPROTO_IGP           = 9,
    IPPROTO_PUP           = 12,
    IPPROTO_UDP           = 17,
    IPPROTO_IDP           = 22,
    IPPROTO_RDP           = 27,
    IPPROTO_IPV6          = 41, // IPv6 header
    IPPROTO_ROUTING       = 43, // IPv6 Routing header
    IPPROTO_FRAGMENT      = 44, // IPv6 fragmentation header
    IPPROTO_ESP           = 50, // encapsulating security payload
    IPPROTO_AH            = 51, // authentication header
    IPPROTO_ICMPV6        = 58, // ICMPv6
    IPPROTO_NONE          = 59, // IPv6 no next header
    IPPROTO_DSTOPTS       = 60, // IPv6 Destination options
    IPPROTO_ND            = 77,
    IPPROTO_ICLFXBM       = 78,
    IPPROTO_PIM           = 103,
    IPPROTO_PGM           = 113,
    IPPROTO_L2TP          = 115,
    IPPROTO_SCTP          = 132,
    IPPROTO_RAW           = 255,

    IPPROTO_MAX           = 256,
} IPPROTO, *PIPROTO;

//
//  Flags used in "hints" argument to getaddrinfo()
//      - AI_ADDRCONFIG is supported starting with Vista
//      - default is AI_ADDRCONFIG ON whether the flag is set or not
//        because the performance penalty in not having ADDRCONFIG in
//        the multi-protocol stack environment is severe;
//        this defaulting may be disabled by specifying the AI_ALL flag,
//        in that case AI_ADDRCONFIG must be EXPLICITLY specified to
//        enable ADDRCONFIG behavior
//

#define AI_PASSIVE                  0x00000001  // Socket address will be used in bind() call
#define AI_CANONNAME                0x00000002  // Return canonical name in first ai_canonname
#define AI_NUMERICHOST              0x00000004  // Nodename must be a numeric address string
#define AI_NUMERICSERV              0x00000008  // Servicename must be a numeric port number
#define AI_DNS_ONLY                 0x00000010  // Restrict queries to unicast DNS only (no LLMNR, netbios, etc.)

#define AI_ALL                      0x00000100  // Query both IP6 and IP4 with AI_V4MAPPED
#define AI_ADDRCONFIG               0x00000400  // Resolution only if global address configured
#define AI_V4MAPPED                 0x00000800  // On v6 failure, query v4 and convert to V4MAPPED format

#define AI_NON_AUTHORITATIVE        0x00004000  // LUP_NON_AUTHORITATIVE
#define AI_SECURE                   0x00008000  // LUP_SECURE
#define AI_RETURN_PREFERRED_NAMES   0x00010000  // LUP_RETURN_PREFERRED_NAMES

#define AI_FQDN                     0x00020000  // Return the FQDN in ai_canonname
#define AI_FILESERVER               0x00040000  // Resolving fileserver name resolution
#define AI_DISABLE_IDN_ENCODING     0x00080000  // Disable Internationalized Domain Names handling
#define AI_EXTENDED                 0x80000000      // Indicates this is extended ADDRINFOEX(2/..) struct
#define AI_RESOLUTION_HANDLE        0x40000000  // Request resolution handle


#define INADDR_ANY              (ULONG)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (ULONG)0xffffffff
#define INADDR_NONE             0xffffffff

#define MSG_OOB         0x1
#define MSG_PEEK        0x2
#define MSG_DONTROUTE   0x4
#define MSG_WAITALL     0x8
#define MSG_INTERRUPT   0x10
#define MSG_MAXIOVLEN   16
#define MSG_PUSH_IMMEDIATE 0x20
#define MSG_PARTIAL     0x8000
typedef struct sockaddr_in {
    ADDRESS_FAMILY sin_family;

    uint16_t sin_port;
    IN_ADDR sin_addr;
    char sin_zero[8];
} SOCKADDR_IN, *PSOCKADDR_IN;

typedef struct sockaddr_in6 {
        int16_t             sin6_family;
        uint16_t            sin6_port;
        uint32_t            sin6_flowinfo;
        struct  in6_addr    sin6_addr;
        uint32_t            sin6_scope_id;
} sockaddr_in6;

/*
    Berkeley Socket address
*/
SOCKET accept(SOCKET s,struct sockaddr* addr,int* addrlen);

int bind(SOCKET s, const struct sockaddr* name, int namelen);

int closesocket(SOCKET s);

int connect(SOCKET s, const struct sockaddr * name, int namelen);

void freeaddrinfo(PADDRINFOA pAddrInfo);

int getaddrinfo(const char* nodename,const char* servname,const struct addrinfo* hints,PADDRINFOA * res);

// The gethostby** calls are deprecated, and should not be used
// use the getaddrinfo() call instead
//struct hostent* gethostbyaddr(const char* addr,int len,int type);
//struct hostent* gethostbyname(const char* name);

// Get our own host's name
int gethostname(char* name, int namelen);

int getsockname(SOCKET s, struct sockaddr* name, int* namelen);

int getsockopt(SOCKET s, int level, int optname, char* optval,int* optlen);

// host to network long, a byte swapping routine
u_long  htonl(u_long hostlong);

// host to network short, a byte swapping routine
u_short htons(u_short hostshort);

unsigned long inet_addr(const char* cp);
char* inet_ntoa(struct   in_addr in);
int inet_pton(int Family, const char * szAddrString, const void * pAddrBuf);
const char * inet_ntop(int Family, const void *pAddr, intptr_t strptr, size_t len);

int ioctlsocket(SOCKET s, long cmd, u_long* argp);

int listen(SOCKET s, int backlog);

// Network to host short, a byte swapping routine
u_short ntohs(u_short netshort);

// Network to host long, a byte swapping routine
u_long  ntohl(u_long netlong);

// receive data from a bound socket
int recv(SOCKET s, char* buf, int len, int flags);


int recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen);

int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout);

// send data on a connected socket
int send(SOCKET s, const char* buf, int len, int flags);

int sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);

int setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen);

int shutdown(SOCKET s, int how);

// creation of a socket
SOCKET socket(int af, int type, int protocol);


/*
    Microsoft Windows networking API extensions
*/
#define MAX_PROTOCOL_CHAIN 7

#define BASE_PROTOCOL      1
#define LAYERED_PROTOCOL   0

#define WSAPROTOCOL_LEN  255

typedef struct _WSAPROTOCOLCHAIN {
    int ChainLen;                                 /* the length of the chain,     */
                                                  /* length = 0 means layered protocol, */
                                                  /* length = 1 means base protocol, */
                                                  /* length > 1 means protocol chain */
    DWORD ChainEntries[MAX_PROTOCOL_CHAIN];       /* a list of dwCatalogEntryIds */
} WSAPROTOCOLCHAIN, * LPWSAPROTOCOLCHAIN;


typedef struct _WSAPROTOCOL_INFOA {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    CHAR   szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOA, * LPWSAPROTOCOL_INFOA;

/*
 * WinSock 2 extension -- manifest constants for WSASocket()
 */
#define WSA_FLAG_OVERLAPPED           0x01
#define WSA_FLAG_MULTIPOINT_C_ROOT    0x02
#define WSA_FLAG_MULTIPOINT_C_LEAF    0x04
#define WSA_FLAG_MULTIPOINT_D_ROOT    0x08
#define WSA_FLAG_MULTIPOINT_D_LEAF    0x10
#define WSA_FLAG_ACCESS_SYSTEM_SECURITY 0x40
#define WSA_FLAG_NO_HANDLE_INHERIT    0x80
#define WSA_FLAG_REGISTERED_IO       0x100


int WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData);
int WSACleanup(void);
int WSAGetLastError(void);
void WSASetLastError(int iError);

BOOL WSAIsBlocking(void);

int WSAAddressToStringA(
    LPSOCKADDR lpsaAddress,
    DWORD               dwAddressLength,
    void * lpProtocolInfo,
    LPSTR lpszAddressString,
    LPDWORD             lpdwAddressStringLength
    );
/*
int
WSAIoctl(
     SOCKET s,
     DWORD dwIoControlCode,
    LPVOID lpvInBuffer,
     DWORD cbInBuffer,
    LPVOID lpvOutBuffer,
     DWORD cbOutBuffer,
     LPDWORD lpcbBytesReturned,
     LPWSAOVERLAPPED lpOverlapped,
     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );
*/
SOCKET WSASocketA(int af, int type, int protocol,
    LPWSAPROTOCOL_INFOA lpProtocolInfo, 
    GROUP g,DWORD dwFlags);


#ifdef __cplusplus
}
#endif
/*
    In this example, we want to connect to various 
    servers on port 80, and see what kind of response they
    return when we ask for the '/' resource.

    We connect to all the IP addresses reported by the server
    and not just the first one on the list.
*/

#include <cstdio>
#include <cstring>

#include "p5.hpp"
#include "TcpClient.hpp"

const char * sites[] = {
    "Microsoft.com",
    "facebook.com",
    "google.com",
    "amazon.com",
    "netflix.com",
    "msn.com",
    "adafruit.com",
    "news.ycombinator.com"
};

/*
    Connect to all the host IP addresses we get
    and send a simple HTTP GET to see what comes back
*/
bool pingHttp(const char * hostname)
{
    static const int recvSize = 1024 * 64; // 64k 
    char response[recvSize+1];

    char request[512];
    sprintf(request, "GET / HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "\r\n", hostname);

    char * portname = "80";

    IPHost *host = IPHost::create(hostname, portname);

    if (host == nullptr) {
        printf("Could not find host: %s\n", hostname);
        return false;
    }

    // Go through each of the host addresses
    // trying to connect to each one in turn
    printf("\n== HOST BEGIN (%s)==\n", hostname);
    for (int idx = 0; idx < host->numberOfAddresses(); idx++)
    {
        // Create a socket for the given address
        IPAddress *addr = host->getAddress(idx);
        TcpClient s(addr);

        // convert address to string form so we can print it out
        char addressBuff[256]={0};
        int addressBuffLen = 255;
        int strLen = addr->toString(addressBuff, addressBuffLen);
        if (strLen > 0) {
            addressBuff[strLen] = 0;
        }

        if (!s.isValid()) {
            printf("Invalid TcpClient: %d\n", s.getLastError());

            // just move onto the next one
            continue;
        }
        
        //s.connect();

        int retCode = s.send(request, strlen(request), 0);
        
        if (retCode == SOCKET_ERROR) {
            printf("SOCKET SEND ERROR: %d\n", s.getLastError());
            continue;
        }

        // Get a response back from the server
        retCode = s.receive(response, recvSize);
        //printf("recv: %d\n", retCode);
        if (retCode == SOCKET_ERROR) {
            printf("RECEIVE ERROR: %d\n", s.getLastError());
            continue;
        }

        // close the socket
        s.close();

        // Null terminate so we can use printf
        response[retCode] = 0;

        printf("\n== RESPONSE BEGIN [%s]==\n", addressBuff);
        printf("%s", response);
        printf("\n== RESPONSE END ==\n");
    }
    printf("\n== HOST END (%s)==\n", hostname);
    
    return true;
}



void setup()
{
    int nSites = sizeof(sites)/sizeof(char *);

    printf("SCAN BEGIN[%d]\n", nSites);

    for (int idx = 0; idx < nSites; idx++)
    {
        bool success = pingHttp(sites[idx]);
        if (!success) {
            printf("FAILED: %s\n", sites[idx]);
            continue;
        }
    }
    printf("\n== SCAN END ==\n");

    halt();
}


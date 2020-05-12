#pragma once

#include "Network.hpp"

class TcpClient {
private:
    IPSocket fSocket;
    IPHost  * fHost;
    IPAddress *fAddress;
    bool    fIsValid;
    int fLastError;

public:
    TcpClient(IPAddress *address)
        :fSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
        , fAddress(address)
        , fLastError(0)
    {
        if (!fSocket.isValid()) {
            fLastError = fSocket.getLastError();
            return ;
        }

        // Try to connect, if failure
        // return immediately
        if (!connect()) {
            return;
        }
        
        fIsValid = true;
    }

    TcpClient(const char *hostname, const char * portname, bool hostNumeric=false)
        : fSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
        , fIsValid(false)
    {
        // check if the socket was created
        if (!fSocket.isValid()) {
            fLastError = fSocket.getLastError();
            return ;
        }

        // Get address to host
        fHost = IPHost::create(hostname, portname, AF_INET, SOCK_STREAM, hostNumeric);
        if (fHost == nullptr)
        {
            printf("could not find host: %s\n", hostname);
            return ;
        }

        fAddress = fHost->getAddress();
        if (fAddress == nullptr) {
            return ;
        }
        
        // Try to connect, if failure
        // return immediately
        if (!connect()) {
            return;
        }

        fIsValid = true;
    }

    bool isValid() const {return fIsValid;}
    int getLastError() const {return fLastError;}

    bool close()
    {
        if (!fSocket.close()) {
            fLastError = fSocket.getLastError();
            return false;
        } 
        return true;
    }

    bool connect()
    {
        int retCode = ::connect(fSocket.fSocket, fAddress->fAddress, fAddress->fAddressLength);

        if (retCode != 0) {
            fLastError = WSAGetLastError();

            return false;
        }

        return true;
    }

    int send(const char *buff, const int buffLen, int flags = 0)
    {
        int result = fSocket.send(buff, buffLen, flags);
        if (result == SOCKET_ERROR) {
            fLastError = WSAGetLastError();
        }

        return result;
    }

    int receive(char *buff, int buffLen, int flags=0)
    {
        return fSocket.receive(buff, buffLen, flags);
    }
};


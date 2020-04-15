#pragma once

#include "Network.hpp"

class TcpServer {
private:
    IPSocket fSocket;   // socket used to listen
    
    struct sockaddr_in fServerAddress;
    int fServerAddressLen;
    bool fIsValid;
    int fLastError;

public:
    TcpServer(int porto, const char * interface = "localhost")
        :fSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
    {
        if (!fSocket.isValid()) {
            // didn't get a valid socket, so we're
            // not valid
            fLastError = WSAGetLastError();

            return ;
        }

        // Create a server address we can listen on
        // IPV4 wildcard address, so we accept from any interface
        fServerAddressLen = sizeof(fServerAddress);
        memset(&fServerAddress, 0, fServerAddressLen);
        fServerAddress.sin_family = AF_INET;
        fServerAddress.sin_addr.S_addr = htonl(INADDR_ANY);  // we don't care about address
        fServerAddress.sin_port = htons(porto);

        if (!bind()) {
            return;
        }

        // listen
        if (!makePassive())
        {
            return ;
        }

        fIsValid = true;
    }

    virtual ~TcpServer() {
        //delete fHost;
    }

    bool isValid() const {return fIsValid;}
    int getLastError() const {return fLastError;}

    IPSocket accept()
    {
        IPSocket s = fSocket.accept();
        printf("TcpServer.accept(): %Id %d\n", s.fSocket, s.getLastError());
        if (!s.isValid()) {
            fLastError = fSocket.getLastError();
        }

        return s;
    }

    bool bind()
    {
        int result = fSocket.bindTo((struct sockaddr *)&fServerAddress, fServerAddressLen);
        if (result != 0) {
            fLastError = WSAGetLastError();
            return false;
        }

        fLastError = 0;
        return true;
    }

    // Typically when you're creating a server, you will 
    // need to tell the socket to start listening for
    // connections.  This is as opposed to be active, like
    // a client connection.
    bool makePassive(const int backlog=5)
    {
        bool success = fSocket.listen(backlog);
        if (!success) {
            fLastError = fSocket.getLastError();
        } else
        {
            fLastError = 0;
        }
        
        return success;
    }

    int receive(char *buff, int buffLen)
    {
        int result = fSocket.receive(buff, buffLen, 0);
        return result;
    }

    int send(const char *buff, int buffLen)
    {
        int result = fSocket.send(buff, buffLen);
        return result;
    }
};

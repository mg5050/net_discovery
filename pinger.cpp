/*
Pinger:

    Desc:       Network discovery tool for Linux
    Depends:    C++11, Linux ping utility
    Author:     Mike Gonzalez (mgonz50@rutgers.edu)

File info:
    pinger.cpp - Network discovery and process threading

Usage:
    pinger.exe 192.168.1.1/24

Compile:   
    g++ -pthread -std=c++11 pinger.cpp -o pinger.exe core/net.cpp
*/

#include <iostream>
#include <map>
#include <mutex>
#include <stdlib.h>
#include <thread>
#include "core/net.h"

#define ERROR_MSG ("USAGE: e.g. - pinger.exe 192.168.1.1/24")

map<addr, bool> pings; // not thread safe so we use mutex
mutex ping_mutex; // mutex for 'pings' map

bool sys_ping(addr dest)
{
    string ip = dest.to_str();
    string cmd = "ping -W 1 -c 1 "+ip+" | grep -q \"from "+ip+"\"";
    return system(cmd.c_str()) == 0;
}

void scan_Worker(network net, bool increment = false)
{
    int count = 1;
    if(increment) // work upwards
    {
        addr next = net.increment_naddr(count);
        while(next != net.get_baddr() && pings.find(next) == pings.end())
        {
            if(sys_ping(next))
            {
                ping_mutex.lock();
                pings[next] = increment;
                ping_mutex.unlock();
            }
            count++;
            next = net.increment_naddr(count);
        }
        return;
    }

    addr prev = net.decrement_baddr(count);
    // workers haven't met in the middle and next isn't broadcast
    while(prev != net.get_naddr() && pings.find(prev) == pings.end())
    {
        if(sys_ping(prev))
        {
            ping_mutex.lock();
            pings[prev] = increment;
            ping_mutex.unlock();
        }
        count++;
        prev = net.decrement_baddr(count);
    }
    return;
}

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        cout << ERROR_MSG << endl;
        return 0;
    }

    network net;
    if(!net.load_cidr_str(argv[1])) 
    {
        cout << ERROR_MSG << endl;
        return 0;
    }

    cout << "\n\t...SCANNING..." << endl;

    thread scan_up(scan_Worker, net, true);
    thread scan_down(scan_Worker, net, false);

    scan_up.join();
    scan_down.join();

    cout << "\nHosts detected:\n" << endl;

    int count = 0;
    for(map<addr, bool>::const_iterator it = pings.begin(); it != pings.end(); it++)
    {
        cout << '\t' << it->first.to_str() << endl;
        count++;
    }
    cout << endl << "\t= " << count << '/' << net.get_hosts() << " total" << endl << endl;

    return 0;
}
# net_scan
Multi-threaded network discovery tool using Linux ping utility to scan subnets

Pinger:

    Desc:       Network discovery tool for Linux
    Depends:    C++11, Linux ping utility
    Author:     Mike Gonzalez (mgonz50@rutgers.edu)

Usage:

    pinger.exe 192.168.1.1/24

Compile:   

    g++ -pthread -std=c++11 pinger.cpp -o pinger.exe core/net.cpp

Setup HOW-TO:
    Requirements:
        GCC C++11 compiler, Linux ping utility

    Steps:
        Extract:
            Extract archive to your working directory.
        Compile:
            Compile with command listed above.
        Run:
            ./pinger.exe (ip address)/(mask)
            e.g. - pinger.exe 192.168.1.1/24

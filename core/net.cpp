/*
Pinger:

    Desc:       Network discovery tool for Linux
    Depends:    C++11, Linux ping utility
    Author:     Mike Gonzalez (mgonz50@rutgers.edu)

File info:
    net.cpp - Network address manipulation functions
*/

#include <cmath>
#include <sstream>
#include "net.h"

addr    addr_bit_and(addr a, addr b);
addr    addr_bit_or(addr a, addr b);
addr    CIDR_mask_invert(addr cidr);
addr    CIDR_to_mask(int cidr);
int     exp_b2(int n);
octet   str_to_octet(string str);

int exp_b2(int n) // exponent base 2
{
    return 1 << n;
}

octet str_to_octet(string str) // string to octet
{
    stringstream sstr;
    sstr << str;
    int val;
    sstr >> val;
    return (octet)val;
}

addr addr_bit_and(addr a, addr b) // binary AND two addresses
{
    addr result;
    for(int i = 0; i < IP_LEN_OCTETS; i++)
    {
        result.add_octet(a.get_octet(i) & b.get_octet(i));
    }
    return result;
}

addr addr_bit_or(addr a, addr b) // binary OR two addresses
{
    addr result;
    for(int i = 0; i < IP_LEN_OCTETS; i++)
    {
        result.add_octet(a.get_octet(i) | b.get_octet(i));
    }
    return result;
}

addr CIDR_to_mask(int cidr) // expand CIDR mask notation to subnet mask
{
    addr result;
    int bytes = floor(cidr / BYTE);

    for(int i = 0; i < bytes; i++)
    {
        result.add_octet(OCTET_VAL_MAX);
    }

    int rem = cidr % BYTE;

    if(rem != 0)
    {
        result.add_octet(OCTET_VAL_MAX << (BYTE - rem));
        for(int i = bytes + 1; i < IP_LEN_OCTETS; i++) result.add_octet(0); // pad remaining bits
    }
    else for(int i = bytes; i < IP_LEN_OCTETS; i++) result.add_octet(0); // pad remaining bits

    return result;
}

addr CIDR_mask_invert(addr cidr) // invert subnet mask to determine host portion
{
    addr result;
    for(int i = 0; i < IP_LEN_OCTETS; i++)
    {
        result.add_octet(~cidr.get_octet(i)); // bit invert
    }
    return result;
}
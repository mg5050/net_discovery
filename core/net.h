/*
Pinger:

    Desc:       Network discovery tool for Linux
    Depends:    C++11, Linux ping utility
    Author:     Mike Gonzalez (mgonz50@rutgers.edu)

File info:
    net.h - Network address class representations
*/

#ifndef __NET__INCL__

#include <iostream>
#include <vector>

#define __NET__INCL__
#define BYTE            (8)                 // bits in a byte
#define CIDR_DELIM      ('/')               // delimiter for mask bits
#define CIDR_LEN_MIN    (IP_LEN_MIN + 2)    // minimum characters in a CIDR string
#define IP_LEN_BITS     (32)                // bits in an IP
#define IP_LEN_MIN      (7)                 // minimum characters in an IP
#define IP_LEN_OCTETS   (4)                 // octets in an IP
#define OCTET_LEN_MAX   (3)                 // max chars in an octet
#define OCTET_VAL_MAX   (255)               // max value of an octet

typedef unsigned char octet;

class   addr;
class   network;

using   namespace std;

extern  addr    addr_bit_and(addr a, addr b);
extern  addr    addr_bit_or(addr a, addr b);
extern  addr    CIDR_mask_invert(addr cidr);
extern  addr    CIDR_to_mask(int cidr);
extern  int     exp_b2(int n);
extern  octet   str_to_octet(string str);

class addr // ip address class
{
private:
    vector<octet> octs;
public:
    addr() {}
    bool load_ip_str(string ip)
    {
        int i = 0, start = 0, count = 0, len = ip.length();
        if(len < IP_LEN_MIN) return false; // 7 is shortest possible len
        while(i <= len)
        {
            if(ip[i] == '.' || i == len)
            {
                string s = ip.substr(start, i - start);
                int sl = s.length();
                if(sl > OCTET_LEN_MAX || sl < 1) return false; // octet is too large
                octs.push_back(str_to_octet(s));
                start = i + 1;
                count++;
            }
            i++;
        }
        if(count != IP_LEN_OCTETS) return false; // 4 octets only
        return true;
    }
    string to_str() const
    {
        string str;
        for(int i = 0; i < octs.size(); i++)
        {
            str += to_string(octs[i]);
            str += '.';
        }
        str.erase(str.length() - 1, string::npos);
        return str;
    }
    void add_octet(octet val)
    {
        octs.push_back(val);
    }
    octet get_octet(int i)
    {
        return octs.at(i);
    }
    void increment() // increment ip address by 1
    {
        for(int i = IP_LEN_OCTETS - 1; i >= 0; i--)
        {
            if(octs[i] < OCTET_VAL_MAX) // add to right most octet below 255
            {
                octs[i]++;
                return;
            }
            else octs[i] = 0;
        }
    }
    void decrement() // decrement by 1
    {
        for(int i = IP_LEN_OCTETS - 1; i >= 0; i--)
        {
            if(octs[i] > 0) // add to right most octet below 255
            {
                octs[i]--;
                return;
            }
            else octs[i] = OCTET_VAL_MAX;
        }
    }
    bool operator<(const addr & b) const
    {
        for(int i = 0; i < IP_LEN_OCTETS; i++)
        {
            if(octs[i] > b.octs[i]) return false;
            if(octs[i] < b.octs[i]) return true;
        }
        return false;
    }
    bool operator>(const addr & b) const
    {
        for(int i = 0; i < IP_LEN_OCTETS; i++)
        {
            if(octs[i] > b.octs[i]) return true;
            if(octs[i] < b.octs[i]) return false;
        }
        return false;
    }
    bool operator!=(const addr & b) const
    {
        int count = 0;
        for(int i = 0; i < IP_LEN_OCTETS; i++) if(octs[i] == b.octs[i]) count++;
        return (count != IP_LEN_OCTETS);
    }
};

class network // ip network class
{
private:
    addr naddr; // network address
    addr baddr; // broadcast address
    addr mask; // subnet mask
    octet maskbits; // CIDR mask notation
    int hostcount; // # of possible hosts on this subnet
public:
    network()
    {
        maskbits = 0;
        hostcount = 0;
    }
    bool load_cidr_str(string cidr)
    {
        string ip;
        int dpos = cidr.find(CIDR_DELIM), len = cidr.length();
        if(dpos == string::npos || len < CIDR_LEN_MIN) return false; // bad string
        ip = cidr.substr(0, dpos);

        maskbits = str_to_octet(cidr.substr(dpos + 1, len));
        if(maskbits >= IP_LEN_BITS - 1 || maskbits < 1) return false; // maskbits too large

        mask = CIDR_to_mask((int)maskbits);

        addr raw_addr = addr(); // raw address created
        if(!raw_addr.load_ip_str(ip)) return false;

        naddr = addr_bit_and(raw_addr, mask);
        baddr = addr_bit_or(naddr, CIDR_mask_invert(mask));

        hostcount = exp_b2(IP_LEN_BITS - maskbits) - 2; // number of possibles hosts

        cout << endl;
        cout << "Network: " << naddr.to_str() << endl;
        cout << "Broadcast: " << baddr.to_str() << endl;
        cout << "Subnet Mask: " << mask.to_str() << endl;
        cout << "Possible hosts: " << hostcount << endl;

        return true;
    }
    addr get_naddr()
    {
        return naddr;
    }
    addr get_baddr()
    {
        return baddr;
    }
    int get_hosts()
    {
        return hostcount;
    }
    addr increment_naddr(int n) // return naddr + n
    {
        addr result = naddr;
        for(int i = 0; i < n && result < baddr; i++)
        {
            result.increment();
        }
        return result;
    }
    addr decrement_baddr(int n) // return baddr - n
    {
        addr result = baddr;
        for(int i = 0; i < n && result > naddr; i++)
        {
            result.decrement();
        }
        return result;
    }
};

#endif
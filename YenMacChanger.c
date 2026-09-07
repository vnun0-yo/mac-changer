/*
============================================================
MAC CHANGER TOOL // C // LINUX // CLI // NO RANDOM
============================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <getopt.h>

#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define RESET   "\033[0m"

void print_success(const char *msg) {
    printf(GREEN "[+] %s\n" RESET, msg);
}

void print_error(const char *msg) {
    fprintf(stderr, RED "[-] %s\n" RESET, msg);
}

void print_info(const char *msg) {
    printf(CYAN "[*] %s\n" RESET, msg);
}

char* get_current_mac(const char *iface) {
    static char mac[18];
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return NULL;
    
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        close(sock);
        return NULL;
    }
    
    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
             (unsigned char)ifr.ifr_hwaddr.sa_data[0],
             (unsigned char)ifr.ifr_hwaddr.sa_data[1],
             (unsigned char)ifr.ifr_hwaddr.sa_data[2],
             (unsigned char)ifr.ifr_hwaddr.sa_data[3],
             (unsigned char)ifr.ifr_hwaddr.sa_data[4],
             (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
    
    close(sock);
    return mac;
}

int set_mac(const char *iface, const char *mac) {
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        print_error("Failed to create socket");
        return -1;
    }
    
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ip link set %s down", iface);
    system(cmd);
    
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    
    unsigned char mac_bytes[6];
    sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
           (unsigned int*)&mac_bytes[0],
           (unsigned int*)&mac_bytes[1],
           (unsigned int*)&mac_bytes[2],
           (unsigned int*)&mac_bytes[3],
           (unsigned int*)&mac_bytes[4],
           (unsigned int*)&mac_bytes[5]);
    
    memcpy(ifr.ifr_hwaddr.sa_data, mac_bytes, 6);
    
    if (ioctl(sock, SIOCSIFHWADDR, &ifr) < 0) {
        print_error("Failed to set MAC address");
        close(sock);
        return -1;
    }
    
    close(sock);
    
    snprintf(cmd, sizeof(cmd), "ip link set %s up", iface);
    system(cmd);
    
    return 0;
}

void show_help() {
    printf(GREEN "\n========================================\n" RESET);
    printf(GREEN "  MAC CHANGER TOOL\n" RESET);
    printf(GREEN "========================================\n\n" RESET);
    
    printf("Usage: mac_changer [options]\n\n");
    printf("Options:\n");
    printf("  -i <interface>    Network interface (e.g., eth0, wlan0)\n");
    printf("  -m <MAC>          New MAC address (e.g., 00:11:22:33:44:55)\n");
    printf("  -h                Show this help message\n\n");
    
    printf("Examples:\n");
    printf("  mac_changer -i eth0 -m 00:11:22:33:44:55\n");
    printf("  mac_changer -h\n\n");
}

int main(int argc, char *argv[]) {
    if (geteuid() != 0) {
        print_error("This tool must be run as root");
        return 1;
    }
    
    char iface[32] = {0};
    char new_mac[18] = {0};
    int opt;
    
    while ((opt = getopt(argc, argv, "i:m:h")) != -1) {
        switch (opt) {
            case 'i':
                strncpy(iface, optarg, sizeof(iface) - 1);
                break;
            case 'm':
                strncpy(new_mac, optarg, sizeof(new_mac) - 1);
                break;
            case 'h':
                show_help();
                return 0;
            default:
                show_help();
                return 1;
        }
    }
    
    if (strlen(iface) == 0) {
        print_error("Interface is required (-i)");
        show_help();
        return 1;
    }
    
    if (strlen(new_mac) == 0) {
        print_error("MAC address is required (-m)");
        show_help();
        return 1;
    }
    
    char *current = get_current_mac(iface);
    if (current) {
        printf(CYAN "[*] Current MAC: " WHITE "%s\n" RESET, current);
    } else {
        print_error("Could not get current MAC");
        return 1;
    }
    
    if (set_mac(iface, new_mac) == 0) {
        print_success("MAC changing successful");
        char *new = get_current_mac(iface);
        if (new) {
            printf(CYAN "[*] New MAC: " WHITE "%s\n" RESET, new);
        }
        return 0;
    } else {
        print_error("MAC change failed");
        return 1;
    }
}
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <poll.h>
#include <stdbool.h>

#define STARTID 0xFFFF
#define ENDID 0xFFFF
#define ACC_PER 0xFFF8

#pragma pack(1)             // disable alligment

void error(const char *msg);
void printbuffer(u_char  array[], int n);
struct responsepacket fill_request_packet(struct responsepacket *pr, u_char technol, u_int ssubno);

//permission request packet
struct responsepacket {
    u_short startid;
    u_char clientid;
    u_short ac_perm;
    u_char segno;
    u_char leng;
    u_char tech;
    u_int subsc_number;
    u_short endid;
};


u_char segcounter = 0;


const int packtimes =5;

int main(int argc, char *argv[]) {
    u_int a = 4085546805;       //subscriber 1 scenario
    u_int b = 4086668821;       //subscriber 2 scenario
    u_int c = 4086808821;       //subscriber 3 scenario
    u_int d = 4086808822;       //wrong subscriber's number scenario
    u_int e = 4086808822;       //correct subscriber's number but wrong technology scenario

    u_int *subnum[5];
    subnum[0] = &a;
    subnum[1] = &b;
    subnum[2]= &c;
    subnum[3]=	&d;
    subnum[4]= &e;

    u_char tec[5];
    tec[0] = 4;
    tec[1] = 3;
    tec[2] = 2;
    tec[3] = 2;
    tec[4] = 5;                 //wrong subscribers technology scenario

    struct responsepacket pr;
    int ret = 1;
    struct pollfd polfd;
    int sock, n;
    struct sockaddr_in server, from;
    struct hostent *hp;
    unsigned int length;
    u_char  buffer[14];
    u_char buffer_rec[14];
    if (argc != 3) {
        printf("Usage: server port\n");
        exit(1);
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        error("socket");
    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    hp = gethostbyname(argv[1]);
    if (hp == 0)
        error("Unknown host");
    bcopy((char *) hp->h_addr, (char *) &server.sin_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length = sizeof(struct sockaddr_in);
    polfd.fd = sock;
    polfd.events = POLLIN;
    int counter = 0;
    int i=0;
    int packnumber = 0;

    while(counter < 5){
        pr = fill_request_packet(&pr, tec[counter], *subnum[counter]);
        memcpy(buffer, (unsigned char *) &pr, sizeof(pr));
//       printf("buffer sent from client: \n");
//       printbuffer(buffer, sizeof(buffer));
        n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr *) &server, length);
        int failedacks = 0;

        while (failedacks < 2) {
            ret = poll(&polfd, 1, 3000);                    //polling for keeping track of the file descriptors' for timeouts
            //timeout resend packet
            if (ret == 0) {
                printf("timeout, resend packet\n");
                n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr *) &server, length);
                ++counter;
                ++failedacks;
            } else if (ret == -1) {
                perror("poll failure\n");
                exit(1);
            }//successful sent of msg
            else if (ret == 1) {
                n = recvfrom(sock, buffer_rec, n, 0, (struct sockaddr *) &from, &length);
                if (n < 0) error("recvfrom");
                printf("\nreceived from server..\n");
                printbuffer(buffer_rec, n);
                printf("\n");
                ++counter;
                break;
            } else {
                printf("else case\n");
            }
            //++counter;

        }

    }
    // to catch the v delayed packets
    while ( (poll(&polfd, 1, 10000)) == 1 ) {
        n = recvfrom(sock, buffer, n, 0, (struct sockaddr *) &from, &length);
        if (n < 0) error("recvfrom");
        printf("\nreceived from server...\n");
        for (i = 0; i < n; i++) {
            printf("%x ", buffer[i]);
            fflush(stdout);
        }
        printf("\n");
    }
    printf("\n** no activity, close the socket **\n");
    close(sock);

    return 0;
}


void error(const char *msg){
    perror(msg);
    exit(0);
}

void printbuffer(unsigned char array[], int n){
    int i;
    for (i = 0; i < n; i++) {
        printf("%x ", array[i]);
        fflush(stdout);
    }
}


struct responsepacket fill_request_packet(struct responsepacket *pr, u_char technol, u_int ssubno){
    u_char lenofpayload = (u_char) (sizeof(technol)+sizeof(ssubno));
    //  printf("size of payload is %x\n", lenofpayload);
    pr->startid = htons(STARTID);
    pr->clientid = 0xFF;
    pr->ac_perm = htons(ACC_PER);
    pr->segno = 0x1+segcounter ;
    pr->leng = lenofpayload;
    pr->tech = technol;
    pr->subsc_number = htonl(ssubno);
    pr->endid=htons(ENDID);
    ++segcounter;
    return *pr;

}
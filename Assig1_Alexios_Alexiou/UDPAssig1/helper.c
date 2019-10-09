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
#define CLIENTIDMAX 0xFF
#define DATA 0xFFF1
#define PAYLOADMAX 0xFF                                             //max size
#define ENDID 0xFFFF
#define VAR 8

#pragma pack(1)                                                     // disable alligment

void error(const char *);
struct datapacket fillin(struct datapacket *dp);
struct datapacket fill_outof_seq(struct datapacket *dp);
void printbuffer_rec(unsigned char array[], int n);
struct datapacket fill_leng_mismatch(struct datapacket *dp);
struct datapacket fill_end_of_packet_missing(struct datapacket *dp);

struct datapacket {
    unsigned short startid;
    unsigned char clientid;
    unsigned short data;
    unsigned char segno;                                        //ascsii transf? - Number of packet
    unsigned char leng;
    unsigned char payload[PAYLOADMAX];
    unsigned short endid;
};

unsigned char segcounter=0;
int sendcounter = 10;
const unsigned short endidfake = 0xFFF0;

int main(int argc, char *argv[]) {

    struct datapacket dp1;
    unsigned char buffer[sizeof(dp1)];
    unsigned char buf_rec[14];
    struct pollfd polfd;
    unsigned char segn;
    int i;
    int ret;
    int sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    //char buffer[264];
    if (argc != 3) {
        printf("Usage: server port\n");
        exit(1);
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        error("socket");
    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == 0)
        error("Unknown host");
    bcopy((char *) hp->h_addr, (char *) &server.sin_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length = sizeof(struct sockaddr_in);
    polfd.fd = sock;
    polfd.events = POLLIN;
    int counter=0;
    int flag = 1;
    int mispacket=0;
    printf("\n");


    while (counter < sendcounter) {  // 5 - delays(times) from server (for the number of packets to send)
        //send the out of sequence error
      //  printf("sendcounter is %d\n", sendcounter);
      printf("\n");
          if (counter == 7) {
           //   printf("inside the out of sequ");
              dp1 = fill_outof_seq(&dp1);
          }
          else if(counter==8){
              dp1 = fill_leng_mismatch(&dp1);
          }
          else if(counter == 9){
              dp1 = fill_end_of_packet_missing(&dp1);
          }
//          else if(counter == 10){
//             //add : duplicate packet error
//              segcounter--;
//              dp1 = fillin(&dp1);
//          }
          else {
              dp1 = fillin(&dp1);
          }
           memcpy(buffer, (unsigned char *) &dp1, sizeof(dp1));
           n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr *) &server, length);

        int failacks =0;
        while(failacks < 2) {                       //attempts to resend the packet 2 times  (3 sent in total)
            ret = poll(&polfd, 1, 3000);
            if (ret == 0) {
                printf("** Error-timeout, resend Data packet with seg no: %x **\n", buffer[5]);
                n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr *) &server, length);
                if (n < 0) error("Sendto");
               ++counter;
               ++failacks;
            } else if (ret == -1) {
                perror("poll failure\n");
                exit(1);
                //successful sent of message
            } else if (ret == 1) {
                n = recvfrom(sock, buf_rec, n, 0, (struct sockaddr *) &from, &length);
                if (n < 0) error("recvfrom");

                if( (buf_rec[3]== 0xFF) && (buf_rec[4] == 0xF3) ){
                    printf("REJ message received from server with size %d is:\n", n);
                    if ( (buf_rec[5]==0xFF) && (buf_rec[6]==0xF4) ){
                        printf("Out Of Sequence error\n");
                    }else if( (buf_rec[5]==0xFF) && (buf_rec[6]==0xF5) ){
                        printf("Length mismatch error\n");
                    }else if((buf_rec[5]==0xFF) && (buf_rec[6]==0xF6) ){
                        printf("End of Packet Missing\n");
                    }else if( (buf_rec[5]==0xFF) && (buf_rec[6]==0xF7) ){
                        printf("Duplicate packet error\n");
                    }else{
                        printf("kanena apo ta parapano errors");
                    }
                    printbuffer_rec(buf_rec, n);
                }else {
                    printf("ACK message received from server with size %d is:\n", n);
                    printbuffer_rec(buf_rec, n);
                }
                printf("\n");
                ++counter;
                break;
            } else {
                printf("else case");
            }
            if(failacks>=2) {
                printf("Server does not respond - packet with seg: %d is dropped\n", buffer[5]);
                //segcounter++;
            }
        }
       // ++counter;
    }
    // to catch the v delayed packets
    while ( (poll(&polfd, 1, 10000)) == 1 ) {
        n = recvfrom(sock, buf_rec, n, 0, (struct sockaddr *) &from, &length);
            if (n < 0) error("recvfrom");
            printf("delayed message ACK message is :\n");
            for (i = 0; i < n; i++) {                                                     // *** n is less than the actual size of thr buffer_rec ***
                printf("%x ", buf_rec[i]);
                fflush(stdout);
            }
            printf("\n");
    }
        printf("\n** no activity, close the socket **\n");
        close(sock);

    return 0;
}

struct datapacket fillin(struct datapacket *dp){
    unsigned char msg [] = "importantmessagehereee";
    dp->startid = htons(STARTID);
    dp->clientid = CLIENTIDMAX;
    dp->data = htons(DATA);
    dp->segno = 0x1+segcounter ;
    unsigned char msgsize = sizeof(msg);
    dp->leng = msgsize;
    //unsigned char paymsg[255] = "alex";
    unsigned char paymsg[255];
    memset(paymsg,0, sizeof(paymsg));
    memcpy(paymsg, (unsigned char *) msg, sizeof(msg));
    memcpy(dp->payload, (unsigned char *) &paymsg, sizeof(paymsg));
    dp->endid=htons(ENDID);
    //printf("%s\n", dp->data);
   ++segcounter;
    return *dp;

}


struct datapacket fill_leng_mismatch(struct datapacket *dp){
    unsigned char msg [] = "fakinglength";
    dp->startid = htons(STARTID);
    dp->clientid = CLIENTIDMAX;
    dp->data = htons(DATA);
    dp->segno = 0x1+segcounter ;
   // unsigned char msgsize = sizeof(msg);
    dp->leng = PAYLOADMAX;                       // faking the size by giving the maximum
    //unsigned char paymsg[255] = "alex";
    unsigned char paymsg[255];
    memset(paymsg,0, sizeof(paymsg));
    memcpy(paymsg, (unsigned char *) msg, sizeof(msg));
    memcpy(dp->payload, (unsigned char *) &paymsg, sizeof(paymsg));
    dp->endid=htons(ENDID);
    //printf("%s\n", dp->data);
    ++segcounter;
    return *dp;

}

struct datapacket fill_outof_seq(struct datapacket *dp){
    unsigned char msg [] = "importantmessagehere";
    dp->startid = htons(STARTID);
    dp->clientid = CLIENTIDMAX;
    dp->data = htons(DATA);
    dp->segno = 0x1+(segcounter +1) ;
    unsigned char msgsize = sizeof(msg);
    dp->leng = msgsize;
    unsigned char paymsg[255] = "outofsequencemessage";
    memcpy(dp->payload, (unsigned char *) &paymsg, sizeof(paymsg));
    dp->endid=htons(ENDID);
    //printf("%s\n", dp->data);
   // segcounter;
    return *dp;
}

struct datapacket fill_end_of_packet_missing(struct datapacket *dp){
    unsigned char msg [] = "importantmessagehereee";
    dp->startid = htons(STARTID);
    dp->clientid = CLIENTIDMAX;
    dp->data = htons(DATA);
    dp->segno = 0x1+segcounter ;
    unsigned char msgsize = sizeof(msg);
    dp->leng = msgsize;
    //unsigned char paymsg[255] = "alex";
    unsigned char paymsg[255];
    memset(paymsg,0, sizeof(paymsg));
    memcpy(paymsg, (unsigned char *) msg, sizeof(msg));
    memcpy(dp->payload, (unsigned char *) &paymsg, sizeof(paymsg));
    dp->endid=htons(endidfake);
    //printf("%s\n", dp->data);
    ++segcounter;
    return *dp;

}

void printbuffer_rec(unsigned char array[], int n){
int i;
    for (i = 0; i < n; i++) {                                                     // *** n is less than the actual size of thr buffer_rec ***
        printf("%x ", array[i]);
        fflush(stdout);
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}




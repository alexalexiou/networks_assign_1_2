#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#define STARTID 0xFFFF
#define CLIENTIDMAX 0xFF
#define ACK 0xFFF2
#define REJ 0xFFF3
#define ENDID 0xFFFF
//#define REJ_OOS 0xFFF4              //Out Of Sequence reject code

#pragma pack(1)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct ackpacket fillin(struct ackpacket *ap, unsigned char seg);
//struct rejpacket fillin_rejpacket(struct rejpacket *rp, unsigned char seg);
struct rejpacket fillin_rejpacket(struct rejpacket *rp, unsigned char seg, unsigned short errortype);

struct ackpacket {
    unsigned short startid;
    unsigned char clientid;
    unsigned short ack;
    unsigned char rec_segno;
    unsigned short endid;
};

struct rejpacket{
    unsigned short startid;
    unsigned char clientid;
    unsigned short rej;
    unsigned short rejcode;
    unsigned char rec_segno;
    unsigned short endid;
};

const unsigned short lengthmissmatch = 0xFFF5;
const unsigned short rejoos = 0xFFF4;
const unsigned short endofpacketiderror = 0xFFF6;
const unsigned short duplicatepacketerror = 0xFFF7;

int main(int argc, char *argv[]) {
    struct ackpacket ap;
    struct rejpacket rp;
    unsigned char buffer_ack[sizeof(ap)];
    unsigned char buffer_rej[sizeof(rp)];
    int c=0;
    int i = 0;
    int sockfd, length, n;
    socklen_t fromlen;                                                                         //length of the msg sent from the client
    struct sockaddr_in server;
    struct sockaddr_in from;                                                                   //keeps info of the client in this structure
    unsigned char buf[264];                                                                    //bytes (characters' size)
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(0);
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);                                                   // 'sock' is a fd
    if (sockfd < 0) error("Opening socket");
    length = sizeof(server);
    bzero(&server,length);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(atoi(argv[1]));
    if (bind(sockfd,(struct sockaddr *) &server, length)<0)                                   // here the socket binds to an address
        error("binding");
    fromlen = sizeof(struct sockaddr_in);
   int prevseg=0;

    while (1) {
        printf("\n");
        n = recvfrom(sockfd, buf, 264, 0, (struct sockaddr *) &from, &fromlen);               //read() in TCP - receives from client
        if (n < 0) error("recvfrom");
        printf("DATA packet received from client with size %d is:\n", n);
        unsigned char seg = buf[5];                                         //assigns the seg number received to the buffer

        for(i=0; i< sizeof(buf); i++){
           printf("%x ", buf[i]);
            fflush(stdout);
        }
        printf("\n");
        int realpaysize=1;
        i=7;
        while(buf[i]!= '\0'){
            realpaysize++;
            ++i;
        }

//        if(c<2) {
//            sleep(3);
//            c++;
//        }else{
//            sleep(2);
//        }

//        if((c>2) && (c<6)){
//            sleep(3);
//            c++;
//        }
//        else{
//            sleep(2);
//        }

        sleep(2);
      //out of sequence
        if ( (seg-prevseg)>=2 ) {
            rp = fillin_rejpacket(&rp, seg, rejoos);
            memcpy(buffer_rej, (unsigned char *) &rp, sizeof(rp));
            n = sendto( sockfd, buffer_rej, sizeof(buffer_rej), 0, (struct sockaddr *) &from, fromlen);
            prevseg = seg;

      //length mismatch error
        }else if(buf[6]!= realpaysize){
            printf("inside the lenght missmatch \n");
            rp = fillin_rejpacket(&rp, seg, lengthmissmatch);
            memcpy(buffer_rej, (unsigned char *) &rp, sizeof(rp));
            n = sendto( sockfd, buffer_rej, sizeof(buffer_rej), 0, (struct sockaddr *) &from, fromlen);
            prevseg = seg;
        }//end of packet identifier is missing
         else if( (buf[262]!=0xFF) || (buf[263]!=0xFF) ){
             printf("inside the End of Packet Identifier error\n");
            rp = fillin_rejpacket(&rp, seg, endofpacketiderror);
            memcpy(buffer_rej, (unsigned char *) &rp, sizeof(rp));
            n = sendto( sockfd, buffer_rej, sizeof(buffer_rej), 0, (struct sockaddr *) &from, fromlen);
            prevseg = seg;
         }
         else if( seg-prevseg == 0 ){
             printf("inside the duplicate package error\n");
            rp = fillin_rejpacket(&rp, seg, duplicatepacketerror);
            memcpy(buffer_rej, (unsigned char *) &rp, sizeof(rp));
            n = sendto( sockfd, buffer_rej, sizeof(buffer_rej), 0, (struct sockaddr *) &from, fromlen);
            prevseg = seg;
         }
      //default send ACCK
        else{
            ap = fillin(&ap, seg);
            memcpy(buffer_ack, (unsigned char *) &ap, sizeof(ap));
            n = sendto(sockfd, buffer_ack, sizeof(buffer_ack), 0, (struct sockaddr *) &from, fromlen);      //write() in TCP - sends to client
            prevseg = seg;

        }

        if (n < 0) error("sendto");

    }
    return 0;
}



struct ackpacket fillin(struct ackpacket *ap, unsigned char seg){
    ap->startid = htons(STARTID);
    ap->clientid = CLIENTIDMAX;
    ap->ack = htons(ACK);                                                                   // Big Endian
    ap->rec_segno = seg;						                                            //needs to examine the clients segno field and reply appropriatelly
    ap->endid = ENDID;
    return *ap;
}

struct rejpacket fillin_rejpacket(struct rejpacket *rp, unsigned char seg, const unsigned short errortype){
    rp->startid = htons(STARTID);
    rp->clientid = CLIENTIDMAX;
    rp->rej = htons(REJ);
    rp->rejcode = htons(errortype);
    rp->rec_segno = seg;
    rp->endid = ENDID;
    return *rp;
}

#pragma clang diagnostic pop



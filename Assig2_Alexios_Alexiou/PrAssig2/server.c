#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>

#define STARTID 0xFFFF
#define ENDID 0xFFFF

#pragma pack(1)                    //for structure padding
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void error(const char *msg);
struct responsepacket fill_resp_packet(struct responsepacket *rp, u_short status, u_char segno, u_char technol, u_int ssubno);
void printbuffer(unsigned char array[], int n);
void readfromfile();
int compare_strings(char restostring[], char str2[]);

struct responsepacket {
    u_short startid;
    u_char clientid;
    u_short subsc_status;
    u_char segno;
    u_char leng;
    u_char tech;
    u_int subsc_number;
    u_short endid;
};

u_short notpaid = 0xFFF9;
u_short notexist = 0xFFFA;
u_short access_ok = 0XFFFB;
char str1 [15];
char str2 [15];
char str3 [15];

int main(int argc, char *argv[]){
    struct responsepacket rp;
    int c=0;
    int sockfd, length, n;
    socklen_t fromlen;                                                                         //length of the msg sent from the client
    struct sockaddr_in server;
    struct sockaddr_in from;                                                                   //keeps info of the client in this structure
    u_char buf[14];
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(0);
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);                                                   // socket's file descriptor
    if (sockfd < 0) error("Opening socket");
    length = sizeof(server);
    bzero(&server,length);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(atoi(argv[1]));
    if (bind(sockfd,(struct sockaddr *) &server, length)<0)                                   // here the socket binds to an address
        error("binding");
    fromlen = sizeof(struct sockaddr_in);
    int i=0;
    readfromfile();

    unsigned char  subs_arr[5];

    while(1){
        n = recvfrom( sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, &fromlen);
        if (n < 0) error("recvfrom");
        u_char segnumber = buf[5];
//        printf("seg number is buf[5]: %x\n", segnumber);
        u_char tecnnl = buf[7];
//        printf("technol is buf[7]: %x\n", tecnnl);
        int k=0;
        for(i=8;i<12; i++){
            subs_arr[k++] = buf[i];
        }
        subs_arr[4] = '\0';
        printf("\nreceived from client..\n");
        printbuffer(buf, n);
        u_int res;

        //retrieve the unisigned 4 byte integer from 4x1 Byte char buffer!!
        res = (((u_int)buf[8]) << 24) | (((u_int)buf[9]) << 16) | (((u_int)buf[10]) << 8) | ((u_int)buf[11]);
//        printf("res is %u\n", res);
       u_char  restostring[11] ;
        sprintf(restostring, "%lu", res);

    if(compare_strings(restostring,str1)){
        printf("Subscriber Found: subno is matching \n");
        if ((u_char) (buf[7]+48)== (u_char)(str1[12])){                                      //ascii 48 -> 0
            printf("str1 tech matches too\n");
            if(str1[14] == 48+1){                                                            //paid, access
                    printf("paid let him access\n");
                    rp = fill_resp_packet(&rp, access_ok, segnumber, tecnnl,res);
                    memcpy(buf, (unsigned char *) &rp, sizeof(rp));
                     sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
            }else{                              //not paid
                printf("has not paid, access denied\n");
                    rp = fill_resp_packet(&rp, notpaid, segnumber, tecnnl,res);
                    memcpy(buf, (unsigned char *) &rp, sizeof(rp));
                     sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
            }
        }else {
            printf("but the str1 technology doesn't match\n ");
                rp = fill_resp_packet(&rp, notexist, segnumber, tecnnl,res);
                memcpy(buf, (unsigned char *) &rp, sizeof(rp));
                 sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
        }

    }else if (compare_strings(restostring, str2)){
        printf("Subscriber Found \n");
        if ((u_char) (buf[7]+48)== (u_char)(str2[12])){
            printf("tech matches too\n");
            if(str2[14] == 48+1){
                printf("paid let him access\n");
                rp = fill_resp_packet(&rp, access_ok, segnumber, tecnnl,res);
                memcpy(buf, (unsigned char *) &rp, sizeof(rp));
                 sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
            }else{                              //not paid
                printf("has not paid, access denied\n");
                rp = fill_resp_packet(&rp, notpaid, segnumber, tecnnl,res);
                memcpy(buf, (unsigned char *) &rp, sizeof(rp));
                 sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
            }
        }else {
            printf("but the tech doesn't match\n");
            rp = fill_resp_packet(&rp, notexist, segnumber, tecnnl,res);
            memcpy(buf, (unsigned char *) &rp, sizeof(rp));
             sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
        }
    } else if(compare_strings(restostring, str3)){
        printf("Subscriber Found \n");
        if ((u_char) (buf[7]+48)== (u_char)(str3[12])){
            printf("tech matches too\n");
            if(str3[14] == 48+1){
                printf("paid let him access\n");

                rp = fill_resp_packet(&rp, access_ok, segnumber, tecnnl,res);
                memcpy(buf, (unsigned char *) &rp, sizeof(rp));
                 sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
            }else{                              //not paid
                printf("has not paid, access denied\n");
                rp = fill_resp_packet(&rp, notpaid, segnumber, tecnnl,res);
                memcpy(buf, (unsigned char *) &rp, sizeof(rp));
                sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
            }
        }else {
            printf("but the  tech doesn't match\n");
            rp = fill_resp_packet(&rp, notexist, segnumber, tecnnl,res);
            memcpy(buf, (unsigned char *) &rp, sizeof(rp));
            sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
        }
    }else{
        printf("Subscriber's number is not found\n");
        rp = fill_resp_packet(&rp, notexist, segnumber, tecnnl,res);
        memcpy(buf, (unsigned char *) &rp, sizeof(rp));
         sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &from, fromlen);
    }

    //simulating delay for the n packet in order to get timeout
        if(c<1) {
            sleep(3);
            c++;
        }else{
            sleep(2);
        }

//            sleep(2);

    }
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
    printf("\n");
}

//adding the info in the struct prior to respond to client
struct responsepacket fill_resp_packet(struct responsepacket *rp, u_short status, u_char segno, u_char technol, u_int ssubno){
    u_char lenofpayload = (u_char) (sizeof(technol)+sizeof(ssubno));
   // printf("size of payload is %x\n", lenofpayload);
    rp->startid = htons(STARTID);
    rp->clientid = 0xFF;
    rp->subsc_status = htons(status); // since the proccessor's architecture is Little Endian
    rp->segno = segno ;
    rp->leng = lenofpayload;
    rp->tech = technol;
    rp->subsc_number = htonl(ssubno); //since the proccessor's architecture is Little Endian
    rp->endid=htons(ENDID);
    return *rp;

}

//reading the text file and loading the information on the server
void readfromfile(){
//printf("printing the file's content\n");
    FILE * pFile;
    pFile = fopen ("/Users/alex/ClionProjects/PrAssig2/Verification_Database.txt","r");
    if(pFile == NULL)
    {
        printf("Error! cannot open file");
        exit(1);
    }
    fscanf(pFile, "%[^\r\n]", str1);
//    printf("%s\n", str1);
//   printf("size of str1 is %lu\n",sizeof(str1) );

    fseek(pFile, 16, SEEK_SET);
    fscanf(pFile, "%[^\r\n]", str2);
//    printf("%s\n", str2);
//    printf("size of str2 is %lu\n",sizeof(str2) );
    fseek(pFile,32,SEEK_SET);
    fscanf(pFile, "%[^\r\n]", str3);
//    printf("%s\n", str3);
//    printf("size of str3 is %lu\n",sizeof(str3) );
//    printf("\n");
    fclose (pFile);
}

//comparison of the phone numbers between client's packet and text file's data
int compare_strings(char restostring[], char strx[]) {
    int d = 0;
    int i=0;
    for (i = 0; i < 11; i++) {
        if (restostring[i] == strx[i]) {
            d++;
        }
    }
    if (d == 10) {
        return 1;
    } else
        return 0;
}
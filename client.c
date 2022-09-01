#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <math.h>
#define MAXLEN 1024
#define WINDOW_SIZE 32 // it is easier to pick 32 since I don't need to slide window.




//TODO'S Reliable Data Transfer using Selective Repepat, checksum function, make packets ...
struct ACK{
    int idx; 
};
struct packet{
    int data[8];
    int seqNumber;
};



struct SelectiveRepeatProtocol{







};
int mycompare(char * arr){
    // :((((((((((((((((((((((((((((((((
    return arr[0] == 'B' && arr[1] == 'Y' && arr[2] == 'E';
}

char ** slicingMsg(char *msgBuff){
    // This function slides messages whose length is 8 and we put these data to packet structure.
    char **retArr = malloc(sizeof(char *) * ceil( (float) strlen(msgBuff) / 8));
    int iter1 = 0,
        iter2 = 0,
        i = 0;
    for(i = 0;i < 8;i++){
        retArr[i] = malloc(sizeof(char) * 9);
    }
    for(i = 0;i < strlen(msgBuff);i++,iter2++){
        if((i % 8 == 0) && i != 0){
            retArr[iter1][iter2] = '\0';
            iter1++;
            iter2 = 0;
        }
        retArr[iter1][iter2] = msgBuff[i];
    }
    retArr[iter1][iter2] = '\0'; 
    return retArr;
}
void serverInitializer(struct sockaddr_in * server,int portNumb){
    //This function sets sockaddr_in struct to intended settings 
    //with portNumber portNumb
    memset(server,0,sizeof(*server));
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
	server->sin_port = htons(portNumb);
}
void sndMsg(int socketNumb, char * msg, struct sockaddr_in * dest){
    //This function sends messages to dest addres dest
    int cntrol = sendto(socketNumb,msg,strlen(msg),MSG_CONFIRM,(struct sockaddr *) dest, sizeof(*dest)); 
    if(cntrol < 0){
        printf("Error at sndMsg()\n");
        exit(EXIT_FAILURE);
    }
}
void receiveMsg(int socketNumb,char * msgBuffer ,struct sockaddr_in * from,int * len){
    // if the message is ready to read in socketNumb we get the message via this function.
    int size = 0;
    size = recvfrom(socketNumb,msgBuffer,MAXLEN,MSG_WAITALL,(struct sockaddr *) from,len);
    if(size < 0){
        printf("Fail at receiveMsg(), size = %d\n",size);
        exit(EXIT_FAILURE);
    }
    msgBuffer[size] = '\0';   
}
void setPollfds(struct pollfd * fds,int socketNo){
    // To understand which socket is ready to process, array of pollfd struct must be initialized.
    // This function handle initializing pollfd struct.

    fds[0].fd = socketNo;
    fds[0].events = POLLIN; // if receiving data is ready to be receive.
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN; // if the sending message is ready.
}
void clearBuff(char * msg,int size){
    // For clearing the message buffer.
    memset(msg,0,sizeof(msg[0]) * size);
}
void receive_and_send(int socketNumb,char * msgBuff,struct sockaddr_in * generic){
    //This function receives and sends char* simultaneously using poll function.
    //Poll function is for monitoring sockets.
    struct pollfd pfds[2];
    setPollfds(pfds,socketNumb);
    while(1){
        int numb_of_events = poll(pfds,2,3000);
        if(!numb_of_events) continue;
        if(pfds[0].revents & POLLIN){
            // data ready to be received so get data.
            int len = sizeof(*generic);
            receiveMsg(socketNumb,msgBuff,generic,&len);
            printf("%s",msgBuff);
            clearBuff(msgBuff,MAXLEN);
        }
        else if(pfds[1].revents & POLLIN){
            // data ready to send. So send data to addr.
            fgets(msgBuff,MAXLEN,stdin);
            fflush(stdin);
            char ** pckt = slicingMsg(msgBuff);
            int size = ceil((float) strlen(msgBuff) / 8); // size of the sliced char * array.
            int i = 0;
            for(;i < size;i++)  sndMsg(socketNumb,pckt[i],generic);
            if(mycompare(msgBuff)){
                // :(((((((((((((
                printf("BYE message send, closing...\n");
                exit(EXIT_SUCCESS);
            }
        }
    }
    clearBuff(msgBuff,MAXLEN);
}

int main(int argc, char * argv[]){
    if(argc < 4){
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    }
    //Getting port numbers from main's arguments
    int clientPortNumb = atoi(argv[3]),
        serverPortNumb = atoi(argv[2]);
    char *ipAddr = argv[1];
    struct sockaddr_in clientAddr,serverAddr;
    serverInitializer(&serverAddr,serverPortNumb);

    //Create udp socket for sending message.
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketfd < 0){
        printf("Socket creation error\n");
        exit(EXIT_FAILURE);
    }
    char msg[MAXLEN];
    receive_and_send(socketfd,msg,&serverAddr);

}
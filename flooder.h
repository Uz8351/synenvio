/**** flooder.h******* */
 
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
 
 
struct supuesto_encabezado
 
{
    unsigned int Direccion_origen;
    unsigned int Direccion_destino;
    unsigned char Marcador    ;
    unsigned char protocolo;
    unsigned short tcp_longitud;
 
    struct tcphdr tcp;
};
 
unsigned short Funcion_sum(unsigned short *ptr,int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
 
    return(answer);
}

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <strings.h>
#include <stdlib.h>

#define HOSTLEN  256
#define BACKLOG  1

int make_server_socket_q(int,int);

int make_server_socket(int portnum)
{
    return make_server_socket_q(portnum, BACKLOG);
}

int make_server_socket_q(int portnum, int backlog)
{
    struct sockaddr_in saddr;
    struct hostent *hp;
    char hostname[HOSTLEN];
    int sock_id;

    sock_id = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_id == -1)
        return -1;

    /** build address and bind it to socket **/
    bzero((void *)&saddr, sizeof(saddr));
    gethostname(hostname, HOSTLEN);
    hp = gethostbyname(hostname);

    bcopy((void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
    saddr.sin_port = htons(portnum);
    saddr.sin_family = AF_INET;

    if (bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
        return -1;


    if (listen(sock_id, backlog) != 0)
        return -1;

    return sock_id;
}


/*
 * proxy.c - ICS Web proxy
 *
 *
 */

#include "csapp.h"
#include <stdarg.h>
#include <sys/select.h>

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, char *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, size_t size);
void doit(int connfd, struct sockaddr_in sockaddr);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void *thread(void *);

sem_t mutex;

typedef struct doitargs
{
    int tid;
    int connfd;
    struct sockaddr_in clientaddr;
} doitargs_t;

ssize_t Rio_readn_w(int fd, void *usrbuf, size_t n)
{
    ssize_t readn;
    if (0 > (readn = rio_readn(fd, usrbuf, n)))
    {
        readn = 0;
        fprintf(stderr, "Rio_readn error");
    }
    return readn;
}
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n)
{
    ssize_t readnb;
    if (0 > (readnb = rio_readnb(rp, usrbuf, n)))
    {
        readnb = 0;
        fprintf(stderr, "Rio_readnb error");
    }
    return readnb;
}
ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen)
{
    ssize_t readlineb;
    if (0 > (readlineb = rio_readlineb(rp, usrbuf, maxlen)))
    {
        readlineb = 0;
        fprintf(stderr, "Rio_readlineb error");
    }
    return readlineb;
}
ssize_t Rio_writen_w(int fd, void *usrbuf, size_t n)
{
    ssize_t write_in = rio_writen(fd, usrbuf, n);
    if (n != write_in)
    {
        fprintf(stderr, "Rio_writen error");
    }
    return write_in;
}

/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{
    /* Check arguments */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    int listenfd;
    socklen_t clientlen;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    doitargs_t* doitargsp;

    pthread_t tid;
    Signal(SIGPIPE, SIG_IGN);

    Sem_init(&mutex, 0, 1);

    listenfd = Open_listenfd(argv[1]);
    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        // connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        // Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);

        // Pthread_create(&tid, NULL, thread, &connfd);
        // printf("Accept connection from (%s, %s)\n", client_hostname, client_port);
        // struct sockaddr_in sockaddr;
        // doit(connfd, &sockaddr);
        // Close(connfd);
        doitargsp = Malloc(sizeof(doitargs_t));
        doitargsp->tid = tid;        
        doitargsp->connfd = Accept(listenfd, (SA *)(&(doitargsp->clientaddr)), &clientlen);
        Getnameinfo((SA *)&(doitargsp->clientaddr), clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        // printf("%d",doitargsp->clientaddr.sin_port);
        // printf("%s",doitargsp->clientaddr.sin_addr.s_addr);
        Pthread_create(&tid, NULL, thread, doitargsp);
        tid++;
    }

    exit(0);
}

void *thread(void *vargp)
{
    doitargs_t* argsp = (doitargs_t*)vargp;
    Pthread_detach(pthread_self());
    doit(argsp->connfd, argsp->clientaddr);
    close(argsp->connfd);
    free(vargp);
    return NULL;
}

void doit(int connfd, struct sockaddr_in sockaddr)
{
    size_t n;
    int serverfd;
    char client_buf[MAXLINE], server_buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];

    char hostname[MAXLINE], pathname[MAXLINE], port[MAXLINE];

    int response_body_size, requset_body_size, response_head_size;
    int response_tmp, read, write;
    rio_t client_rio, server_rio;

    Rio_readinitb(&client_rio, connfd);
    n = Rio_readlineb_w(&client_rio, client_buf, MAXLINE);
    if (n <= 0)
    {
        return;
    }

    sscanf(client_buf, "%s %s %s", method, uri, version);
    // if(strcasecmp(method, "GET")){
    //     clienterror(connfd, method, "501", "Not Iimplemented",
    //                 "Tiny does not implement this method");
    //     return;
    // }

    if (parse_uri(uri, hostname, pathname, port) != 0)
    {
        return;
    }
    // printf("%s\n",hostname);
    // printf("%s\n",pathname);
    // printf("%s\n",port);
    char request[MAXLINE];
    sprintf(request, "%s /%s %s\r\n", method, pathname, version);

    n = 0;

    while ((n = Rio_readlineb_w(&client_rio, client_buf, MAXLINE)))
    {
        if (n <= 0)
        {
            return;
        }
        if (!strncasecmp(client_buf, "Content-Length", 14))
        {
            requset_body_size = atoi(client_buf + 16);
        }
        sprintf(request, "%s%s", request, client_buf);
        // printf("--%s", request);
        // printf("bb%s", client_buf);
        if (!strcmp(client_buf, "\r\n"))
        {
            break;
        }
    }

    if (n <= 0)
    {
        return;
    }
    // printf("%s", request);

    // printf("%d\n",serverfd);

    serverfd = Open_clientfd(hostname, port);
    if (serverfd <= 0)
    {
        return;
    }

    Rio_readinitb(&server_rio, serverfd);
    n = Rio_writen_w(serverfd, request, strlen(request));

    if (n != strlen(request))
    {
        Close(serverfd);
        return;
    }
    if (strcmp("GET", method))
    {
        // the body ends with 0\r\n if it is chunked
        if (0 == requset_body_size)
        {
            while (0 != (n = Rio_readlineb_w(&client_rio, client_buf, MAXLINE)))
            {
                Rio_writen_w(serverfd, client_buf, n);
                if (!strcmp(client_buf, "0\r\n"))
                {
                    break;
                }
            }
        }
        // or we can use content length to determine how many bytes to read
        else
        {
            for (int i = 0; i < requset_body_size; i++)
            {
                if (0 < Rio_readnb_w(&client_rio, client_buf, 1))
                {
                    Rio_writen_w(serverfd, client_buf, 1);
                }
            }
        }
    }

    n = 0;
    while ((n = Rio_readlineb_w(&server_rio, server_buf, MAXLINE)) != 0)
    {
        if (n <= 0)
        {
            Close(serverfd);
            return;
        }

        // printf("Proxy received %ld bytes, then send\n", n);
        if (!strncasecmp(server_buf, "Content-Length", 14))
        {
            response_body_size = atoi(server_buf + 16);
        }

        response_head_size += n;
        // if (Rio_writen_w(connfd, server_buf, n) != strlen(server_buf))
        // {
        //     Close(serverfd);
        //     return;
        // }
        write = Rio_writen_w(connfd, server_buf, strlen(server_buf));

        if (write != strlen(server_buf))
        {
            Close(serverfd);
            return;
        }

        if (!strcmp(server_buf, "\r\n"))
        {
            break;
        }
    }

    if (n <= 0)
    {
        Close(serverfd);
        return;
    }

    response_tmp = response_body_size;
    read = 0;
    write = 0;
    while (response_tmp > 0)
    {
        read = Rio_readnb_w(&server_rio, server_buf, 1);
        if ((read <= 0) && (response_tmp > 1))
        {
            Close(serverfd);
            return;
        }

        write = Rio_writen_w(connfd, server_buf, 1);
        if (write != 1)
        {
            Close(serverfd);
            return;
        }

        response_tmp -= 1;
    }

    Close(serverfd);

    char logstring[MAXLINE];
    format_log_entry(logstring, &sockaddr, uri, response_body_size + response_head_size);

    P(&mutex);
    printf("%s\n", logstring);
    V(&mutex);
}

/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, char *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0)
    {
        hostname[0] = '\0';
        return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    if (hostend == NULL)
        return -1;
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    if (*hostend == ':')
    {
        char *p = hostend + 1;
        while (isdigit(*p))
            *port++ = *p++;
        *port = '\0';
    }
    else
    {
        strcpy(port, "80");
    }

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL)
    {
        pathname[0] = '\0';
    }
    else
    {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), the number of bytes
 * from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, size_t size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /*
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 12, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s %zu", time_str, a, b, c, d, uri, size);
    // sprintf(logstring, "%s: 127.0.0.1 %s %zu", time_str, uri, size);

}

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>

/****************************************
 Author: Joel Klein, Katie Stasaski, Lucas Chaufournier, Tim Wood
 with a little help from
 http://beej.us/guide/bgnet/
 ****************************************/

#define BACKLOG 10     // how many pending connections queue will hold

/* Struct for contents of http request */
typedef struct http_request {
    char* method;
    char* request;
    char* version;
} http_request;

/* Struct to contain information for an http response */
typedef struct http_response {
    char* version;
    char* response_code;
    char* host;
    char* content_type;
    int len;
    char* body;
} http_response;

http_response *http_response_alloc()
{
    http_response* response = malloc(sizeof(struct http_response));
    response->version = "HTTP/1.1";
    response->host = "host.name";
    printf("response object created\n");
    return response;
}

void http_response_free(http_response * response)
{
    free(response);
}

//char* response_creation()

int main(int argc, char ** argv)
{
    char* server_port = "1234";
    int sockfd, rc;
    int yes=1;
    struct addrinfo hints, *server;
    char message[256];
    char *requestmsgseg = NULL; /* Storage for current segment of the request message */
    char* resp = NULL; /* holds the response message returned from a request */
    int o, i;
    
    /* Command line args:
     -p port
     */
    while ((o = getopt (argc, argv, "p:")) != -1) {
        switch(o){
            case 'p':
                server_port = optarg;
                break;
            case '?':
                if(optopt == 'p') {
                    fprintf (stderr, "Option %c requires an argument.\n", optopt);
                }
                else {
                    fprintf (stderr, "Unknown argument: %c.\n", optopt);
                }
                break;
        }
    }
    
    printf("listening on port: %s\n", server_port);
    
    /* The hints struct is used to specify what kind of server info we are looking for */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; /* or SOCK_DGRAM */
    hints.ai_flags = AI_PASSIVE;
    
    /* getaddrinfo() gives us back a server address we can connect to.
     The first parameter is NULL since we want an address on this host.
     It actually gives us a linked list of addresses, but we'll just use the first.
     */
    if (rc = getaddrinfo(NULL, server_port, &hints, &server) != 0) {
        perror(gai_strerror(rc));
        exit(-1);
    }
    
    /* Now we can create the socket and bind it to the local IP and port */
    sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (sockfd == -1) {
        perror("ERROR opening socket");
        exit(-1);
    }
    /* Get rid of "Address already in use" error messages */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(-1);
    }
    rc = bind(sockfd, server->ai_addr, server->ai_addrlen);
    if (rc == -1) {
        perror("ERROR on connect");
        close(sockfd);
        exit(-1);
        // TODO: could use goto here for error cleanup
    }
    
    /* Time to listen for clients.*/
    listen(sockfd, BACKLOG);
    /* Loop forever accepting new connections. */
    while(1) {
        struct sockaddr_storage client_addr;
        socklen_t addr_size;
        int clientfd;
        int bytes_read;
        char * response_message = NULL; // container for the HTTP response message
        
        addr_size = sizeof client_addr;
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        
        bytes_read = read(clientfd, message, sizeof message);
        if(bytes_read < 0) {
            perror("ERROR reading socket");
        }
        
        http_request *req = malloc(sizeof(struct http_request));
        /* Break up the request message into space delimited segments */
        requestmsgseg = strtok(message, " ");
        req->method = strdup(requestmsgseg);
        
        /*This is necessary to prevent not get requests from segfaulting server. #Lucas*/
        if(strcmp(req->method,"GET")!=0)
        {
            //printf("Not a get request");
            close(clientfd);
            free(req);
            continue;
        }
        
        
        requestmsgseg = strtok(NULL, " ");
        req->request = strdup(requestmsgseg);
        requestmsgseg = strtok(NULL, " ");
        req->version = strdup(requestmsgseg);
        /*Check for get request*/
        
        /* Test the request was recorded correctly */
        printf("Method: %s, Request: %s, Version: %s\n", req->method, req->request, req->version);
        
        
        /* File exists, return contents */
        if (access(++req->request, R_OK) != -1) {
            FILE * fp;
            struct stat st;
            int file_size, size;
            
            /* create the response object to contain response information */
            http_response* response = http_response_alloc();
            response->response_code = "200";
            
            /* open the file requested and move the information into the response object */
            fp = fopen(req->request, "r");
            stat(req->request, &st);
            size = st.st_size; // need to know the size of the object we are manipulating
            response->body = malloc(size);
            file_size = fread(response->body, 1, size, fp); //moves contents into response->body
            response->len = file_size;
            
            asprintf(&response_message, "%s %s\nHost: %s\nContent-Type: %s\nContent-Length: %d\n\n%s", response->version, response->response_code, response->host, response->content_type, response->len, response->body);
            rc = send(clientfd, response_message, strlen(response_message)+1, 0);
            if(rc <0) {
                perror("ERROR on send");
                exit(-1);
            }
            free(response);
            
            /* File does not exist, return 404 */
        } else {
            //file does not exist return 404
            printf("File does not exist\n");
            http_response * response = http_response_alloc();
            response->response_code = "404";
            response->body = "<html><head></head><body><h1 style='max-width:600px;margin:auto;margin-top:25px'>ERROR 404: File Not Found</h1></body></html>";
            response->len = strlen(response->body);
            
            asprintf(&response_message, "%s %s\nHost: %s\nContent-Type: %s\nContent-Length: %d\n\n%s", response->version, response->response_code, response->host, response->content_type, response->len, response->body);
            rc = send(clientfd, response_message, strlen(response_message)+1, 0);
            if(rc <0) {
                perror("ERROR on send");
                exit(-1);
            }
            free(response);
        }
        
        close(clientfd);
        free (req);
    }
    
out:
    freeaddrinfo(server);
    close(sockfd);
    
    printf("Done.\n");
    return 0;
}

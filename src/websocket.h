#pragma once

#include <netinet/in.h>

#include "acl.h"

#define BUFSIZE 65536
#define DBUFSIZE (BUFSIZE * 3) / 4 - 20

#define SERVER_HANDSHAKE_HIXIE "HTTP/1.1 101 Web Socket Protocol Handshake\r\n\
Upgrade: WebSocket\r\n\
Connection: Upgrade\r\n\
%sWebSocket-Origin: %s\r\n\
%sWebSocket-Location: %s://%s%s\r\n\
%sWebSocket-Protocol: %s\r\n\
\r\n%s"

#define SERVER_HANDSHAKE_HYBI "HTTP/1.1 101 Switching Protocols\r\n\
Upgrade: websocket\r\n\
Connection: Upgrade\r\n\
Sec-WebSocket-Accept: %s\r\n\
Sec-WebSocket-Protocol: %s\r\n\
\r\n"

#define SERVER_HANDSHAKE_HYBI_NO_PROTOCOL "HTTP/1.1 101 Switching Protocols\r\n\
Upgrade: websocket\r\n\
Connection: Upgrade\r\n\
Sec-WebSocket-Accept: %s\r\n\
\r\n"


#define HYBI_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

#define HYBI10_ACCEPTHDRLEN 29

#define HIXIE_MD5_DIGEST_LENGTH 16

#define POLICY_RESPONSE "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>\n"

#define OPCODE_TEXT    0x01
#define OPCODE_BINARY  0x02

typedef struct {
    char path[1024+1];
    char host[1024+1];
    char origin[1024+1];
    char version[1024+1];
    char connection[1024+1];
    char protocols[1024+1];
    char x_forwarded_for[1024+1];
    char key1[1024+1];
    char key2[1024+1];
    char key3[8+1];
} headers_t;

typedef struct {
    int        sockfd;
    int        hixie;
    int        hybi;
    int        opcode;
    headers_t *headers;
    char      *cin_buf;
    char      *cout_buf;
    char      *tin_buf;
    char      *tout_buf;

    char target_host[256];
    int target_port;
} ws_ctx_t;

typedef struct {
    int verbose;
    char listen_host[256];
    int listen_port;
    void (*handler)(ws_ctx_t*);
    int handler_id;
    char *cert;
    char *key;
    int ssl_only;
    int daemon;
    int run_once;
    struct sockaddr_in ws_client_addr;
    struct sockaddr_in tcp_server_addr;
    char ws_endpoint[256], tcp_endpoint[256];

    struct acl_t *src_whitelist;
    struct acl_t *dst_whitelist;
} settings_t;


int resolve_host(struct in_addr *sin_addr, const char *hostname);

ssize_t ws_recv(ws_ctx_t *ctx, void *buf, size_t len);

ssize_t ws_send(ws_ctx_t *ctx, const void *buf, size_t len);
ssize_t ws_send_ping(ws_ctx_t *ctx);


#define gen_handler_msg(stream, ...) \
if (! settings.daemon) { \
fprintf(stream, "%d: ", settings.handler_id); \
fprintf(stream, __VA_ARGS__); \
}

#define handler_msg(...) gen_handler_msg(stdout, __VA_ARGS__);
#define handler_emsg(...) gen_handler_msg(stderr, __VA_ARGS__);

#define connection_msg(fmt, args...) \
    fprintf(stderr, "%d: %s => %s " fmt, \
        settings.handler_id, \
        settings.ws_endpoint, \
        settings.tcp_endpoint, \
        ##args); \

void traffic(const char * token);

int encode_hixie(u_char const *src, size_t srclength,
                 char *target, size_t targsize);
int decode_hixie(char *src, size_t srclength,
                 u_char *target, size_t targsize,
                 unsigned int *opcode, unsigned int *left);
int encode_hybi(u_char const *src, size_t srclength,
                char *target, size_t targsize, unsigned int opcode);
int decode_hybi(unsigned char *src, size_t srclength,
                u_char *target, size_t targsize,
                unsigned int *opcode, unsigned int *left);

void start_server();

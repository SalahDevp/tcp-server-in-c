#include <arpa/inet.h>  // IP address conversion stuff
#include <netinet/in.h> // Internet-specific socket address structures
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h> // Core socket functions and data types
#include <sys/types.h>  // Definitions of data types used in system calls
#include <unistd.h>     // Miscellaneous POSIX definitions

const int MAX_PENDING_CONN = 10;
const int PORT = 8000;
const char ADDRESS[] = "127.0.0.1";

void recv_msg(int sockfd, char **buf, uint16_t *msg_ln);

int main(int argc, char *argv[]) {

  fd_set readfds;

  struct sockaddr_in sr_addr, cl_addr;
  unsigned int cl_len = 0;
  bzero(&sr_addr, sizeof(sr_addr));
  bzero(&cl_addr, sizeof(cl_addr));

  sr_addr.sin_family = AF_INET;
  sr_addr.sin_port = htons(PORT);
  inet_aton(ADDRESS, &sr_addr.sin_addr);

  char *recv_buf = NULL;
  uint16_t recv_msg_ln = 0;

  struct timeval tv;
  // wait for 5 secs
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  int server_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (bind(server_sd, (struct sockaddr *)&sr_addr, sizeof(sr_addr)) < 0) {
    printf("error binding socket\n");
    return 1;
  }

  listen(server_sd, MAX_PENDING_CONN);
  printf("server listening on port: %d\n", PORT);

  while (1) {
    int clsfd = accept(server_sd, (struct sockaddr *)&cl_addr, &cl_len);
    if (clsfd < 0) {
      printf("error on accept\n");
      exit(1);
    }

    /* FD_ZERO(&readfds);

    FD_SET(server_sd, &readfds);

    select(server_sd + 1, &readfds, NULL, NULL, &tv); */

    recv_msg(clsfd, &recv_buf, &recv_msg_ln);
    printf("received message: %s\n", recv_buf);
    /* if (FD_ISSET(server_sd, &readfds)) {
    }
    FD_CLR(server_sd, &readfds); */
  }

  return 0;
}

void recv_msg(int sockfd, char **buf, uint16_t *msg_ln) {
  // the first 2bytes are the msg length
  recv(sockfd, msg_ln, sizeof(*msg_ln), 0);

  // allocate buffer to receive message
  *buf = (char *)malloc((*msg_ln) + 1);
  if (*buf == NULL) {
    printf("error allocating buffer\n");
    exit(1);
  }

  recv(sockfd, *buf, *msg_ln, 0);
  *(buf + *msg_ln) = 0;
}

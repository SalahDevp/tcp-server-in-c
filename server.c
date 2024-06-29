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

  struct sockaddr_in sr_addr, cl_addr;
  unsigned int cl_len = 0;
  bzero(&sr_addr, sizeof(sr_addr));
  bzero(&cl_addr, sizeof(cl_addr));

  sr_addr.sin_family = AF_INET;
  sr_addr.sin_port = htons(PORT);
  inet_aton(ADDRESS, &sr_addr.sin_addr);

  char *recv_buf = NULL;
  uint16_t recv_msg_ln = 0;

  int server_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (bind(server_sd, (struct sockaddr *)&sr_addr, sizeof(sr_addr)) < 0) {
    printf("error binding socket\n");
    return 1;
  }

  listen(server_sd, MAX_PENDING_CONN);
  printf("server listening on port: %d\n", PORT);

  // automatically clean child processes after they exit
  signal(SIGCHLD, SIG_IGN);

  while (1) {
    int clsfd = accept(server_sd, (struct sockaddr *)&cl_addr, &cl_len);
    int pid = fork();

    if (pid < 0) {
      printf("error creating fork\n");
      exit(1);
    } else if (pid == 0) { // only child process handles connection

      if (clsfd < 0) {
        printf("error on accept\n");
        exit(1);
      }

      recv_msg(clsfd, &recv_buf, &recv_msg_ln);
      printf("received message: %s\n", recv_buf);
      free(recv_buf);
      close(clsfd);
      close(server_sd);
      exit(0);
    } else {
      close(clsfd);
    }
  }

  return 0;
}

void recv_msg(int sockfd, char **buf, uint16_t *msg_ln) {
  uint16_t received_ln = 0;
  // the first 2bytes are the msg length
  read(sockfd, msg_ln, sizeof(*msg_ln));

  // allocate buffer to receive message
  *buf = (char *)malloc((*msg_ln) + 1);
  if (*buf == NULL) {
    printf("error allocating buffer\n");
    exit(1);
  }

  while (received_ln < *msg_ln) {
    int r = read(sockfd, *buf + received_ln, *msg_ln - received_ln);
    if (r < 0) {
      printf("error reading message\n");
      exit(1);
    } else if (r == 0) {
      break;
    } else {
      received_ln += r;
    }
  }
  *msg_ln = received_ln;
  (*buf)[*msg_ln] = 0;
}

#include <arpa/inet.h>  // IP address conversion stuff
#include <netinet/in.h> // Internet-specific socket address structures
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h> // Core socket functions and data types
#include <sys/types.h>  // Definitions of data types used in system calls
#include <unistd.h>     // Miscellaneous POSIX definitions

const int SERVER_PORT = 8000;
const char SERVER_ADDRESS[] = "127.0.0.1";

int main(int argc, char *argv[]) {
  int clsfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in srv_addr;
  bzero(&srv_addr, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(SERVER_PORT);
  inet_aton(SERVER_ADDRESS, &srv_addr.sin_addr);

  uint16_t msg_ln;
  char *buf = NULL;

  if (connect(clsfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
    printf("error connecting to server\n");
    exit(1);
  }

  printf("enter the message length\n");
  scanf("%hu", &msg_ln);
  fflush(stdin);

  int buf_size = msg_ln + sizeof(msg_ln);
  buf = (char *)malloc(buf_size + 1);
  if (buf == NULL) {
    printf("memory error\n");
    exit(1);
  }
  *(uint16_t *)buf = msg_ln;
  printf("enter your message:\n");
  fgets(buf + sizeof(msg_ln), msg_ln + 1, stdin);

  // send msg to server
  if (send(clsfd, buf, buf_size, 0) != buf_size) {
    printf("couldn't send message\n");
    exit(1);
  }

  printf("message: %s sent successfuly to %s:%d\n", buf + sizeof(msg_ln),
         SERVER_ADDRESS, SERVER_PORT);
  close(clsfd);
  return 0;
}

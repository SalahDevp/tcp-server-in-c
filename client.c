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

  // set server address
  struct sockaddr_in srv_addr;
  bzero(&srv_addr, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(SERVER_PORT);
  inet_aton(SERVER_ADDRESS, &srv_addr.sin_addr);

  uint16_t msg_ln;
  char *buf = NULL;

  // connect to server
  if (connect(clsfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
    printf("error connecting to server\n");
    exit(1);
  }

  printf("enter the message length\n");
  scanf("%hu", &msg_ln);
  fflush(stdin);
  // send length
  write(clsfd, &msg_ln, sizeof(msg_ln));

  // allocate message buffer
  buf = (char *)malloc(msg_ln + 1);
  if (buf == NULL) {
    printf("memory error\n");
    exit(1);
  }

  int sent_ln = 0;
  while (sent_ln < msg_ln) {
    printf("enter your message:\n");
    fgets(buf, msg_ln - sent_ln + 1, stdin);
    int input_ln = strlen(buf);
    // remove \n
    if (buf[input_ln - 1] == '\n') {
      buf[--input_ln] = '\0';
    }
    // send msg to server
    if (write(clsfd, buf, input_ln) != input_ln) {
      printf("couldn't send message\n");
      exit(1);
    }
    printf("message: %s sent successfuly to %s:%d\n", buf, SERVER_ADDRESS,
           SERVER_PORT);
    sent_ln += input_ln;
  }

  close(clsfd);
  return 0;
}

/**
 * @file simple_message_server.c
 * Distributed Systems - Implementation of a simple TCP message server
 *
 * @author Christian Moedlhammer,ic14b027
 * @author Harald Partmann,ic15b039
 *
 * @date 2016-12-12
 *
 * @version 0.1
 *
 */


#include <sys/types.h>     	
/* POSIX.1-2001 does not require the inclusion of <sys/types.h>, and this header file is not required on Linux.
 portable applications are probably wise to include it. */
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MY_SOCK_PATH "/home/ic15b039/TCPProject/TCPServer"
#define LISTEN_BACKLOG 50
#define SMSNAME "simple_message_server_logic"
#define SMSPATH "/usr/local/bin/simple_message_server_logic"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int sock_fd, conn_fd;
 //   struct sockaddr_un my_addr, peer_addr;

    socklen_t peer_addr_size;

/*    // or call getaddrinfo() to fill out the struct addrinfo
    // but you need to peer inside the structs to get the values
    struct addrinfo {
    	int ai_family;
    	int ai_socktype;
    	int ai_protocol;
    	// struct sockaddr *ai_addr; // struct sockaddr_in
    };
   */
    struct addrinfo hints; /* struct for parameters for getaddrinfo*/
    //struct addrinfo hints, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    // bind the IP of the host it is running on (Beef p. 19)
    //hints.ai_flags = AI_PASSIVE; // fill in my IP for me

  /*  // in for Internet - IPv4 only ! - (sockaddr_in6 for IPv6, or sockaddr as general)
    struct sockaddr_in {
    	short int sin_family; // Address family, AF_INET
    	unsigned short int sin_port; // Port number
    	struct in_addr sin_addr; // Internet address
    	unsigned char sin_zero[8]; // Same size as struct sockaddr
    };
*/
    // struct sockaddr_in sa; // IPv4
    struct sockaddr_in my_addr;

  //  sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	sock_fd = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
	// with pointer res: sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_fd == -1)
        handle_error("socket");

 //   memset(&my_addr, 0, sizeof(struct sockaddr_un));
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
                        /* Clear structure */

 //   my_addr.sun_family = AF_UNIX;
//    my_addr.sa_family = AF_INET;
    my_addr.sin_family = AF_INET;
 //   strncpy(my_addr.sun_path, MY_SOCK_PATH, sizeof(my_addr.sun_path) - 1);
  ? ?? strncpy(my_addr.sin_path, MY_SOCK_PATH, sizeof(my_addr.sun_path) - 1);

  // New way: bind(sockfd, res->ai_addr, res->ai_addrlen);
  // Old way:

 //   if (bind(sock_fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1)
  if (bind(sock_fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1)
        handle_error("bind");

    if (listen(sock_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen");
	   
	   /* MISSING: errorhandling according listen(2) manpage */

    /* Now we can accept incoming connections one
       at a time using accept(2) */
	   
// loop until shutdown() from client

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
    }
    printf("server: waiting for connections...\n");
// while(1)
do {
    peer_addr_size = sizeof(struct sockaddr_un);
    conn_fd = accept(sock_fd, (struct sockaddr *) &peer_addr,
                 &peer_addr_size);
    if (conn_fd == -1)
        handle_error("accept");
	   
    /* Code to deal with incoming connection(s)... */

	   pid_t conn_pid;
	   char buf;
	   
	conn_pid = fork();
    if (conn_pid == -1) {
        perror("fork");
        // gehört nicht: close(sock_fd); ?? // laut Beejs guide
        close(conn_fd);
        exit(EXIT_FAILURE);
    }

    if (conn_pid == 0) {
	
		printf( "printout from the child process\n" );
		
		// close listening socket process in child

		close(sock_fd);

		// redirect stdin and stdout from forked child daemon to connected socket
		dup2(conn_fd, STDIN_FILENO);
		dup2(conn_fd, STDOUT_FILENO);

		close(conn_fd);

		// executes simple_message_server_logic
		(void) execl(SMSPATH, SMSNAME, (char*)NULL);

		// implemented by sms logic:
		// exec read(conn_fd, &buf, 1);

		
		// implemented by sms logic:
		// exec write(conn_fd, &buf, 1);
	
		
        // execve executes program - example:
		/*       char *newargv[] = { NULL, "hello", "world", NULL };
               char *newenviron[] = { NULL };

               assert(argc == 2);  /* argv[1] identifies
                                      program to exec */
               newargv[0] = argv[1];


			   execve(argv[1], newargv, newenviron);
			   */

		// end connection
        write(conn_fd, "\n", 1);
        close(conn_fd);
        _exit(EXIT_SUCCESS);

    } else {            /* Parent writes argv[1] to pipe */
		printf( "printout from the parent process\n" );	
        close(conn_fd);          /* Close unused read end */

	//  close(pipefd[0]);          /* Close unused read end */
     /*   write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);          /* Reader will see EOF */
		*/
		
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    }
	
// end of do loop
} while(1); // SMSLOGIC: (NO_SHUTDOWN); /* no shutdown received from client */
	
    /* When no longer required, the socket pathname, MY_SOCK_PATH
       should be deleted using unlink(2) or remove(3) */
	   
	   unlink(MY_SOCK_PATH);
	   
	   return 0;
}


// MISSING: dup2(2), close(2)

/* Server Pseudo flowdiagram according Fibich
socket(2) -> AF_INET, SOCK_STREAM, 0
bind(2) -> man ip(7): struct sockaddr (sa_family, sa_data), struct_sockaddr_in (sa_family=AF_INET, sin_port=htons(port), sin_addr=INADDR_ANY)
listen(2)  - setsockopt(SOL_REUSEADDR,1)
loop:
	accept(2)
	fork(2)
	exec(2)
	close(2)
loop end

dup2(2)
close(2)
*/

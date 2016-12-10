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

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    int sock_fd, conn_fd;
    struct sockaddr_un my_addr, peer_addr;
    socklen_t peer_addr_size;

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1)
        handle_error("socket");

    memset(&my_addr, 0, sizeof(struct sockaddr_un));
                        /* Clear structure */
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, MY_SOCK_PATH,
            sizeof(my_addr.sun_path) - 1);

    if (bind(sock_fd, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_un)) == -1)
        handle_error("bind");

    if (listen(sock_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen");
	   
	   /* MISSING: errorhandling according listen(2) manpage */

    /* Now we can accept incoming connections one
       at a time using accept(2) */
	   
// loop until shutdown() from client

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
        exit(EXIT_FAILURE);
    }

    if (conn_pid == 0) {
	
		printf( "printout from the child process\n" );
		exec read(conn_fd, &buf, 1);
		
		// process request - MISSING
		
		exec write(conn_fd, &buf, 1);
	
		
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
} while (NO_SHUTDOWN); /* no shutdown received from client */
	
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
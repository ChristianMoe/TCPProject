/**
 * @file simple_message_client.c
 * Distributed Systems - Implementation of a simple message client
 *
 * @author Christian Mödlhammer,ic14b027
 * @details This program implements a simple message client using a Library libsimple_message_client_commandline_handling.a for parsing the command line input
 * @date 2016-01-12
 *
 * @version 0.1
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h> /* Include Standard Input/Output Features */
#include <stdlib.h> /* Include Standard Library Functions */
#include <string.h> /* Include String Functions */

#include "simple_message_client_commandline_handling.h" /* Include external Parser Functions */


static void usageinfo(FILE *outputdevice, const char *filename, int suc_or_fail) {

	fprintf(outputdevice, "%s: Usage will be implemented \n", filename);
	exit(suc_or_fail);

}

int main(int argc, const char *argv[]) {

	  const char *server = NULL;
	  const char *port = NULL;
	  const char *user = NULL;
	  const char *message = NULL;
	  const char *imgurl = NULL;
	  int verbose = 0;

	  smc_parsecommandline(argc, argv, &usageinfo, &server, &port, &user, &message, &imgurl, &verbose);

	  fprintf(stdout, "Server:%s \n", server);
	  fprintf(stdout, "Port:%s \n", port);
	  fprintf(stdout, "User:%s \n", user);
	  fprintf(stdout, "Message:%s \n", message);


	return EXIT_SUCCESS;

}




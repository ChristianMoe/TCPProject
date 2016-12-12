##
## @file Makefile
## Distributed Systems - simple_message-client Makefile
## 
## @author Christian Moedlhammer,ic14b027
## @author Harald Partmann,ic15b039
##
## @version 0.1
##

    
##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-Wall -Werror -Wextra -Wstrict-prototypes -pedantic -fno-common -O3 -g -std=gnu11
LDFLAGS=-lsimple_message_client_commandline_handling

##OBJ1=simple_message_client.o
##BIN1=simple_message_client
OBJ2=har_sms.o
BIN2=har_sms



##
## ----------------------------------------------------------------- rules --
##

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: $(BIN2) ## $(BIN1)

##$(BIN1): $(OBJ1)
##	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

$(BIN2): $(OBJ2)
	$(CC) $(CFLAGS) -o $@ $^ 

.PHONY: cleanall
cleanall: clean clean_r

.PHONY: clean
clean:
##	rm -rf $(BIN1) $(OBJ1) 
	rm -rf $(BIN2) $(OBJ2)

.PHONY: clean_r
clean_r:
	rm -rf *.html *.png
	

##
## ---------------------------------------------------------- documentation --
##

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##

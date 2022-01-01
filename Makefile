CC = gcc
CFLAGS = -Wall -pedantic

PHONY = server_build client_build test run clean c_clean s_clean S_BIN C_BIN S_OBJ C_OBJ

#SERVER DIRECTORIES
S_DIR=server
S_OBJDIR=$(S_DIR)/obj
S_BINDIR=$(S_DIR)/bin
S_HEADERS=$(S_DIR)/src/headers
S_SOURCES=$(S_DIR)/src

S_EXE=$(S_BINDIR)/ram-storage-server

#CLIENT DIRECTORIES
C_DIR=client
C_OBJDIR= $(C_DIR)/obj
C_BINDIR= $(C_DIR)/bin
C_HEADERS= $(C_DIR)/src/headers
C_SOURCES=$(C_DIR)/src

C_EXE=$(C_BINDIR)/ram-storage-client

vpath %.c src
vpath %.h src

s_objects = $(addprefix $(S_OBJDIR)/, *.o )
s_headers = $(addprefix $(S_HEADERS)/, *.h)
s_sources = $(addprefix $(S_SOURCES)/, *.c)

c_objects = $(addprefix $(C_OBJDIR)/, *.o )
c_headers = $(addprefix $(C_HEADERS)/, *.h)
c_sources = $(addprefix $(C_SOURCES)/, *.c)

all: client_build #server

server_build: $(s_objects) | S_BIN
	$(CC) $^ -o $(S_EXE)

client_build: $(c_objects) | C_BIN
	$(CC) $^ -o $(C_EXE)

$(S_OBJDIR)/%.o: $(s_sources) $(s_headers) | S_OBJ
	$(CC) $(CFLAGS) -c $< -o $@

$(C_OBJDIR)/%.o: $(c_sources) $(c_headers) | C_OBJ
	$(CC) $(CFLAGS) -c $< -o $@

clean: s_clean c_clean

s_clean:
	rm -rf $(S_BINDIR) $(S_OBJDIR)

c_clean:
	rm -rf $(C_BINDIR) $(C_OBJDIR)

run_client:
	./client/bin/ram-storage-client
	
S_BIN:
	mkdir -p $(S_BINDIR)

S_OBJ:
	mkdir -p $(S_OBJDIR)

C_BIN:
	mkdir -p client/bin

C_OBJ:
	mkdir -p client/obj


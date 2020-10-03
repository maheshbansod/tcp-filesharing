# tcp-filesharing

This repository contains two files: server.c and client.c which can be used to retrieve the file from a server to client through TCP.

## Compiling
You can compile the files individually:
```bash
cc -o <output executable filename> <source file name>
```
i.e.
```bash
cc -o server server.c
cc -o client client.c
```

## Usage

### Testing locally
Place both the executable files in different folders(or run them with different PWDs) so that the client executable doesn't try to write on an already open file.
Start the server first:
```bash
./server
```
Enter the filename to copy as a command line argument to the client program:
```bash
./client path/to/file
```

### On a remote server
Before compiling, change the IP address in the source client file to the server IP address.
Place the client executable on your machine and the server executable on the server machine.
Have the server running:
```bash
./server
```

Execute the client executable file on your machine with the file name as the command line argument.
```bash
./client path/to/file
```

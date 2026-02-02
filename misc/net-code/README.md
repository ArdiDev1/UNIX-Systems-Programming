# myftp

This is an example of customized FTP protocol. 

The commands supported are

	* ls
	* get filename	 Download a file. Save it in filename.download
	* exit

server listens on TCP port 3103. server uses threads to handle clients. 

In one terminal, run server: 

```
./server
```

In one terminal, run client:

```
./client localhost
```

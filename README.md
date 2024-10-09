# File Download/Transfer Application in C

A simple TCP client-server program using socket programming in C. In this program, on the client side,
the user enters a filename to the terminal, the client retreives the filename and sends it to the server.
When the server receives the filename, it will try to open the requested file and if found, the client will
write back the file contents into a text file. 


![Screenshot_1](https://github.com/user-attachments/assets/d35d74c0-2619-46ff-8b2d-3dcd711c67a3)

Use the Makefile to compile both the echo_server and echo_client code 
- Start an "Echo" server with `./echo_server [port_number]`
- Start an "Echo" client with `./echo_client [server_IP_address] [port_number]`
- Enter the requested filename:
  - If file does not exist or is too short, an error is outputted and the TCP connection is terminated.  
  - if file opening is successful, the server will send the file to the client and the client will write the contents of the file essentially downloading it.

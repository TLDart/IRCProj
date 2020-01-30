# Introduction to Networks Project
  C based project that emphasises the use of sockets to create Intercomputer communications. In This project:
  * UDP Sockets
  * TCP Sockets
  * Proxy 
  * Listing files from a remote directory
  * Showing Active Connections in the proxy
  
 ## Lore
 The goal of the project is to create a Client, proxy, Server architecture where both UDP and TCP connections are possible and the following features should work:
 * Client can list Files in the server
 * Client can download files from the server: both in TCP or UDP, with or without encryption
 * Proxy Should be able to list active connections
 * Proxy save a the last file transmited
 * Proxy should Implement a loss system where X% of the bytes/packets are lost
 
 ##Flow
 In the client:
 * After the client validates the connection with the proxy/server,he will be able to type commands. Those commands are LIST and DOWNLOAD.
 In the server:
 * When the server starts it created both a TCP and a UDP socket, where it can receive both connections from the proxy or client.
 * When a command is received a process is created to correctly handle the resquest, redirecting it to the correct slot (TCP or UDP);
 In the Proxy:
 * Creates both a TCP and UDP socket on startup;
 * After both connection between proxy<->server and client<->proxy are established, for each new received connection a thread is created to handle said request.
  
 ## Known Bugs
  * Due to a poor implementation of the UDP system it is not possible to support multiple UDP connections at the same time
  
 ## Not Implemented
   Unfortunately time was rather short to finish and we left some things behind, those include:
   * Simple encryption and authentication scheme using libsodium
   * Allowing UDP Connections to have losses
   * Caching the last file transfered in the proxy
   * Showing encryted data in the proxy

# Watch
Online video player synchronizer using Qt/C++ and VLC.

Synchronizes players which run local videos.

## Features
+ Clients-Server application to synchronize timestamps between local video players.
+ Features a chat as well.

## Requirements
+ VLC media player on linux

Tested on Ubuntu 18.04 and Windows 10.

## Before compiling
Run *certs/create_client_certificate.sh* and *certs/create_server_certificate.sh* to generate SSL certificates.
The client **does not** check the server certificate. It does not need to stick to one IP. For portability purposes, it is only used for encryption. 
The server **does** check the client certificate, this way only this application can connect to the server.

## How to use
**Server side :** The server uses port 3000 (modifiable in messages/constants).

Set up your network (NAT/firewall) and let it run on your machine.

**Client side :** Connect to the server IP and open a local video or URL. Opening a URL will share it automatically to the other users.

The server will synchronize the timestamps and play/pauses of all client applications.

The local videos must be present on disk for each user (only timestamp info is sent).

On Windows, allow the app to communicate in your firewall.

*TODO : stream from one PC to the others*


# Watch
Online video player synchronizer using Qt/C++ and VLC.

Synchronizes players which run local videos.

## Features
+ Clients-Server application to synchronize timestamps between local video players.
+ Features a chat as well.

## Requirements
+ VLC media player
+ Qt5 network, core and gui dynamic libraries (built-in on most Linux distros)

Tested on Ubuntu 18.04.

## How to use
**Server side :** The server uses port 3000 (modifiable in messages/constants).

Set up your network (NAT/firewall) and let it run on your machine.

**Client side :** Connect to the server IP and open a local video.

The server will synchronize the timestamps and play/pauses of all client applications.

The videos must be present on disk for each user (only timestamp info is sent).

*TODO : stream from one PC to the others*


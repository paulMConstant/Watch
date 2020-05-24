#-------------------------------------------------
#
# Project created by QtCreator 2020-05-07T14:45:03
#
#-------------------------------------------------

TEMPLATE = subdirs

SUBDIRS = \
        Messages \
        Client \
        Server

Messages.subdir = src/Messages
Client.subdir   = src/Client
Server.subdir   = src/Server

app.depends = Messages

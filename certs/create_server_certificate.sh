#!/bin/bash
openssl req -x509 -nodes -days 1000 -newkey rsa:2048 -keyout server_pkey.pem -out server_cert.pem

#!/usr/bin/env bash

mkdir -p /etc/my-envoy-certs
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/my-envoy-certs/envoy1.key \
    -out /etc/my-envoy-certs/envoy1.crt -subj "/CN=envoy1.local"

openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/my-envoy-certs/envoy2.key \
    -out /etc/my-envoy-certs/envoy2.crt -subj "/CN=envoy2.local"
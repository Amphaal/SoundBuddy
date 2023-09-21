#! /bin/bash

# stops if any command fails
set -e

# create recipient folder if it do not exist
mkdir -p ../certs

KEY_PATH="../certs/key.pem"
CERT_PATH="../certs/cert.crt"
PFX_PATH="../certs/all.pfx"

#
# generate both certificates and private key
#

#
if [[ ! -f "$KEY_PATH" || ! -f "$CERT_PATH" ]]; then
    echo ">> Creating new certificate && private key !"
    openssl genrsa -out $KEY_PATH 2048
    openssl req -x509 -new -nodes -key $KEY_PATH -sha256 -days 10000 -out $CERT_PATH
fi

#
if [[ ! -f "$PFX_PATH" ]]; then
    openssl pkcs12 -export -out $PFX_PATH -inkey $KEY_PATH -in $CERT_PATH
fi


echo ">> Done."
#!/bin/bash
# ZTFCP - Zero Trust mTLS Certificate Generation Script
# DO NOT UPLOAD ACTUAL .KEY FILES TO GITHUB

echo "Generating Zero Trust PKI Certificates..."

# 1. Generate Certificate Authority (CA)
openssl req -new -x509 -days 3650 -extensions v3_ca -keyout ca.key -out ca.crt -subj "/C=IN/ST=MH/L=Pune/O=ZTFCP/CN=Factory_CA"

# 2. Generate Jetson Broker Server Key and CSR
openssl genrsa -out server.key 2048
openssl req -out server.csr -key server.key -new -subj "/C=IN/ST=MH/L=Pune/O=ZTFCP/CN=10.200.155.193"

# 3. Sign the Server Certificate
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 3650

# 4. Generate ESP32 Client Key and CSR
openssl genrsa -out client.key 2048
openssl req -out client.csr -key client.key -new -subj "/C=IN/ST=MH/L=Pune/O=ZTFCP/CN=ESP32_Edge_Node"

# 5. Sign the Client Certificate
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 3650

echo "Certificates generated successfully. Remember to run 'chmod 644' on necessary keys."

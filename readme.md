# behavior client
For the Raspberry Pi with gcc/g++ version 4.9.x, there is no definition of nullptr.

Also, for Raspberry Pi with gcc/g++ version 4.9.x, manually give the username in function `get_pid` is a must, otherwise pid value cannot be got. You can give the username directly in the function `int get_pid(const char* process_name, const char* user)` of `utils.cpp`. 

add `-lpthread` whe using g++ to compile and build

the TLS uses openssl-1.1.1q

## arm64 (test on jetsonTX2)
g++ main.cpp utils.cpp /home/$USER/openssl/lib/libssl.a  /home/$USER/openssl/lib/libcrypto.a -I/home/$USER/openssl/include -lpthread -ldl

## Raspberry Pi pi_armv7l_32
g++ main.cpp utils.cpp /home/$USER/openssl/lib/libssl.a  /home/$USER/openssl/lib/libcrypto.a -I/home/$USER/openssl/include -lpthread -ldl -DRPI

(luckily, this works for g++ version 4.9.2)

## debug
1. socket error too many open files 24:
https://stackoverflow.com/questions/880557/socket-accept-too-many-open-files
`ulimit -n 16384`

## how to generate keys and certificates
### self-signed CA certificate
```
# generate root certificate private key (pem file)
openssl genrsa -out cakey.pem 2048       
# generate root certificate issuance application file (csr file)
openssl req -new -key cakey.pem -out ca.csr -subj "/C=EE/ST=myprovince/L=mycity/O=myorganization/OU=mygroup/CN=myCA" 
# self-signed root certificate (cer file)
openssl x509 -req -days 3650 -sha1 -extensions v3_ca -signkey cakey.pem -in ca.csr -out  cacert.pem     
```
### server private key and certificate
```
# generate server private key
openssl genrsa -out serverkey.pem 2048 
# generate certificate request file
openssl req -new -key serverkey.pem -out server.csr -subj "/C=EE/ST=myprovince/L=mycity/O=myorganization/OU=mygroup/CN=myServer"
# use the root certificate to issue the server certificate
openssl x509 -req -days 3650 -sha1 -extensions v3_req -CA cacert.pem -CAkey cakey.pem -CAserial serverca.srl -CAcreateserial -in server.csr -out servercert.pem
# use the CA certificate to verify the server-side certificate
openssl verify -CAfile cacert.pem  servercert.pem
```
### client private key and certificate
```
# generate client private key                                                                                                           
openssl genrsa -out clientkey.pem 2048     
# generate certificate request file       
openssl req -new -key clientkey.pem -out client.csr -subj "/C=EE/ST=myprovince/L=mycity/O=myorganization/OU=mygroup/CN=myClient"
# sign client certificate with root certificate
openssl x509 -req -days 3650 -sha1 -extensions v3_req -CA cacert.pem -CAkey cakey.pem  -CAserial serverca.srl -in client.csr -out clientcert.pem
# verify client certificate with CA certificate   
openssl verify -CAfile cacert.pem  clientcert.pem
```

# behavior client

For the Raspberry Pi with gcc/g++ version 4.9.x, there is no definition of nullptr.

Also, for Raspberry Pi with gcc/g++ version 4.9.x, manually give the username in function `get_pid` is a must, otherwise pid value cannot be got. 

add `-lpthread` whe using g++ to compile and build

## debug

1. socket error too many open files 24:
https://stackoverflow.com/questions/880557/socket-accept-too-many-open-files
`ulimit -n 16384`

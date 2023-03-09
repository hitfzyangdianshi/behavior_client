# DPI client
For the Raspberry Pi with gcc/g++ version 4.9.x, there is no definition of nullptr.

Also, for Raspberry Pi with gcc/g++ version 4.9.x, manually give the username in function `get_pid` is a must, otherwise pid value cannot be got. You can give the username directly in the function `int get_pid(const char* process_name, const char* user)` of `utils.cpp`. 

add `-lpthread` whe using g++ to compile and build


## arm64 (test on jetsonTX2)
g++ main.cpp utils.cpp  -lpthread -ldl

## Raspberry Pi pi_armv7l_32
g++ main.cpp utils.cpp  -lpthread -ldl -DRPI

(luckily, this works for g++ version 4.9.2)

## debug
1. socket error too many open files 24:
https://stackoverflow.com/questions/880557/socket-accept-too-many-open-files
`ulimit -n 16384`


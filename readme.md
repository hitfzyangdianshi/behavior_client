For the Raspberry Pi with gcc/g++ version 4.9.x, there is no definition of nullptr.

Also, for Raspberry Pi with gcc/g++ version 4.9.x, manually give the username in function `get_pid` is a much, otherwise pid value cannot be got. 

add `-lpthread` whe using g++ to compile and build

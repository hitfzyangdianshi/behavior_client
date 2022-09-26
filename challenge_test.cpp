//
// Created by qwertyu on 9/21/22.
//

#include<cstdio>
#include<cstdlib>
using namespace std;


//challenge test: a+b==c???
int main(int argc, char** argv)
{
    if(argc!=3)
    {
        perror("argc is not 3");
        exit (400);
    }

    int a,b,c;
    a=atoi(argv[1]);
    b=atoi(argv[2]);
    c=a+b;
    printf("%d\n",c);
    return c;
}
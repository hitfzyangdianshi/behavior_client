//
// Created by qwertyu on 9/23/22.
//

#include "cpu_utilization_1.h"

using namespace std;

/* Return resource usage information on process indicated by WHO
   and put it in *USAGE.  Returns 0 for success, -1 for failure.  */
//extern int getrusage (__rusage_who_t __who, struct rusage *__usage) __THROW;



int ger_cpu_utilizatoin_withpid_save(__rusage_who_t pid, FILE *output_cpu_values){
    int getrusage_result;
    getrusage_result=getrusage(pid,&usage_value);






}


//
// Created by qwertyu on 9/21/22.
//

#ifndef UNTITLED_MEMORY_UTILIZATION_H
#define UNTITLED_MEMORY_UTILIZATION_H

#include <cstdlib>
#include <cstdio>

class memory_utilization {

};

typedef struct mem_info_
{
    char mem_total_name[20];
    unsigned long mem_total_;
    char mem_total_unit[4];

    char mem_free_name[20];
    unsigned long mem_free_;
    char mem_free_unit[4];

    char mem_available_name[20];
    unsigned long mem_available_;
    char mem_available_unit[4];

}mem_info_t;

double get_memory_utilization();

#endif //UNTITLED_MEMORY_UTILIZATION_H

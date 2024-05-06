//
// Created by Marcel Beyer on 13.02.24.
//
#include "settings.h"

#ifndef WORK_UTILS_H
#define WORK_UTILS_H

int get_min_index(volatile float fitness[POPSIZE]);
int get_max_index(volatile float fitness[POPSIZE]);
float randf_range(float min, float max);
int randi_range(int min, int max);
float randf_normal(float m, float s);
float randf_cauchy(float m, float s);
void iota(int* array, int size, int start);

#endif //WORK_UTILS_H

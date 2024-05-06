//
// Created by Marcel Beyer on 13.02.24.
//

#include "utils.h"
#include "settings.h"
#include "math.h"

#include <stdlib.h>

#include <s2app.h>
#include <spinn_log.h>

int get_min_index(volatile float fitness[POPSIZE]){
    // Find index of individual with minimum fitness
    int min_index = 0;
    float min_fitness = fitness[0];
    for (int i = 1; i < POPSIZE; ++i) {
        if (fitness[i] < min_fitness) {
            min_index = i;
            min_fitness = fitness[i];
        }
    }
    return min_index;
}

int get_max_index(volatile float fitness[POPSIZE]) {
    // Find index of individual with maximum fitness
    int max_index = 0;
    float max_fitness = fitness[0];
    for (int i = 1; i < POPSIZE; ++i) {
        if (fitness[i] > max_fitness) {
            max_index = i;
            max_fitness = fitness[i];
        }
    }
    return max_index;
}

float randf_range(float min, float max){
    return (float)rand()/(float)RAND_MAX*(max-min)+min;
}

int randi_range(int min, int max){
    int r = (rand() % (max - min + 1)) + min;
    return r;
}

float randf_normal(float m, float s){
    float r1 = (float)rand()/(float)RAND_MAX;
    float r2 = (float)rand()/(float)RAND_MAX;
    float standard_norm_rand = sqrtf(-2 * log(r1)) * cosf(2*M_PI*r2);    // Box–Muller transform
    return m + s * standard_norm_rand;
}

float randf_cauchy(float m, float s){
    float r1 = (float)rand()/(float)RAND_MAX;
    float standard_cauchy = tanf(M_PI*(r1-0.5));
    return m + s * standard_cauchy;
}

void iota(int* array, int size, int start){
    for(int i = 0; i < size; i++){
        array[i] = start;
        start++;
    }
}
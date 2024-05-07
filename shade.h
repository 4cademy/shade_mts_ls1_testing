//
// Created by beyer on 13.02.24.
//

#include "settings.h"

#ifndef WORK_SHADE_H
#define WORK_SHADE_H

void initialize_shade();
void shade(volatile float population[POPSIZE][DIM], volatile float fitness[POPSIZE], volatile float best_solution[DIM], volatile float* best_fitness, int function_evals);

#endif //WORK_SHADE_H

//
// Created by beyer on 13.02.24.
//

#include "settings.h"

#ifndef WORK_SHADE_H
#define WORK_SHADE_H

void initialize_shade();
void shade(volatile float population[POPSIZE][DIM], volatile float fitness[POPSIZE], volatile float best_solution[DIM], volatile float* best_fitness, int function_evals);
void shade_reset(volatile float population[POPSIZE][DIM], volatile float fitness[POPSIZE], volatile float current_best_solution[DIM], volatile float* current_best_fitness);

#endif //WORK_SHADE_H

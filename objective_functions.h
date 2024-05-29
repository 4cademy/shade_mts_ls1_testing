//
// Created by Marcel Beyer on 06.02.24.
//

#include "settings.h"

#ifndef WORK_OBJECTIVE_FUNCTIONS_H
#define WORK_OBJECTIVE_FUNCTIONS_H

float objective_function_1(volatile float solution[DIM]);
float objective_function_2(volatile float solution[DIM]);
float (*objective_function[2])(volatile float solution[DIM]);

#endif //WORK_OBJECTIVE_FUNCTIONS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "settings.h"
#include "objective_functions.h"
#include "shade.h"
#include "utils.h"
#include "mts_ls1.h"

// volatile float trial_pop[POPSIZE][DIM] __attribute__((section(".myTrialPop")));
volatile float population[POPSIZE][DIM];
volatile float best_solution[DIM];
volatile float best_fitness;

// global variables
float fitness[POPSIZE];
float trial_fitness[POPSIZE];

int main()
{
    uint32_t check = 0xcafebabe;
    
    // initialize SHADE
    initialize_shade();

    // randomly initialise population
    for (unsigned int i = 0; i < POPSIZE; i++){
        for (unsigned int j = 0; j < DIM; j++) {
            population[i][j] = randf_range(MIN, MAX);
        }
        fitness[i] = objective_function_1(population[i]);
    }
    float current_best_solution[DIM] = {(MAX + MIN) / 2.0f};
    float current_best_fitness = objective_function_1(current_best_solution);

    printf("%f\n", current_best_fitness);

    //ToDo: first LS step
    // mts_ls1(25000, current_best_solution);

    // copy individual
    for (unsigned int j = 0; j < DIM; j++) {
        best_solution[j] = current_best_solution[j];
    }
    best_fitness = current_best_fitness;

    int fe = 25000;
    float improvement;
    int reset_counter = 0;
    float previous_best_fitness = current_best_fitness;

    // Main loop
    while (fe < FUNCTION_EVALS) {
        // SHADE
        shade(population, fitness, current_best_solution, &current_best_fitness, 2500);
        fe += 2500;

        improvement = (previous_best_fitness - current_best_fitness) / previous_best_fitness;
        previous_best_fitness = current_best_fitness;

        printf("FE: %d, Best fitness: %f, Improvement: %f\n", fe, current_best_fitness, improvement);

        // LS
        // ToDo: LS
        // fe += 25000;

        if (current_best_fitness < best_fitness) {
            // replace best individual
            for (unsigned int j = 0; j < DIM; j++) {
                best_solution[j] = current_best_solution[j];
            }
            best_fitness = current_best_fitness;
        }

        // RESET
        if (improvement < 0.05f) {
            reset_counter++;
        } else {
            reset_counter = 0;
        }

        if (reset_counter >= 3) {
            reset_counter = 0;
            // ToDo: RESET
        }
    }
    

    return check;
}

// vim: path+=../../qpe-common/include

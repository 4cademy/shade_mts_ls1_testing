#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
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
    printf("POPSIZE; %d\n", POPSIZE);
    printf("H: %d\n", H);
    printf("DIM: %d\n", DIM);
    printf("MIN: %d\n", MIN);
    printf("MAX: %d\n", MAX);
    printf("FUNCTION_EVALS: %d\n", FUNCTION_EVALS);
    printf("STEP_SIZE: %d\n", STEP_SIZE);

    srand(time(NULL));
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
    mts_ls1(STEP_SIZE, current_best_solution);

    // copy individual
    for (unsigned int j = 0; j < DIM; j++) {
        best_solution[j] = current_best_solution[j];
    }
    best_fitness = current_best_fitness;

    int fe = STEP_SIZE;
    float improvement;
    int reset_counter = 0;
    float previous_best_fitness = current_best_fitness;

    // Main loop
    while (fe < FUNCTION_EVALS) {
        // SHADE
        shade(population, fitness, current_best_solution, &current_best_fitness, STEP_SIZE);
        fe += STEP_SIZE;

        // LS
        // ToDo: LS
        mts_ls1(STEP_SIZE, current_best_solution);
        fe += STEP_SIZE;

        if (previous_best_fitness == 0.0f) {
            improvement = 0.0f;
        } else {
            improvement = (previous_best_fitness - current_best_fitness) / previous_best_fitness;
        }
        previous_best_fitness = current_best_fitness;

        printf("FE: %d, Best fitness: %f, Improvement: %f\n", fe, current_best_fitness, improvement);

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
            shade_reset(population, fitness, current_best_solution, &current_best_fitness);
        }
    }
    

    return check;
}

// vim: path+=../../qpe-common/include


#include <s2app.h>
#include <spinn_log.h>
#include <stdlib.h>
#include "settings.h"
#include "objective_functions.h"
#include "shade.h"
#include "utils.h"
#include "mts_ls1.h"
#include <stdlib.h>

#define LOG_DATA_BASE 0x0982c
#define LOG_STATUS 0
#define LOG_DEBUG_START 3
#define LOG_DEBUG_SIZE (1000 * 4)

#define LOG_STATUS_RUNNING 0
#define LOG_STATUS_DONE 1 // not used
#define LOG_STATUS_READY 2 // not used

static volatile uint32_t *status;
static volatile uint32_t *data = (uint32_t *) LOG_DATA_BASE;



// reserve memory
uint32_t log_data[LOG_DEBUG_SIZE/4 + 4] __attribute__((section(".myLogInfoSection")));
// volatile float trial_pop[POPSIZE][DIM] __attribute__((section(".myTrialPop")));
volatile float population[POPSIZE][DIM] __attribute__((section(".myPopulation")));
volatile float best_solution[DIM] __attribute__((section(".myReturnIndividual")));
volatile float best_fitness __attribute__((section(".myReturnFitness")));

// global variables
float fitness[POPSIZE];
float trial_fitness[POPSIZE];

void log_prepare(){
    status = &(data[LOG_STATUS]);
    *status = LOG_STATUS_RUNNING;
    log_init((uint32_t *) &(data[LOG_DEBUG_START]), LOG_DEBUG_SIZE);
}

int main()
{
    log_prepare();
    uint32_t check = 0xcafebabe;
    
    // initialize SHADE
    initialize_shade();

    // randomly initialise population
    for (unsigned int i = 0; i < POPSIZE; i++){
        for (unsigned int j = 0; j < DIM; j++) {
            population[i][j] = randf_range(MIN, MAX);
        }
    }
    float current_best_solution[DIM] = {(MAX + MIN) / 2.0f};
    float current_best_fitness = objective_function_1(current_best_solution);

    // THIS IS TESTING STUFF: REMOVE
    float num = 1234565.5;

    char str[20];
    sprintf(str, "%f", num);
    log_info("str\0");
    best_fitness = current_best_fitness;
    return check;
    // TESTING STUFF: END

    log_info("Init done\n");

    //ToDo: first LS step
    // mts_ls1(25000, current_best_solution);

    // copy individual
    for (unsigned int j = 0; j < DIM; j++) {
        best_solution[j] = current_best_solution[j];
    }
    best_fitness = current_best_fitness;

    log_info("After LS:\n");
    log_info("%i\n", (int)current_best_fitness);

    int fe = 25000;
    float improvement;
    int reset_counter = 0;
    float previous_best_fitness = current_best_fitness;

    // Main loop
    while (fe < FUNCTION_EVALS) {
        // SHADE
        shade(population, fitness, current_best_solution, current_best_fitness, 2500);
        fe += 2500;
        log_info("%f\n", current_best_fitness);

        improvement = (previous_best_fitness - current_best_fitness) / previous_best_fitness;
        previous_best_fitness = current_best_fitness;

        // LS
        // ToDo: LS
        fe += 25000;

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

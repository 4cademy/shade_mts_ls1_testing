//
// Created by beyer on 13.02.24.
//

#include "shade.h"
#include "settings.h"
#include "utils.h"
#include "objective_functions.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

float m_cr[H];             // memory for cr
float m_f[H];              // memory for f
int memory_counter = 0;    // counter for next memory position to be updated

// fills memories for cr and f with 0.5
void initialize_shade(){
    for(int i = 0; i<H; i++){
        m_cr[i] = 0.5f;
        m_f[i] = 0.5f;
    }
}

// this function calculates the weighted arithmetic mean of a vector of values
float weighted_arithmetic_mean(float* values, float* weights, int length){
    float total_weight = 0;
    for (int i = 0; i < length; i++){
        total_weight += weights[i];
    }

    float mean = 0;
    for(int i = 0; i < length; i++){
        mean += (values[i] * weights[i]/total_weight);
    }
    return mean;
}

// this function calculates the weighted Lehmer mean of a vector of values
float weighted_lehmer_mean(float* values, float* weights, int length, int power){
    float dividend = 0;
    float divisor = 0;
    for(int i = 0; i < length; i++){
        dividend += (powf(values[i], (float)power) * weights[i]);
        divisor += (powf(values[i], (float)(power - 1)) * weights[i]);
    }
    float mean = dividend/divisor;
    return mean;
}

int num_in_array(int array[POPSIZE], int length, int num){
    int in = 0;
    for (int i = 0; i < length; i++){
        if (num == array[i]){
            in = 1;
            break;
        }
    }
    return in;
}

void get_p_min_indices(int indices[POPSIZE], int p, volatile float fitness[POPSIZE]){
    // find index of best fitness
    int min_index = 0;
    for (int i = 1; i < POPSIZE; i++){
        if (fitness[i] < fitness[min_index]){
            min_index = i;
        }
    }
    indices[0] = min_index;
    int counter = 1;
    while (counter < p){
        // get first index that is not already in the array
        min_index = 0;
        while (num_in_array(indices, counter, min_index) == 1){
            min_index++;
        }
        for (int i = min_index+1; i < POPSIZE; i++){
            if (fitness[i] < fitness[min_index] && num_in_array(indices, counter, i) == 0){
                min_index = i;
            }
        }
        indices[counter] = min_index;
        counter++;
    }
}

void generate_trial_vector(float trial_vector[DIM], volatile float population[POPSIZE][DIM], volatile float fitness[POPSIZE], int parent_index, float f, float cr){
    int j = randi_range(0, POPSIZE - 1);
    // get first donor vector from population
    int donor1_index = randi_range(0, POPSIZE - 1);
    while (donor1_index == parent_index) {
        donor1_index = randi_range(0, POPSIZE - 1);
    }

    // copy donor1
    float donor1[DIM];
    for (int i = 0; i < DIM; i++){
        donor1[i] = population[donor1_index][i];
    }
    // get second donor vector from population or archive
    int donor2_index = randi_range(0, POPSIZE - 1);
    while ((donor2_index == donor1_index) || (donor2_index == parent_index)) {
        donor2_index = randi_range(0, POPSIZE - 1);
    }

    // copy donor2
    float donor2[DIM];
    for (int i = 0; i < DIM; i++){
        donor2[i] = population[donor2_index][i];
    }
    //current-to-pbest/1/bin
    int indices[POPSIZE];
    int p = randi_range(0, (int)(POPSIZE * 0.2) - 1);
    if (p < 2){
        p = 2;
    }
    get_p_min_indices(indices, p, fitness);
    int pbest_index = randi_range(0, p - 1);
    float pbest[DIM];
    for (int i = 0; i < DIM; i++){
        pbest[i] = population[pbest_index][i];
    }
    // build trial vector
    for (int i = 0; i < DIM; i++) {
        float old_gene = population[parent_index][i];
        float diff = f * ((pbest[i] - old_gene) + (donor1[i] - donor2[i]));
        float new_gene = old_gene + diff;
        if (new_gene < MIN) {
            new_gene = (MIN + new_gene)/2;
        } else if (new_gene > MAX) {
            new_gene = (MAX + new_gene)/2;
        }

        // binomial crossover
        if ((j == i) || (randf_range(0, 1) < cr)) {
            trial_vector[i] = new_gene;
        } else {
            trial_vector[i] = old_gene;
        }
    }
}

void shade(volatile float population[POPSIZE][DIM], volatile float fitness[POPSIZE], volatile float best_solution[DIM], volatile float* best_fitness, int function_evals){
    int fe = 0;
    while (fe < function_evals) {

        float trial_cr[POPSIZE];
        float trial_f[POPSIZE];
        float trial_pop[POPSIZE][DIM];
        float trial_fitness[POPSIZE];

        float s_cr[POPSIZE];    //storage for all cr of the trial population
        int s_cr_index = 0;
        float s_f[POPSIZE];    //storage for all f of the trial population
        int s_f_index = 0;
        float delta_fitness[POPSIZE];
        int delta_fitness_index = 0;

        // put current best into population by replacing the worst individual
        int max_index = get_max_index(fitness);
        for (int i = 0; i < DIM; i++) {
            population[max_index][i] = best_solution[i];
        }
        fitness[max_index] = *best_fitness;

        // generate trial population
        for (int i = 0; i < POPSIZE; ++i) {
            int ri = randi_range(0, H - 1);

            // generate new evaluation parameters cr and f for each individual
            float cr = randf_normal(m_cr[ri], 0.1);
            if (cr < 0) {
                cr = 0;
            } else if (cr > 1) {
                cr = 1;
            }
            trial_cr[i] = cr;
            float f = randf_cauchy(m_f[ri], 0.1);
            while (f < 0) {
                f = randf_cauchy(m_f[ri], 0.1);
            }
            if (f > 1) {
                f = 1;
            }
            trial_f[i] = f;
            // generate trial vector
            float trial_vector[DIM];
            generate_trial_vector(trial_vector, population, fitness, i, f, cr);
            // copy trial vector to trial population
            for (int j = 0; j < DIM; j++) {
                trial_pop[i][j] = trial_vector[j];
            }

            // evaluate trial vector
            trial_fitness[i] = objective_function_1(trial_vector);
            fe++;
        }

        for (int i = 0; i < POPSIZE; ++i) {
            if (trial_fitness[i] <= fitness[i]) {
                if(trial_fitness[i] < fitness[i]) {
                    // Here SHADE-ILS would add the not chosen parent to the archive

                    // temporarily store cr and f for calculating weighted arithmetic mean and weighted Lehmer mean in the next step
                    s_cr[s_cr_index] = trial_cr[i];
                    s_cr_index++;
                    s_f[s_f_index] = trial_f[i];
                    s_f_index++;
                    delta_fitness[delta_fitness_index] = fabsf( trial_fitness[i] - fitness[i]);
                    delta_fitness_index++;
                }

                // replace original individual with trial individual
                for (int j = 0; j < DIM; j++) {
                    population[i][j] = trial_pop[i][j];
                }
                fitness[i] = trial_fitness[i];
            } else {
                // original individual stays in population
            }
        }

        // if better solution was found S_CR and S_F are not empty -> update memories M_CR and M_F with weighted arithmetic mean of S_CR and weighted Lehmer mean of S_F
        if (s_cr_index > 0 && s_f_index > 0) {
            float mean_cr = weighted_arithmetic_mean(s_cr, delta_fitness, s_cr_index);
            float mean_f = weighted_lehmer_mean(s_f, delta_fitness, s_f_index, 2);
            m_cr[memory_counter] = mean_cr;
            m_cr[memory_counter] = mean_f;
            memory_counter++;
            if (memory_counter >= H) {
                memory_counter = 0;
            }
        }

        // find the best individual
        int best_index = 0;
        for (int i = 1; i < POPSIZE; ++i) {
            if (fitness[i] < fitness[best_index]) {
                best_index = i;
            }
        }
        *best_fitness = fitness[best_index];
        for (int i = 1; i < DIM; ++i) {
            if (fitness[i] < fitness[best_index]) {
                best_solution[i] = population[best_index][i];
            }
        }
    }
}
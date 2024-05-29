//
// Created by beyer on 02.04.24.
//
#define _GNU_SOURCE

#include <math.h>
#include <stdlib.h>

#include "mts_ls1.h"
#include "settings.h"
#include "objective_functions.h"
#include "utils.h"

void next_permutation(int* array, int length){
    for (int i = length-1; i >= 0; --i){
        //generate a random number [0, n-1]
        int j = rand() % (i+1);

        //swap the last element with element at random index
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int compare_with_thunk(const void* a, const void* b, void* array_to_sort_by)
{
    int int_a = * ( (int*) a );
    int int_b = * ( (int*) b );
    float* float_array = (float*) array_to_sort_by;

    if ( float_array[int_a] == float_array[int_b] ) return 0;
    else if ( float_array[int_a] < float_array[int_b] ) return -1;
    else return 1;
}

int mts_ls1_improve_dim(volatile float* sol, volatile float* best_fitness, unsigned dim_to_improve, float* SR){
    float newsol[DIM];
    for(int j = 0; j < DIM; j++){
        newsol[j] = sol[j];
    }

    newsol[dim_to_improve] -= SR[dim_to_improve];

    float fitness_newsol = (*objective_function[FUNC_NO])(newsol);

    int evals = 1;

    if( fitness_newsol < *best_fitness ){
        *best_fitness = fitness_newsol;
        for(int j = 0; j < DIM; j++){
            sol[j] = newsol[j];
        }
    } else if( fitness_newsol > *best_fitness ){
        newsol[dim_to_improve] = sol[dim_to_improve];
        newsol[dim_to_improve] += 0.5f * SR[dim_to_improve];

        newsol[dim_to_improve] = fmin(newsol[dim_to_improve], MAX);
        newsol[dim_to_improve] = fmax(newsol[dim_to_improve], MIN);

        fitness_newsol = (*objective_function[FUNC_NO])(newsol);
        evals++;

        if( fitness_newsol < *best_fitness ){
            *best_fitness = fitness_newsol;
            for(int j = 0; j < DIM; j++){
                sol[j] = newsol[j];
            }
        }
    }
    return evals;
}

void mts_ls1(unsigned maxevals, volatile float sol[DIM], volatile float* best_fitness){
    unsigned totalevals = 0;

    // search range
    float SR[DIM] = {(MAX - MIN) * 0.4f};

    int evals;

    float current_best_sol[DIM];
    for(int i = 0; i < DIM; i++){
        current_best_sol[i] = sol[i];
    }
    float current_best_fitness = *best_fitness;

    float improvement[DIM] = {0.0f};

    int dim_sorted[DIM];
    iota(dim_sorted, DIM, 0);

    float improve;
    //warm-up
    if( totalevals < maxevals ){
        next_permutation(dim_sorted, DIM);
        int dim_to_improve;
        for(int i = 0; i < DIM; i++){
            dim_to_improve = dim_sorted[i];
            evals = mts_ls1_improve_dim(sol, best_fitness, dim_to_improve, SR);
            totalevals += evals;
            improve = fmax(current_best_fitness - *best_fitness, 0.0f);
            improvement[i] = improve;

            if( improve > 0.0 ){
                for(int j = 0; j < DIM; j++){
                    current_best_sol[j] = sol[j];
                }
                current_best_fitness = *best_fitness;
            } else {
                SR[i] /= 2.0f;
            }
        }
    }


    iota(dim_sorted, DIM, 0);
    qsort_r( dim_sorted, DIM, sizeof(float), compare_with_thunk, improvement);

    int i, d = 0, next_d, next_i;
    while( totalevals < maxevals ){
        i = dim_sorted[d];
        evals = mts_ls1_improve_dim(sol, best_fitness, i, SR);
        totalevals += evals;
        improve = fmax(current_best_fitness - *best_fitness, 0.0f);
        improvement[i] = improve;
        next_d = (d+1)%DIM;
        next_i = dim_sorted[next_d];

        if( improve > 0.0 ){
            for(int j = 0; j < DIM; j++){
                current_best_sol[j] = sol[j];
            }
            current_best_fitness = *best_fitness;

            if( improvement[i] < improvement[next_i] ){
                for(int j = 0; j < DIM; j++){
                    dim_sorted[j] = j;
                }
                qsort_r( dim_sorted, DIM, sizeof(float), compare_with_thunk, improvement);
            }
        } else {
            SR[i] /= 2.0;
            d = next_d;
            if( SR[i] < 1e-15 ){
                SR[i] = (MAX - MIN) * 0.4f;
            }
        }
    }
    for(int j = 0; j < DIM; j++){
        sol[j] = current_best_sol[j];
    }
    *best_fitness = current_best_fitness;
}
//
// Created by Marcel Beyer on 06.02.24.
//
#include "objective_functions.h"
#include "settings.h"
#include "const_vectors.h"

#include <math.h>
#ifndef  M_PI
#define  M_PI 3.1415926535897932384626433
#endif

#ifndef  M_E
#define  M_E 2.7182818284590452353602874
#endif


float elliptic_function(float* x, unsigned size) {
    float result = 0;
    for(unsigned i = 1; i <= size; i++) {
        result += powf(1.0e6,  (float)(i-1)/(float)(size - 1) ) * x[i-1] * x[i-1];
    }
    return result;
}

void T_osz(float* x, unsigned size) {
    int sign;
    float hat;
    float c1;
    float c2;

    for(unsigned i = 1; i <= size; i++) {
        if (x[i-1] > 0) {
            sign = 1;
            hat = logf(fabsf(x[i-1]));
            c1 = 10;
            c2 = 7.9;
            x[i-1] = (float) sign * expf(hat + (float) 0.049 * (sinf(c1 * hat) + sinf(c2 * hat)));
        } else if (x[i-1] == 0) {
            x[i-1] = 0;
        } else {
            sign = -1;
            hat = logf(fabsf(x[i-1]));
            c1 = 5.5;
            c2 = 3.1;
            x[i-1] = (float) sign * expf(hat + (float) 0.049 * (sinf(c1 * hat) + sinf(c2 * hat)));
        }
    }
}

void T_asy(float* x, float beta) {
    for(unsigned i = 1; i <= DIM; i++) {
        if (x[i-1] > 0) {
            x[i-1] = powf(x[i-1], 1.0f + beta * (float)(i-1) / (float)(DIM-1) * sqrtf(x[i-1]));
        }
    }
}

void multiply_with_lambda_matrix(float* x, float alpha) {
    for(unsigned i = 1; i <= DIM; i++) {
        x[i-1] = powf(alpha, 0.5f*(float)(i-1)/(float)(DIM-1)) * x[i-1];
    }
}

void multiply_matrix_vector(unsigned matrix_height, unsigned matrix_width, float matrix[matrix_height][matrix_width], float vector[matrix_width], float* result_vector) {
    for (unsigned i = 0; i < matrix_height; i++) {
        result_vector[i] = 0;
        for (unsigned j = 0; j < matrix_width; j++) {
            result_vector[i] += matrix[i][j] * vector[j];
        }
    }
}

// objective function 1: Shifted Elliptic Function
float objective_function_1(volatile float solution[DIM]) {
    float result = 0;
    float z[DIM];

    // Shift
    for (unsigned i = 0; i < DIM; i++) {
        z[i] = solution[i] - F1_xopt[i];
    }

    // Oscillation
    T_osz(z, DIM);

    // Function
    for(unsigned i = 0; i < DIM; i++) {
        result += powf(1.0e6,  (float)i/(float)((DIM - 1)) ) * z[i] * z[i];
    }

    return result;
}

// objective function 2: Shifted Rastrigin’s Function
float objective_function_2(volatile float solution[DIM]) {
    float result = 0;
    float z[DIM];

    // Shift
    for (unsigned i = 0; i < DIM; i++) {
        z[i] = solution[i] - F2_xopt[i];
    }

    // Adding Asymmetry
    T_asy(z, 0.2f);

    // Multiply with lambda matrix
    multiply_with_lambda_matrix(z, 10.0f);

    // Function
    for(unsigned i = 0; i < DIM; i++) {
        result += z[i] * z[i] - 10.0f * cosf(2.0f * M_PI * z[i]) + 10.0f;
    }

    return result;
}

// objective function 3: Shifted Ackley's Function
float objective_function_3(volatile float solution[DIM]) {
    float result = 0;
    float sum1 = 0;
    float sum2 = 0;
    float z[DIM];

    // Shift
    for (unsigned i = 0; i < DIM; i++) {
        z[i] = solution[i] - F3_xopt[i];
    }
    
    // Adding Asymmetry
    T_asy(z, 0.2f);

    // Multiply with lambda matrix
    multiply_with_lambda_matrix(z, 10.0f);

    // Sum 1
    for(unsigned i = 0; i < DIM; i++) {
        sum1 += z[i] * z[i];
    }

    // Sum 2
    for (unsigned i = 0; i < DIM; i++)
    {
        sum2 += cosf(2.0f * M_PI * z[i]);
    }

    // Function
    result = -20.0f * expf(-0.2f * sqrtf(sum1 / DIM)) - expf(sum2 / DIM) + 20.0f + M_E;

    return result;
}

// objective function 4: 7-nonseparable, 1-separable Shifted and Rotated Elliptic Function
float objective_function_4(volatile float solution[DIM]) {
    float result = 0;
    float y[DIM];
    unsigned size_S = 8;

    // Shift
    for (unsigned i = 0; i < DIM; i++) {
        y[i] = solution[i] - F4_xopt[i];
    }

    // Calculation of 7 nonseparable subcomponents
    for (unsigned i = 0; i < size_S-1; i++) {
        unsigned subcomponent_size = F4_s[i];
        float y_sub[subcomponent_size];

        // Calculate starting index of subcomponent
        unsigned start_index = 0;
        for (unsigned j = 0; j < i; j++) {
            start_index += F4_s[j];
        }

        // Move subcomponent from y to y_sub according to p (permuated dimension indices)
        for (unsigned j = 0; j < subcomponent_size; j++) {
            y_sub[j] = y[F4_p[start_index+j]];
        }

        // Rotate subcomponent
        float z_sub[subcomponent_size];
        if (subcomponent_size == 25) {
            multiply_matrix_vector(subcomponent_size, subcomponent_size, F4_R25, y_sub, z_sub);
        } else if (subcomponent_size == 50) {
            multiply_matrix_vector(subcomponent_size, subcomponent_size, F4_R50, y_sub, z_sub);
        } else { // subcomponent_size == 100
            multiply_matrix_vector(subcomponent_size, subcomponent_size, F4_R100, y_sub, z_sub);
        }

        // Oscillation of subcomponent
        T_osz(z_sub, subcomponent_size);

        // Elliptic function of subcomponent and add weighted to result
        result += F4_w[i] * elliptic_function(z_sub, subcomponent_size);
    }

    // ---------------------------------------

    // Calculation of 1 separable subcomponent
    unsigned subcomponent_size = F4_s[size_S-1];

    // Calculate starting index of subcomponent
    unsigned start_index = 0;
    for (unsigned j = 0; j < size_S-1; j++) {
        start_index += F4_s[j];
    }

    // Move subcomponent from y to y_sub according to p (permuated dimension indices)
    for (unsigned j = 0; j < subcomponent_size; j++) {
        y[j] = y[F4_p[start_index+j]];
    }

    // Oscillation of subcomponent
    T_osz(y, subcomponent_size);

    // Elliptic function of subcomponent and add to result
    result += elliptic_function(y, subcomponent_size);

    return result;
}

// objective function 5:
float objective_function_5(volatile float solution[DIM]) {
    return 0;
}

// objective function 6:
float objective_function_6(volatile float solution[DIM]) {
    return 0;
}

// objective function 7:
float objective_function_7(volatile float solution[DIM]) {
    return 0;
}

// objective function 8:
float objective_function_8(volatile float solution[DIM]) {
    return 0;
}

// objective function 9:
float objective_function_9(volatile float solution[DIM]) {
    return 0;
}

// objective function 10:
float objective_function_10(volatile float solution[DIM]) {
    return 0;
}

// objective function 11:
float objective_function_11(volatile float solution[DIM]) {
    return 0;
}

// objective function 12:
float objective_function_12(volatile float solution[DIM]) {
    return 0;
}

// objective function 13:
float objective_function_13(volatile float solution[DIM]) {
    return 0;
}

// objective function 14:
float objective_function_14(volatile float solution[DIM]) {
    return 0;
}

// objective function 15:
float objective_function_15(volatile float solution[DIM]) {
    return 0;
}

float (*objective_function[15])(volatile float solution[DIM]) = {objective_function_1, objective_function_2, objective_function_3, objective_function_4, objective_function_5, objective_function_6, objective_function_7, objective_function_8, objective_function_9, objective_function_10, objective_function_11, objective_function_12, objective_function_13, objective_function_14, objective_function_15};
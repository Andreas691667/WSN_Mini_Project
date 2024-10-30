#include <math.h>
#include <stdio.h>
#include "inttypes.h"

/*
DCT Transform function
args:
    x: input array
    y: output array
    n: size of the array
*/
// void dct_transform(float *x, float *y, int n)
// {
//     int k, j;               // loop variables
//     float sum;              // sum variable
//     float c;                // variable for the constant
//     for (k = 0; k < n; k++) // loop through the input array
//     {
//         sum = 0;                // initialize sum for the kth component
//         for (j = 0; j < n; j++) // loop through the input array
//         {
//             sum += x[j] * cos(PI / n * (j + 1 / 2) * k); // calculate the sum
//         }
//         if (k == 0)
//         {
//             c = sqrt(1 / n); // calculate the constant for k = 0
//         }
//         else
//         {
//             c = sqrt(2 / n); // calculate the constant for k != 0
//         }

//         y[k] = c * sum; // store the kth component in the output array
//     }
// }
// parameters
#define _N 16                  // number of temperature readings
#define L 64                    // size of the temperature block
#define M 4                    // size of the compressed block
// #define PI 3.14159265359       // value of pi
#define FIXED_POINT_SCALE pow(2,5) // scale factor for fixed-point arithmetic

static int32_t H[L][L];

// Function to create the DCT matrix
void create_dct_matrix()
{
    for (int k = 0; k < L; k++)
    {
        for (int n = 0; n < L; n++)
        {
            // Calculate the normalization factor alpha_k
            double alpha_k = (k == 0) ? sqrt(1.0 / L) : sqrt(2.0 / L);
            // Populate H[k][n] using the DCT formula
            int32_t val = (int32_t)(alpha_k * cos(M_PI / L * (n + 0.5) * k) * FIXED_POINT_SCALE);
            H[k][n] = val;
        }
    }
}

void dct_transform_test(int *x, int *y)
{
    
    // Loop through the rows of the DCT matrix
    for (int k = 0; k < L; ++k)
    {
        // Initialize the sum for the k-th component
        // y[k] = 0;
        int sum = 0;

        // Loop through the input data
        for (int n = 0; n < L; ++n)
        {
            // Calculate the sum
            sum += (x[n] * H[k][n]);
        }

        y[k] = (int)(sum / FIXED_POINT_SCALE);
    }
}

int main()
{
    // create the DCT matrix and print it
    create_dct_matrix();

    // print DCT matrix
    printf("DCT matrix: \n");
    for (int i = 0; i < L; i++)
    {
        for (int j = 0; j < L; j++)
        {
            printf("%" PRId32 " ", H[i][j]);
        }
        printf("\n");
    }

    // int x[L] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
    // int x[L] = {26650, 26640, 26629, 26629, 26620, 26610, 26620, 26610};
    // 26480 26480 26480 26480 26490 26490 26490 26501
    // int x[L] = {26480, 26480, 26480, 26480, 26490, 26490, 26490, 26501};
    // 25866 25876 25896 25907 25896 25907 25907 25907
    // int x[L] = {25866, 25876, 25896, 25907, 25896, 25907, 25907, 25907};
    // 25651 25692 25702 25722 25733 25743 25743 25753
    // int x[L] = {2565, 2569, 2570, 2572, 2573, 2574, 2574, 2575};
    // 25579 25600 25610 25620 25630 25630 25620 25620
    // int x[L] = {25579, 25600, 25610, 25620, 25630, 25630, 25620, 25620};
    // 2498 2500 2501 2501 2500 2501 2501 2501
    // int x[L] = {2498, 2500, 2501, 2501, 2500, 2501, 2501, 2501};
    // 2486 2487 2487 2487 2487 2490 2489 2489
    // int x[L] = {2486, 2487, 2487, 2487, 2487, 2490, 2489, 2489};
    //  2440 2440 2441 2441 2441 2444 2443 2444
    // int x[L] = {2440, 2440, 2441, 2441, 2441, 2444, 2443, 2444};
    //  2478 2480 2482 2483 2483 2483 2483 248
    // int x[L] = {2478, 2480, 2482, 2483, 2483, 2483, 2483, 2483};
    // 2454 2453 2451 2451 2453 2454 2453 2455
    // int x[L] = {2454, 2453, 2451, 2451, 2453, 2454, 2453, 2455};


    // int y[L];
    // dct_transform_test(x, y);

    // for (int i = 0; i < L; i++)
    // {
    //     printf("%d ", y[i]);
    // }
}
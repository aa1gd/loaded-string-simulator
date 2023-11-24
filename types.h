/*----------------------------------------------------------------------------*/
/* types.h                                                                    */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <limits.h>

enum SimType {STRING, SPRING}; /* Simulation types */

typedef struct bead
{
    double mass; /* Mass of bead in kg */
    double x0; /* Initial displacement away from equilibrium point in m */
    double v0; /* Initial velocity in m/s */
} Bead;

typedef struct simulation
{
    char filename[NAME_MAX + 1]; /* prefix of simulation input file name */
    enum SimType sim_type; /* Either STRING or SPRING */
    Bead *beads; /* Array containing num_beads beads */
    double *connections; /* Array of length num_beads + 1. For string
                            simulations, represents distance between beads in m.
                            For spring simulations, represents spring constants
                            of springs between beads in N/m. */
    double tension; /* For string simulations, the tension in the string in N */
    int num_beads; /* Number of beads */
} Simulation;

typedef struct coefficient
{
    double a; /* Coefficient of the cosine term */
    double b; /* Coefficient of the sine term */
} Coefficient;

typedef struct result
{
    int num_modes; /* Number of normal modes. Equal to number of beads. */
    double *eigenfrequencies; /* Array containing num_modes eigenfrequencies.
                                 Sorted in order from smallest to largest. */
    double **eigenvectors; /* Array containing num_modes arrays of normalized
                              eigenvectors. Each eigenvector corresponds to the
                              eigenfrequency in above array. */
    Coefficient *coefficients; /* Array of num_modes Coefficients */
} Result;

#endif

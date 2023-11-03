/*--------------------------------------------------------------------*/
/* types.h                                                            */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

enum SimType {STRING, SPRING};

typedef struct bead
{
    double mass;
    double x0;
    double v0;
} Bead;

typedef struct simulation
{
    Bead *beads;
    double *connections;
    double tension;
    int num_beads;
    enum SimType sim_type;
} Simulation;

typedef struct coefficient
{
    double a;
    double b;
    /* double phase psi; */
    /* Perhaps damping goes in here too */
} Coefficient;

typedef struct result
{
    int num_modes;
    double *eigenfrequencies;
    double **eigenvectors;
    Coefficient *coefficients;
} Result;

#endif

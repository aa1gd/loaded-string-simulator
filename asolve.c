/*--------------------------------------------------------------------*/
/* asolve.c                                                           */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
*/

#include "asolve.h"

int asolve(Bead *beads[], double *connections[], 
        double tension, int num_beads, enum SimType sim_type)
{
    int i;

    assert(beads != NULL);
    assert(connections != NULL);

    printf("Performing an analytical solution for a ");
    if (sim_type == STRING)
        printf("string with ");
    else if (sim_type == SPRING)
        printf("spring with ");
    printf("%d beads.\n", num_beads);

    /*
    if (sim_type == STRING)
        printf("String tension: %f\n", tension);

    for (i = 0; i < num_beads; i++)
    {
        printf("Length: %lf\n", (*connections)[i]);
        printf("Mass: %lf x0: %lf v0: %lf\n", (*beads)[i].mass, (*beads)[i].x0,
            (*beads)[i].v0);
    }
    printf("Length: %lf\n", (*connections)[num_beads]);
    */

    return 0;
}



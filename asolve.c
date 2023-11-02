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

int asolve(Simulation sim)
{
    int i;

    assert(sim.beads != NULL);
    assert(sim.connections != NULL);

    printf("Performing an analytical solution for a ");
    if (sim.sim_type == STRING)
        printf("string with ");
    else if (sim.sim_type == SPRING)
        printf("spring with ");
    printf("%d beads.\n", sim.num_beads);

    /* Check that data was loaded correctly
    if (sim.sim_type == STRING)
        printf("String tension: %f\n", sim.tension);

    for (i = 0; i < sim.num_beads; i++)
    {
        printf("Length: %lf\n", sim.connections[i]);
        printf("Mass: %lf x0: %lf v0: %lf\n", sim.beads[i].mass,
                sim.beads[i].x0, sim.beads[i].v0);
    }
    printf("Length: %lf\n", sim.connections[sim.num_beads]);
    */

    return 0;
}



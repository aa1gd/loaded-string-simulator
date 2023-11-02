/*--------------------------------------------------------------------*/
/* simulate.c                                                         */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "importdata.h"
#include "asolve.h"
#include "nsolve.h"
#include "plot.h"

int main(int argc, char *argv[])
{
    Simulation sim;

    /* Check if a filename has been specified in the command */
    if (argc < 2)
    {
        printf("Missing Filename\n");
        return EXIT_FAILURE;
    }
    else
    {
        if (import_data(argv[argc - 1], &sim))
            return EXIT_FAILURE;
    }

    asolve(sim);

    free(sim.beads);
    free(sim.connections);

    return EXIT_SUCCESS;
}


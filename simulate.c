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
    Result result;

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

    result = asolve(sim);

    /*
    print_result(result);
    plot_eigenfrequencies(result);
    plot_mode_amplitudes(result);
    */

    animate_string(result, sim.connections);

    /* TODO: make freer functions */
    free(sim.beads);
    free(sim.connections);
    
    /*
    free(result.eigenfrequencies);
    free(result.coefficients);
    */

    /* free eigenvectors in a for loop */

    return EXIT_SUCCESS;
}


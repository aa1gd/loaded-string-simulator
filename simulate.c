/*----------------------------------------------------------------------------*/
/* simulate.c                                                                 */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "importdata.h"
#include "asolve.h"
#include "plot.h"

/* Simulates a loaded string or mass-spring coupled oscillator.
 *
 * Usage:
 * ./simulate [OPTIONS] [FILE]
 *
 * Options:
 * -p, --print
 *        prints eigenfrequencies, eigenvectors, and mode amplitudes in terminal
 *
 * -e, --eigenfrequencies
 *        plots eigenfrequencies
 *
 * -a, --amplitudes
 *        plots mode amplitudes
 *
 * -m, --modes
 *        plots individual normal modes
 *
 * -s, --simulate [TIME_SCALE]
 *        animates the simulation at a speed TIME_SCALE x real speed
 *        TIME_SCALE defaults to 1.0 if unspecified
 *
 * -g, --gif
 *        only use following -s option. Saves animation as a .gif
 *
 * -p option is used if no options specified
 */
int main(int argc, char *argv[])
{
    Simulation sim;
    Result result;
    int argnum;
    int i;

    /* Check if a filename has been specified in the command */
    if (argc < 2)
    {
        fprintf(stderr, "Missing filename.\n");
        return EXIT_FAILURE;
    }

    if (import_data(argv[argc - 1], &sim))
    {
        fprintf(stderr, "Failed to import data.\n");
        return EXIT_FAILURE;
    }

    result = asolve(sim);

    /* print results if no flags specified */
    if (argc == 2)
        print_result(result);

    for (argnum = 1; argnum < argc - 1; argnum++)
    {
        if (!strcmp(argv[argnum], "-p") || !strcmp(argv[argnum], "--print"))
            print_result(result);
        else if (!strcmp(argv[argnum], "-e") || !strcmp(argv[argnum], "--eigenfrequencies"))
            plot_eigenfrequencies(result);
        else if (!strcmp(argv[argnum], "-a") || !strcmp(argv[argnum], "--amplitudes"))
            plot_mode_amplitudes(result);
        else if (!strcmp(argv[argnum], "-m") || !strcmp(argv[argnum], "--modes"))
            plot_normal_modes(result, sim);
        else if (!strcmp(argv[argnum], "-s") || !strcmp(argv[argnum], "--simulate"))
        {
            double time_scale; /* defaults to real time if not specified */
            if ((time_scale = atof(argv[argnum + 1])) != 0)
                argnum++;
            else
            {
                fprintf(stderr, "No time scale specified, default to 1.\n");
                time_scale = 1.0;
            }

            /* Save gif of simulation: name will be simulationfilename.gif */
            if (!strcmp(argv[argnum + 1], "-g") || !strcmp(argv[argnum + 1], "--gif"))
            {
                animate(result, sim, time_scale, true);
                argnum++;
            }
            else
                animate(result, sim, time_scale, false);
        }
        else
            fprintf(stderr, "Invalid flag.\n");
    }

    free(sim.beads);
    free(sim.connections);
    free(result.eigenfrequencies);
    free(result.coefficients);
    for (i = 0; i < result.num_modes; i++)
        free(result.eigenvectors[i]);
    free(result.eigenvectors);

    return EXIT_SUCCESS;
}


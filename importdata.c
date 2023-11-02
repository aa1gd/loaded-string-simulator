/*--------------------------------------------------------------------*/
/* importdata.c                                                       */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "importdata.h"

int import_data(const char *filename, Simulation *sim)
{
    FILE *fp;
    char string_sim_type[7];
    int i;

    assert(filename != NULL);

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open input file.\n");
        return 1;
    }

    printf("Importing data from %s\n", filename);

    fscanf(fp, "%6s\n", string_sim_type);

    if (strcasecmp("String", string_sim_type) == 0)
        sim->sim_type = STRING;
    else if (strcasecmp("Spring", string_sim_type) == 0)
        sim->sim_type = SPRING;
    else
    {
        fprintf(stderr, "Simulation type must be either string or \
                spring.\n");
        return 1;
    }

    /* Scan in tension if it's a string simulation */
    if (sim->sim_type == STRING)
        fscanf(fp, "%lf", &(sim->tension));

    fscanf(fp, "%d", &(sim->num_beads));
    
    /* Allocate memory for beads and connections */
    sim->beads = calloc(sim->num_beads, sizeof(Bead));
    if (sim->beads == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for beads.\n");
        return 1;
    }

    sim->connections = calloc(sim->num_beads + 1, sizeof(double));
    if (sim->connections == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for connections.\n");
        return 1;
    }

    for (i = 0; i < sim->num_beads; i++)
    {
        fscanf(fp, "%lf", &(sim->connections[i]));
        fscanf(fp, "%lf %lf %lf", &(sim->beads[i].mass),
                &(sim->beads[i].x0),
                &(sim->beads[i].v0));
    }

    /* There's one more connection than bead; scan that in */
    fscanf(fp, "%lf", &(sim->connections[sim->num_beads]));

    fclose(fp);

    printf("Finished importing data from %s\n\n", filename);
    return 0;
}


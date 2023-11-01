/*--------------------------------------------------------------------*/
/* importdata.h                                                       */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#ifndef IMPORTDATA_INCLUDED
#define IMPORTDATA_INCLUDED

#include "types.h"

int import_data(const char *filename, Bead *beads[], double *connections[], 
        double *tension, int *num_beads, enum SimType *sim_type);

/*
void import_test_data(Bead *beads[], double *connections[], 
        double *tension, int *num_beads, enum SimType *sim_type);
        */

#endif

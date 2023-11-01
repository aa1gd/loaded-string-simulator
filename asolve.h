/*--------------------------------------------------------------------*/
/* asolve.h                                                           */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#ifndef ASOLVE_INCLUDED
#define ASOLVE_INCLUDED

#include "types.h"

int asolve(Bead *beads[], double *connections[], 
        double tension, int num_beads, enum SimType sim_type);

#endif

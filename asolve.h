/*----------------------------------------------------------------------------*/
/* asolve.h                                                                   */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#ifndef ASOLVE_INCLUDED
#define ASOLVE_INCLUDED

#include "types.h"

/* Given simulation parameters in sim, calculates eigenfrequencies,
 * eigenvectors, coefficients corresponding to initial conditions, stores these
 * in a Result which is returned. Caller responsible for freeing all dynamically
 * allocated parts of the Result. */
Result asolve(Simulation sim);

#endif

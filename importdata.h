/*----------------------------------------------------------------------------*/
/* importdata.h                                                               */
/* Author: Godwin Duan                                                        */
/*----------------------------------------------------------------------------*/

#ifndef IMPORTDATA_INCLUDED
#define IMPORTDATA_INCLUDED

#include "types.h"

/* Given a simulation parameter file of the form described in
 * examplestringinput.txt or examplespringinput.txt, read in and store
 * simulation parameters into sim. Returns 1 if an error occured, 0 otherwise */
int import_data(const char *filename, Simulation *sim);

#endif

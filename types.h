/*--------------------------------------------------------------------*/
/* types.h                                                            */
/* Author: Godwin Duan                                                */
/*--------------------------------------------------------------------*/

#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

typedef struct bead
{
    double mass;
    double x0;
    double v0;
} Bead;

enum SimType {STRING, SPRING};

#endif

#ifndef _PROJECTION_H
#define _PROJECTION_H

#define PROJ_NONE 0
#define PROJ_POLAR 1
#define PROJ_CYLINDRIC 2
#define PROJ_LAMBERT_LCC 3

typedef struct _proj_S {
    int proj_type;
} proj;

proj *init_Lambert(double l0, double p0, double p1, double p2);
void project(double *x, double *y, double phi, double lambda, proj *P);

#endif /* _PROJECTION_H */

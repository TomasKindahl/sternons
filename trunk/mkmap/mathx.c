#include <math.h>
#include "mathx.h"

double cot(double x) { return 1/tan(x); }
double sec(double x) { return 1/cos(x); }
double deg2rad(double x) { return M_PI * x / 180.0; }
double rad2deg(double x) { return 180.0 * x / M_PI; }


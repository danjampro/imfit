/* FILE: func_gaussianring3d.cpp -------------------------------------- */
/* VERSION 0.1
 *
 *   Experimental function object class for a circular 3D ring (luminosity
 * density = Gaussian centered at r0 with width sigma and vertical exponential with
 * scale heigh h_z), seen at position angle PA and inclination inc.
 *   
 *   BASIC IDEA:
 *      Setup() is called as the first part of invoking the function;
 *      it pre-computes various things that don't depend on x and y.
 *      GetValue() then completes the calculation, using the actual value
 *      of x and y, and returns the result.
 *      So for an image, we expect the user to call Setup() once at
 *      the start, then loop through the pixels of the image, calling
 *      GetValue() to compute the function results for each pixel coordinate
 *      (x,y).
 *
 *   NOTE: Currently, we assume input PA is in *degrees* [and then we
 * convert it to radians] relative to +x axis.
 *
 *   MODIFICATION HISTORY:
 *     [v0.1]: 24 Aug 2012: Created (as modification of func_exp3d.cpp).
 */





/* ------------------------ Include Files (Header Files )--------------- */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <gsl/gsl_errno.h>

#include "func_gaussianring3d.h"
#include "integrator.h"


using namespace std;

/* ---------------- Definitions ---------------------------------------- */
const int   N_PARAMS = 6;
const char  PARAM_LABELS[][20] = {"PA", "inc", "I_0", "R_0", "sigma", "h_z"};
const char  FUNCTION_NAME[] = "GaussianRing3D function";
const double  DEG2RAD = 0.017453292519943295;
const int  SUBSAMPLE_R = 10;

const double  INTEGRATION_MULTIPLIER = 2;

const char GaussianRing3D::className[] = "GaussianRing3D";


/* ---------------- Local Functions ------------------------------------ */

double LuminosityDensityRing( double s, void *params );




/* ---------------- CONSTRUCTOR ---------------------------------------- */

GaussianRing3D::GaussianRing3D( )
{
  string  paramName;
  
  nParams = N_PARAMS;
  functionName = FUNCTION_NAME;
  shortFunctionName = className;

  // Set up the vector of parameter labels
  for (int i = 0; i < nParams; i++) {
    paramName = PARAM_LABELS[i];
    parameterLabels.push_back(paramName);
  }

  // Stuff related to GSL integration
  gsl_set_error_handler_off();
  F.function = LuminosityDensityRing;
  
  doSubsampling = false;
}


/* ---------------- PUBLIC METHOD: Setup ------------------------------- */

void GaussianRing3D::Setup( double params[], int offsetIndex, double xc, double yc )
{
  x0 = xc;
  y0 = yc;
  PA = params[0 + offsetIndex];
  inclination = params[1 + offsetIndex];
  I_0 = params[2 + offsetIndex ];
  R_0 = params[3 + offsetIndex ];
  sigma = params[4 + offsetIndex ];
  h_z = params[5 + offsetIndex ];

  // pre-compute useful things for this round of invoking the function
  // convert PA to +x-axis reference
  PA_rad = (PA + 90.0) * DEG2RAD;
  cosPA = cos(PA_rad);
  sinPA = sin(PA_rad);
  inc_rad = inclination * DEG2RAD;
  cosInc = cos(inc_rad);
  sinInc = sin(inc_rad);
  //tanInc = tan(inc_rad);
  
  // Don't try doing the following here; it make the whole thing ~ 4 times slower!
  //integLimit = INTEGRATION_MULTIPLIER * h;
}


/* ---------------- PUBLIC METHOD: GetValue ---------------------------- */

double GaussianRing3D::GetValue( double x, double y )
{
  double  x_diff = x - x0;
  double  y_diff = y - y0;
  double  xp, yp, x_d0, y_d0, z_d0, totalIntensity, error;
  double  integLimit;
  double  xyParameters[9];
  int  nSubsamples;
  int  nEvals;
  
  // Calculate x,y in component's (projected sky) reference frame: xp,yp
  xp = x_diff*cosPA + y_diff*sinPA;
  yp = -x_diff*sinPA + y_diff*cosPA;

  // Calculate (x,y,z)_start in component's native xyz reference frame, corresponding to
  // intersection of line-of-sight ray with projected sky frame
  x_d0 = xp;
  y_d0 = yp * cosInc;
  z_d0 = yp * sinInc;

  // Set up parameter vector for the integration (everything that stays unchanged
  // for this particular xp,yp location)
  xyParameters[0] = x_d0;
  xyParameters[1] = y_d0;
  xyParameters[2] = z_d0;
  xyParameters[3] = cosInc;
  xyParameters[4] = sinInc;
  xyParameters[5] = I_0;
  xyParameters[6] = R_0;
  xyParameters[7] = sigma;
  xyParameters[8] = h_z;
  F.params = xyParameters;

  // integrate out to +/- integLimit, which is multiple of ring radius
  // (NOTE: it seems like it would be faster to precalculate integLimit in the
  // Setup() call above; for some reason doing it that way makes the whole thing
  // take ~ 4 times longer!)
  integLimit = INTEGRATION_MULTIPLIER * R_0;
//  printf("x,y = %f,%f; x_d0,y_d0,z_d0 = %f,%f,%f; cosInc,sinInc = %f,%f\n",
//  	x,y, x_d0,y_d0,z_d0, cosInc,sinInc);
  totalIntensity = Integrate(F, -integLimit, integLimit);

  return totalIntensity;
}







/* ----------------------------- OTHER FUNCTIONS -------------------------------- */


/* Compute luminosity density for a location (x_d,y_d,z_d) which is at line-of-sight 
 * distance s from start point (x_d0, y_d0, z_d0), where midplane of component (e.g.,
 * disk of galaxy) is oriented at angle (90 - inclination) to the line of sight vector. 
 */ 
double LuminosityDensityRing( double s, void *params )
{
  double  y_d, z_d, z, R, deltaR, exponent, lumDensity;
  double  *paramsVect = (double *)params;
  double x_d0 = paramsVect[0];
  double y_d0 = paramsVect[1];
  double z_d0 = paramsVect[2];
  double cosInc = paramsVect[3];
  double sinInc = paramsVect[4];
  double I_0 = paramsVect[5];
  double R_0 = paramsVect[6];
  double sigma = paramsVect[7];
  double h_z = paramsVect[8];
  
  // Given s and the pre-defined parameters, determine our 3D location (x_d,y_d,z_d)
  // [by construction, x_d = x_d0]
  y_d = y_d0 + s*sinInc;
  z_d = z_d0 - s*cosInc;
  
  // Convert 3D Cartesian coordinate to R,z coordinate
  R = sqrt(x_d0*x_d0 + y_d*y_d);
  z = fabs(z_d);
  
  deltaR = R - R_0;
  exponent = deltaR/sigma;
  lumDensity = I_0 * exp(-exponent*exponent) * exp(-z/h_z);
  return lumDensity;
}



/* END OF FILE: func_gaussianring3d.cpp -------------------------------- */

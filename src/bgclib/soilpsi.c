/* 
soilpsi.c
soil water potential as a function of volumetric water content and
constants related to texture

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.2 (final release)
See copyright.txt for Copyright information
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include "bgc.h"
//change soilw to soilWobs, it will change epv->psi. -- Y.He Oct/29/14
int soilpsi(const siteconst_struct* sitec, double soilWobs, double* psi,
double* vwc_out, wstate_struct* ws)
{
	/* given a list of site constants and the soil water mass (kg/m2),
	this function returns the soil water potential (MPa)
	inputs:
	ws.soilw           (kg/m2) water mass per unit area
	ws.soilWobs	   (kg/m2) water mass per unit area from observation 
	sitec.soil_depth   (m)     effective soil depth               
	sitec.soil_b       (DIM)   slope of log(psi) vs log(rwc)
	sitec.vwc_sat      (DIM)   volumetric water content at saturation
	sitec.psi_sat      (MPa)   soil matric potential at saturation
	output:
	psi_s              (MPa)   soil matric potential

	uses the relation:
	psi_s = psi_sat * (vwc/vwc_sat)^b

	For further discussion see:
	Cosby, B.J., G.M. Hornberger, R.B. Clapp, and T.R. Ginn, 1984.  A     
	   statistical exploration of the relationships of soil moisture      
	   characteristics to the physical properties of soils.  Water Res.
	   Res. 20:682-690.
	
	Saxton, K.E., W.J. Rawls, J.S. Romberger, and R.I. Papendick, 1986.
		Estimating generalized soil-water characteristics from texture.
		Soil Sci. Soc. Am. J. 50:1031-1036.
	*/

	int ok=1;
	double vwc,psi_temp1,psi_temp2;
	double Alpha, Beta, Theta_r, Theta_s, fc;// van Genuchten parameters

	Alpha = sitec->obs_alpha;
	Beta = sitec->obs_beta;
	Theta_r = sitec->obs_theta_r;
	Theta_s = sitec->obs_theta_s;
	fc = sitec->obs_fc;

	/* convert kg/m2 --> m3/m2 --> m3/m3 */
	vwc = (soilWobs) / (1000.0 * sitec->soil_depth);//inputs are in %
	//change soilw to soilWobs -- Y. He Oct/29/14
	*vwc_out = vwc;
	
	/* calculate psi */
	//*psi = sitec->psi_sat * pow((vwc/sitec->vwc_sat), sitec->soil_b);
	//printf("soil_psi_obs=%f, %f\n",*psi, sitec->psi_sat);
	
	//Changed Cosby Equation to van Genuchten Equation.Y.He
	if(vwc>Theta_s)
	{
	  vwc=Theta_s;
	}
	psi_temp1= pow((vwc-Theta_r)/(Theta_s-Theta_r),(Beta/(1-Beta)))-1;
	psi_temp2= -1/Alpha*pow(psi_temp1,1/Beta);//WARNING: Units in cm
	//convert cm --> m --> m2/s2 (*g) --> Pa (*density of water) --> MPa (/1000000)
	*psi = psi_temp2*0.01*9.8*1000/1000000; //units in MPa
	
	ws->soilw_sat_obs = Theta_s*sitec->soil_depth*1000;
	ws->soilw_fc_obs = fc*sitec->soil_depth*1000;
	//printf("psi_temp1=%f\n",psi_temp1);
	return(!ok);
}


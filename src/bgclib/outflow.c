/* 
outflow.c
daily hydrologic outflow

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.2 (final release)
See copyright.txt for Copyright information
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include "bgc.h"

int outflow(const siteconst_struct* sitec, const wstate_struct* ws,
wflux_struct* wf)
{
	/* calculates the outflow flux from the difference between soilwater
	and maximum soilwater */
	int ok=1;
	/* water in excess of saturation to outflow */
	//this was model output soil outflow calculated with modeled soil moisture
	if (ws->soilw > sitec->soilw_sat)  
        {
                wf->soilw_outflow = ws->soilw - sitec->soilw_sat;
        }
	/* slow drainage from saturation to field capacity */
	else if (ws->soilw > sitec->soilw_fc)
	{
		wf->soilw_outflow = 0.5 * (ws->soilw - sitec->soilw_fc);
	}
	/* otherwise, no outflow */
	else
	{
		wf->soilw_outflow = 0.0;
	}

	// to conserve mass of water, Y. He, Jun 08, 2015
	wf->soilw_outflow_dummy = wf->soilw_outflow + ws->soilw - ws->soilWobs;
	if (wf->soilw_outflow_dummy < 0.0)
	{
		wf->soilw_outflow_dummy = 0.0;
	}	
	return(!ok);
}


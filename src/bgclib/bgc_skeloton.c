#include "bgc.h"
signed char summary_sanity = INSANE;
int bgc (bgcin_struct * bgcin, bgcout_struct * bgcout, int mode)
{
  /* make zero-flux structures for use inside annual and daily loops */
  if (ok && make_zero_flux_struct (&zero_wf, &zero_cf, &zero_nf))

  /* atmospheric pressure (Pa) as a function of elevation (m) */
  if (ok && atm_pres (sitec.elev, &metv.pa))

  /* determine phenological signals */
  if (ok && prephenology (&ctrl, &epc, &sitec, &metarr, &phenarr))

  /* calculate the annual average air temperature for use in soil 
     temperature corrections. */

  /* if this simulation is using a restart file for its initial
     conditions, then copy restart info into structures */
  if (ok && ctrl.read_restart)
  else
    /* no restart file, user supplies initial conditions */

  /* zero water, carbon, and nitrogen source and sink variables */
  if (ok && zero_srcsnk (&cs, &ns, &ws, &summary))

  /* initialize the indicator for first day of current simulation, so
     that the checks for mass balance can have two days for comparison */
  first_balance = 1;

  /* mode == MODE_SPINUP only */
  if (mode == MODE_SPINUP)
      /* for simulations with fewer than 50 metyears, find the multiple of
         metyears that gets close to 100, use this as the block size in
         spinup control */
      /* initialize spinup control variables */
  
/* do loop for spinup. will only execute once for MODE_MODEL */
  do
    { /* begin the annual model loop */
      for (simyr = 0; ok && simyr < tmpyears; simyr++)
	{
	  if (mode == MODE_MODEL)
	  /* reset the simple annual output variables for text output */

	  /* set current month to 0 (january) at the beginning of each year */
	  curmonth = 0;

	  if (mode == MODE_SPINUP)
	    {
	      /* calculate scaling for N additions (decreasing with
	         time since the beginning of metcycle = 0 block */
	      naddfrac = 1.0 - ((double) simyr / (double) nblock);
	    }

	  /* test whether metyr needs to be reset for cyclic input */
	  if (metyr == ctrl.metyears)

	  if (mode == MODE_MODEL) /* output to screen to indicate start of simulation year */
	  else if (mode == MODE_SPINUP) /* output to screen to indicate start of simulation year */

	  /* set the max lai variable, for annual diagnostic output */

	  if (mode == MODE_MODEL)
	    {
	      /* atmospheric CO2 and Ndep handling */
	      if (!(co2.varco2))
		  /* constant CO2, constant Ndep */
	      else
		{
		  /* when varco2 = 1, use file for co2 */
		  /* when varco2 = 2, use the constant CO2 value, but vary Ndep */
		  /* increasing CO2, constant Ndep */
		}
	    }
	  else if (mode == MODE_SPINUP)
	    {
	      /* atmospheric concentration of CO2 (ppm) */
	      /* Always assign a fixed CO2 value for spinups */
	    }

	  /* begin the daily model loop */
	  for (yday = 0; ok && yday < 365; yday++)
	    {
	      /* Test for very low state variable values and force them
	         to 0.0 to avoid rounding and floating point overflow errors */
	      if (ok && precision_control (&ws, &cs, &ns))

	      /* set the day index for meteorological and phenological arrays */
	      metday = metyr * 365 + yday;

	      /* zero all the daily flux variables */

	      /* daily meteorological variables from metarrays */
	      if (ok && daymet (&metarr, &metv, &ws, metday))

	      /* soil temperature correction using difference from annual average tair */

	      /* daily phenological variables from phenarrays */
	      if (ok && dayphen (&phenarr, &phen, metday))

	      /* test for the annual allocation day */
	      if (phen.remdays_litfall == 1)	annual_alloc = 1;
	      else	annual_alloc = 0;

	      /* phenology fluxes */
	      if (ok && phenology (&epc, &phen, &epv, &cs, &cf, &ns, &nf))

	      /* calculate leaf area index, sun and shade fractions, and specific
	         leaf area for sun and shade canopy fractions, then calculate
	         canopy radiation interception and transmission */
	      if (ok && radtrans (&cs, &epc, &metv, &epv, sitec.sw_alb))
	      
	      /* update the ann max LAI for annual diagnostic output */
	      if (epv.proj_lai > epv.ytd_maxplai) epv.ytd_maxplai = epv.proj_lai;

	      /* precip routing (when there is precip) */
	      if (metv.prcp && prcp_route (&metv, epc.int_coef, epv.all_lai, &wf))

	      /* snowmelt (when there is a snowpack) */
	      if (ok && ws.snoww && snowmelt (&metv, &wf, ws.snoww))

	      /* bare-soil evaporation (when there is no snowpack) */
	      if (ok && !ws.snoww && baresoil_evap (&metv, &wf, &epv.dsr))

	      /* soil water potential *///change ws.soilw to ws.soilWobs
	      if (soilpsi (&sitec, ws.soilWobs, &epv.psi, &epv.vwc, &ws))

	      /* daily maintenance respiration */
	      if (ok && maint_resp (&cs, &ns, &epc, &metv, &cf, &epv))

	      /* begin canopy bio-physical process simulation */
	      /* do canopy ET calculations whenever there is leaf area
	         displayed, since there may be intercepted water on the 
	         canopy that needs to be dealt with */
	      if (ok && cs.leafc && metv.dayl)
		{  /* conductance and evapo-transpiration */
		  if (ok && canopy_et (&metv, &epc, &epv, &wf, 1))
		}
	      /* do photosynthesis only when it is part of the current
	         growth season, as defined by the remdays_curgrowth flag.  This
	         keeps the occurrence of new growth consistent with the treatment
	         of litterfall and allocation */
	      if (ok && cs.leafc && phen.remdays_curgrowth && metv.dayl)
		{if (total_photosynthesis (&metv, &epc, &epv, &cf,&psn_sun, &psn_shade))}
		/* end of photosynthesis calculations */
	      else
		  epv.assim_sun = epv.assim_shade = 0.0;

	      /* nitrogen deposition and fixation */

	      /* calculate outflow */
	      if (ok && outflow (&sitec, &ws, &wf))

	      /* daily litter and soil decomp and nitrogen fluxes 
 		- changed from metv.tsoil to metv.soilTobs */
	      if (decomp (metv.soilTobs, &epc, &epv, &sitec, &cs, &cf, &ns,&nf, &nt))

	      /* Daily allocation gets called whether or not this is a
	         current growth day, because the competition between decomp
	         immobilization fluxes and plant growth N demand is resolved
	         here.  On days with no growth, no allocation occurs, but
	         immobilization fluxes are updated normally */
	      if (mode == MODE_MODEL)
		  if (daily_allocation (&cf, &cs, &nf, &ns, &epc, &epv, &nt, &ws, &phen))
	      else if (mode == MODE_SPINUP)
		{
		  /* spinup control */
		  /* in the rising limb, use the spinup allocation
 			code that supplements N supply */
		  if (!steady1 && rising && metcycle == 0)
		      if (daily_allocation (&cf, &cs, &nf, &ns, &epc, &epv,&nt, &ws, &phen, naddfrac))
		  else
		      if (daily_allocation (&cf, &cs, &nf, &ns, &epc, &epv,&nt, &ws, &phen))
		}

	      /* reassess the annual turnover rates for livewood --> deadwood,
	         and for evergreen leaf and fine root litterfall. This happens
	         once each year, on the annual_alloc day (the last litterfall day) */
	      if (ok && annual_alloc)
		  if (ok && annual_rates (&epc, &epv))

	      /* daily growth respiration */
	      if (ok && growth_resp (&epc, &cf))

	      /* daily update of the water state variables */
	      if (ok && daily_water_state_update (&wf, &ws))

	      /* daily update of carbon state variables */
	      if (daily_carbon_state_update (&cf, &cs, annual_alloc, epc.woody, epc.evergreen))

	      /* daily update of nitrogen state variables */
	      if (daily_nitrogen_state_update (&nf, &ns, annual_alloc, epc.woody, epc.evergreen))

	      /* calculate N leaching loss.  This is a special state variable
	         update routine, done after the other fluxes and states are
	         reconciled in order to avoid negative sminn under heavy leaching
	         potential */
	      if (ok && nleaching (&ns, &nf, &ws, &wf))

	      /* calculate daily mortality fluxes and update state variables */
	      /* this is done last, with a special state update procedure, to
	         insure that pools don't go negative due to mortality fluxes
	         conflicting with other proportional fluxes */
	      if (ok && mortality (&epc, &cs, &cf, &ns, &nf))

	      /* test for water balance */
	      if (ok && check_water_balance (&ws, first_balance))

	      /* test for carbon balance */
	      if (ok && check_carbon_balance (&cs, first_balance))

	      /* test for nitrogen balance */
	      if (ok && check_nitrogen_balance (&ns, first_balance))

	      /* calculate carbon summary variables */
	      if (ok && csummary (&cf, &cs, &summary))

	      /* calculate water summary variables */
	      if (ok && wsummary (&ws, &wf, &summary))

	      /* DAILY OUTPUT HANDLING */
	      /* fill the daily output array if daily output is requested,
	         or if the monthly or annual average of daily output variables
	         have been requested */
	      if (ok && dayout)
		{  /* fill the daily output array */
		  for (outv = 0; outv < ctrl.ndayout; outv++)
		}
	      /* only write daily outputs if requested */
	      if (ok && ctrl.dodaily)
		  /* write the daily output array to daily output file */

	      /* MONTHLY OUTPUTS */
	      /* MONTHLY AVERAGE OF DAILY OUTPUT VARIABLES */
	      if (ctrl.domonavg)
		{
		  /* update the monthly average array */
		  for (outv = 0; outv < ctrl.ndayout; outv++)
		  
		  /* if this is the last day of the current month, output... */
		  if (yday == endday[curmonth])
		    {
		      /* finish the averages */
		      /* write to file */
		      /* reset monthly average variables for next month */
		      /* increment current month counter */
		      curmonth++;
		    }
		}

	      /* ANNUAL AVERAGE OF DAILY OUTPUT VARIABLES */
	      if (ctrl.doannavg)
		{
		  /* update the annual average array */
		  for (outv = 0; outv < ctrl.ndayout; outv++)

		  /* if this is the last day of the year, output... */
		  if (yday == 364)
		    {
		      /* finish averages */
		      /* write to file */
		      /* reset annual average variables for next month */
		    }
		}

	      if (mode == MODE_MODEL)
		  /* very simple annual summary variables for text file output */
	      else if (mode == MODE_SPINUP)
		{
		  /* spinup control */
		  /* keep a tally of total soil C during successive
		     met cycles for comparison */
		}

	      /* at the end of first day of simulation, turn off the 
	         first_balance switch */
	      if (first_balance)
		first_balance = 0;
	    }/* end of daily model loop */

	  /* ANNUAL OUTPUT HANDLING */
	  /* only write annual outputs if requested */
	  if (ok && ctrl.doannual)
	    {
	      /* fill the annual output array */
	      /* write the annual output array to annual output file */
	    }

	  if (mode == MODE_MODEL && bgcout->bgc_ascii)
	      /* write the simple annual text output */
	  metyr++;

	  if (mode == MODE_SPINUP)
	      /* spinup control */     spinyears++;

	}/* end of annual model loop */

      if (mode == MODE_SPINUP)
	{
	  /* spinup control */
	  /* if this is the third pass through metcycle, do comparison */
	  /* first block is during the rising phase */
	  if (!steady1 && metcycle == 2)
	      /* convert tally1 and tally2 to average daily soilc */
	  /* second block is after supplemental N turned off */
	  else if (steady1 && metcycle == 2)
	    {
	      /* convert tally1 and tally2 to average daily soilc */
	      /* if rising above critical rate, back to steady1=0 */
	    }
	  else
	      metcycle++;
	}
    }  /* end of do block, test for steady state */
  while (mode == MODE_SPINUP && (!(steady1 && steady2)
	     && (spinyears < ctrl.maxspinyears || metcycle != 0)));

  /* mode == MODE_SPINUP only */
  if (mode == MODE_SPINUP)
      /* save some information on the end status of spinup */

  /* RESTART OUTPUT HANDLING */
  /* if write_restart flag is set, copy data to the output restart struct */
  /* Removed 'write_restart' restriction to ensure that restart data are */
  /* available for spin and go operation.  WMJ 3/16/2005 */
  if (ok)
      if (restart_output (&ctrl, &ws, &cs, &ns, &epv, metyr,&(bgcout->restart_output)))

  /* free phenology memory */
  /* free memory for local output arrays */
  /* print timing info if error */
}

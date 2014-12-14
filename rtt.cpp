#include "project.h"
#include "rtt.h"
#include <stdio.h>
#include "rprintf.h"


#define         count_period      (16*EXT_OC)
#define         compens_delay     5

char refdate[7]={'8','0','0','1','0','1','0'};

//*--------------------------------------------------------------------------------------
//* Function Name       : RTPPRES_Correction
//* Object              : Compensation process
//* Input Parameters    : Delay for the next compensation in second.
//* Output Parameters   : none.
//*--------------------------------------------------------------------------------------
// mthomas void RTPPRES_Correction (int next_compensation_delay)
__ramfunc void RTPPRES_Correction (int next_compensation_delay)
{

  AT91PS_PMC     pPMC = AT91C_BASE_PMC;

  unsigned int freq_rc, n;

/* FIRST STEP: We are going to start the embedded PMC process called Timing Interval Acquisition
in application note. For that, we have to stop and start the Mayn Oscillator */
        // Switch to Slow
        pPMC->PMC_MCKR = (0x4 | AT91C_PMC_CSS_SLOW_CLK);
        while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

        // Modify PRES value down to 0
        pPMC->PMC_MCKR = AT91C_PMC_CSS_SLOW_CLK;
        while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

        // Stop the PLL
        pPMC->PMC_PLLR = 0;

        // Stop the Main Oscillator
        pPMC->PMC_MOR = 0;

        // Start the Main Oscillator
        // mthomas: pPMC->PMC_MOR = (( AT91C_CKGR_OSCOUNT & (0x06 <<8) | AT91C_CKGR_MOSCEN ));
        pPMC->PMC_MOR = ( AT91C_CKGR_OSCOUNT & (0x06 <<8) ) | AT91C_CKGR_MOSCEN;

        // Waiting necessary Main Oscillator startup time
        while(!(pPMC->PMC_SR & AT91C_PMC_MOSCS));

        // Switch to the Mayn Oscillator clock
        pPMC->PMC_MCKR = AT91C_PMC_CSS_MAIN_CLK;
        while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

        // Start the PLL and waiting necessary settling time
        // The PLLCOUNT value has been calculated according to the following parameters:
        /* PLL Filter components R=1.5k, C1=10 nF, C2=1nF, Fn=18kHz,
        Settling Time (max)= 855 µs, RC Oscillator (max)=42 kHz      */
        pPMC->PMC_PLLR = ((AT91C_CKGR_DIV & 0x0E) |
                         (AT91C_CKGR_PLLCOUNT & (0x28 << 8)) |
                         (AT91C_CKGR_MUL & (0x48 << 16)));
        while(!(pPMC->PMC_SR & AT91C_PMC_LOCK));

        // Switch to the PLL Clock
        // Set the PRES field to divide the PLL output by  2
        pPMC->PMC_MCKR = AT91C_PMC_PRES_CLK_2 ;
        while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

        // Switch to PLL output. After this step we're running from the PLL
        pPMC->PMC_MCKR = ((pPMC->PMC_MCKR)|AT91C_PMC_CSS_PLL_CLK);
         while(!(pPMC->PMC_SR & AT91C_PMC_MCKRDY));

/* SECOND STEP: Evaluating real RC-Oscillator working frequency according to the Main Oscillator */

         freq_rc =  (count_period/((*AT91C_CKGR_MCFR & AT91C_CKGR_MAINF)));

        /* Waiting new end of cycle before to modify the RTPPRES field vs. MAINF value
         (take a look at dedicated ATMEL' s Application Note)
        !! IMPORTANT !!: Because there is this constraint, this task should be used in interrupt mode
        In this example, we're using pulling method (It is not the best choice, of course)
        to wait next event just before upgrade RTT prescaler value */
        n = *AT91C_RTTC_RTVR;

        while (n == *AT91C_RTTC_RTVR);
/*				/ \
				 |
			The user has to take care in having the smallest delay between the end of the while
			and the *AT91C_RTTC_RTMR modification. In best case, this part must work from the internal
			RAM in order to fetch without wait state @ full speed.
				 |
				\ /
*/
        rprintf("freq_rc:%d\n", freq_rc );
        *AT91C_RTTC_RTMR  = 3;//freq_rc; // milliseconds

        // Preset the next compensation event
        *AT91C_RTTC_RTAR = (n) + next_compensation_delay;

}


static unsigned int  Date_preset (void)
{

/*
unsigned int Data = 0;
unsigned int Second = 0;

   refdate[0]=Data/10 +'0';
   refdate[1]= Data - ((refdate[0]-'0')*10) +'0';
   refdate[2]=Data/10+'0';
   refdate[3]=Data - ((refdate[2]-'0')*10) +'0';
   refdate[4]=Data/10+'0';
   refdate[5]=Data - ((refdate[4]-'0')*10) +'0';
   Second=Data*3600;
   Second+=Data*60;
   Second+=Data;
   Second += date_to_days(refdate) * 86400;
*/
   return (0);
}

static void reset_rtt()
{
  *AT91C_RTTC_RTVR = 0;
}

void delay(int t)
{
  unsigned now = *AT91C_RTTC_RTVR;
  while(*AT91C_RTTC_RTVR < now + t);
}

static int DateTime_request ()
{
  unsigned int second;
    second += *AT91C_RTTC_RTVR;
    return second;
}

void init()
{
	unsigned int ref_time_InSecond;
   // The ref_time_InSecond must be hold all the time in application
   
	ref_time_InSecond = Date_preset();
	// First Step: Reset RTT and Start it
	*AT91C_RTTC_RTMR = 0x40000;
	RTPPRES_Correction (compens_delay);
}

unsigned int millis()
{
	return DateTime_request() / 10;
}
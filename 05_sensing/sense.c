/**
 * \file
 *         Sensing temperature, humidity and light intensity in the vicinity of Tmote-sky
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"

#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"

#include <stdio.h>
#include <math.h>

/*---------------------------------------------------------------------------*/
PROCESS(sensor_acq_process,"Sensor Acquisition");
AUTOSTART_PROCESSES(&sensor_acq_process);

PROCESS_THREAD(sensor_acq_process,ev,data)
{ 
      static struct etimer et;
      static int val;
      static float s = 0;
      static int dec;
      static float frac;

      PROCESS_BEGIN();

      printf("Starting Sensor Example.\n");
      
      while(1)
      {
	   etimer_set(&et, CLOCK_SECOND * 2);
	   SENSORS_ACTIVATE(light_sensor);
      	   SENSORS_ACTIVATE(sht11_sensor);
        
	   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));


           val = sht11_sensor.value(SHT11_SENSOR_TEMP);
      	   if(val != -1) 
      	   {
		s= ((0.01*val) - 39.60);
      	  	dec = s;
      	  	frac = s - dec;
      	  	printf("\nTemperature=%d.%02u C (%d)\n", dec, (unsigned int)(frac * 100),val);               
           }

	   val=sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
	   if(val != -1) 
      	   {
		s= (((0.0405*val) - 4) + ((-2.8 * 0.000001)*(pow(val,2))));  
      	  	dec = s;
      	  	frac = s - dec;
      	  	printf("Humidity=%d.%02u %% (%d)\n", dec, (unsigned int)(frac * 100),val);               
           }

           val = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
      	   if(val != -1) 
      	   {
      		s = (float)(val * 0.4071);
      	  	dec = s;
      	  	frac = s - dec;
      	  	printf("Light=%d.%02u lux (%d)\n", dec, (unsigned int)(frac * 100),val);               
           } 
	
	 //  etimer_reset(&et);
    	   SENSORS_DEACTIVATE(light_sensor);
    	   SENSORS_DEACTIVATE(sht11_sensor);

      } //end of while
    
      PROCESS_END();
}


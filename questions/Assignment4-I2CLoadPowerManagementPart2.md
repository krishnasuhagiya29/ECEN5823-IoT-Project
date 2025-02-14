Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.* 

1. What is the average current per period? (Remember, once you have measured your average current, average current is average current over all time. Average current doesn’t carry with it the units of the timespan over which it was measured).
   Answer: 16.26 uA
   <br>Screenshot:  
   ![Avg_current_per_period](Images/assignment4_Q1.jpg)  

2. What is the ave current from the time we sleep the MCU to EM3 until we power-on the 7021 in response to the LETIMER0 UF IRQ?
   Answer: 
   <br>Screenshot: 1.79 uA 
   ![Avg_current_LPM_Off](Images/assignment4_Q2.jpg)  

3. What is the ave current from the time we power-on the 7021 until we get the COMP1 IRQ indicating that the 7021's maximum time for conversion (measurement) has expired.
   Answer: 121.54 uA
   <br>Screenshot:  
   ![Avg_current_LPM_Off](Images/assignment4_Q3.jpg)  

4. How long is the Si7021 Powered On for 1 temperature reading?
   Answer: 98.30 ms
   <br>Screenshot:  
   ![duration_lpm_on](Images/assignment4_Q4.jpg)  

5. Given the average current per period from Q1, calculate the operating time of the system for a 1000mAh battery? - ignoring battery manufacturers, efficiencies and battery derating - just a first-order calculation.
   Answer (in hours): 61500.61 hours
   
6. How has the power consumption performance of your design changed since the previous assignment?
   Answer: By implementing sleep mode (EM1) between I2C transfers and replacing the polling-based timer wait function with an interrupt-based approach, the system's power consumption has significantly decreased. 
   Previously, the average current per period was 159.05 µA, which has now been reduced to 16.26 µA. As a result, the estimated operating time of the system on a 1000mAh battery has increased from 6287.33 hours to 61500.61 hours - an improvement of nearly 10 times. 
   



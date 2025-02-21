Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.*

1. Provide screen shot verifying the Advertising period matches the values required for the assignment.
   <br>Screenshot:  
   ![advertising_period](Images/assignment5_Q1.jpg)  

2. What is the average current between advertisements - i.e. when the MCU is sleeping in EM2. (Don't measure an interval with a LETIMER UF event)?
   Answer: 1.34 uA
   <br>Screenshot:  
   ![avg_current_between_advertisements](Images/assignment5_Q2.jpg)  

3. What is the peak current of an advertisement? 
   Answer: 28.05 mA
   <br>Screenshot:  
   ![peak_current_of_advertisement](Images/assignment5_Q3.jpg)  

4. Provide screen shot showing the connection interval setting. Does the connection interval match the values you requested in your slave(server) code, or the master's(client) values?.
   <br>Screenshot: 
   ![connection_interval](Images/assignment5_Q4.jpg)  

5. What is the average current between connection intervals - i.e. when the MCU is sleeping in EM2. (Don't measure an interval with a LETIMER UF event)?
   Answer: 2.01 uA
   <br>Screenshot:  
   ![avg_current_between_connection_intervals](Images/assignment5_Q5.jpg)  

6. If possible, provide screen shot verifying the slave latency matches what was reported when you logged the values from event = gecko_evt_le_connection_parameters_id.
   Answer: It can be observed from the screenshot that there is a connection event right in the middle of when the 7021 is taking a temp measurement. The indication is queued and will be transmitted to the Client in the next connection event. So, it matches with the latency value 0. The peak current is higher on that 3rd connection event. 
   <br>Screenshot:  
   ![slave_latency](Images/assignment5_Q6.jpg)  


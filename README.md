# Embedded\_Sysdev\_2025



This repository holds all coding assignments from the course Sulautettujen järjestelmien ohjelmistokehitys held during fall 2025 at Oulun Ammattikorkeakoulu.

Every assignment was divided in three parts.



## Week 1



No coding assignments on week 1.



## Week 2



Week 2 assignment was to get familiar with RTOS, tasks and state machines.



In part one, we create tasks to blink colored LEDs (green, yellow, red) and create a state machine to handle the order of blinking the lights.



In part two, we add a button, button\_0. When the button is pressed, the light sequence is paused. When pressed again, the light sequence will continue.



In part three, we add four more buttons. When the light sequence is paused, pressing buttons 1-3 will turn a LED on or off. Each button has different color: button\_1 is red, button\_2 is yellow, button\_3 is green. Button\_4 will start or stor blinking yellow light.


## Week 3


Week 3 assignment was to get familiar with signaling tasks and using serial port data.



In part one, we added a task to read UART serial port data and save it to FIFO buffer. Then we added a dispatcher task to handle the data and signal correct LED tasks. Finally we added a release signal from the LED tasks to the dispatcher to help with the timings.



In part two, we refactored the button functionality. Now button_0 will add a pre-determined sequence (RYGYR) to the FIFO buffer. Buttons 1-3 will add a single letter depending on the color, as in week 1, to the FIFO buffer. These buttons will then signal the dispatcher. Button_4 signals a separate blink_task that will blink the yellow LED five times in a row.



In part three, we refactored the code again. Now we insert the color and time we want the LED to be turned on, for example:
`R, 1000`
will keep the red LED on for 1000 ms.

The same functionality is hard-coded to buttons 1-3, keeping the corresponding LED on for 1000 ms. Button_0 will send a sequence of `RYGYR` to the FIFO, keeping each LED on for 1000 ms.

## Week 4


Week 4 we get familiar with timing our programs performance and debugging.



In part one, we time add timing to our LED tasks and time how long each task takes. The total time for the light sequence is also calculated. We time the program twice: first with debugging prints (for example `printk("Red LED on\n")`) and second time with these prints turned off. Both times we run the same sequence `RYGYR,1`.



![Screenshot of console, showing the timing of program with debugging prints turned on.](/images/w4p1_timing_prints.png)

Here we can see it takes between 1,4 to 1,9 ms to run one task. The whole sequence takes about 8,5 ms.



![Screenshot of console, showing the timing of program with debugging prints turned off.](/images/w4p1_timing_no_prints.png)

Removing the prints improves our performance significantly. Now each task takes between 0,6 to 1 ms to run, with the whole sequence taking just 3,7 ms.



In part two, we add a separat debug-task to handle the printing of debug data. Now the UART-message, dispatcher parsed data and timing of tasks is printed only from the debug task.
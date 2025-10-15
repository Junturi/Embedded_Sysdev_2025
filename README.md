# Embedded\_Sysdev\_2025



This repository holds all coding assignments from the course Sulautettujen järjestelmien ohjelmistokehitys held during fall 2025 at Oulun Ammattikorkeakoulu, along with my solutions. Every weekly assignment was divided in parts, worth one point. We were free to choose which parts we wanted to complete, but it was possible to earn a maximum of three points per week.


We used [nRD5340 Audio Development Kit](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-Audio-DK) as our development platform.



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



![Screenshot of terminal, showing the timing of program with debugging prints turned on.](/images/w4p1_timing_prints.png)

Here we can see it takes between 1,4 to 1,9 ms to run one task. The whole sequence takes about 8,5 ms.



![Screenshot of terminal, showing the timing of program with debugging prints turned off.](/images/w4p1_timing_no_prints.png)

Removing the prints improves our performance significantly. Now each task takes between 0,6 to 1 ms to run, with the whole sequence taking just 3,7 ms.



In part two, we add a separat debug-task to handle the printing of debug data. Now the UART-message, dispatcher parsed data and timing of tasks is printed only from the debug task.


In part three, additional debugging prints and timing of other parts of the program were added. Debug-task will now print out the time taken by the uart-task and dispatcher-task.


## Week 5

Week 5 we start unit testing with GoogleTest. All the test code can be found in the [unit_test folder](unit_testing/)


In part one, we modify provided [TimeParser.cpp](unit_testing/TimeParser.cpp)-file and add null pointer check and boundary checks. With the boundary checks we ensure that the parsed time is valid: seconds or minutes are not negative or over 59 and hours are not negative or over 23. If the parsed time is invalid, we return `TIME_VALID_ERROR`. If the parsed time is valid, we return the time calculated in seconds.


After the [TimeParser.cpp](unit_testing/TimeParser.cpp)-file is modified, we write appropriate tests in the [TimeParserTest.cpp](unit_testing/test_cases/TimeParserTest.cpp)-file. We test for simple cases of:
- Is the time parsed and seconds calculated correctly
- Is the pointer null
- Is the parsed seconds, minutes and hours
    - valid
    - not negative


![Screenshot of terminal, showing the tests run. All 8 tests were successful](/images/w5p1_simple_tests.png)

Above we can see that all 8 tests were successful.


In part two, we add more test cases to [TimeParserTest.cpp](unit_testing/test_cases/TimeParserTest.cpp). After writing the tests, we add functionality to [TimeParser.cpp](unit_testing/TimeParser.cpp) so that the tests will pass. The previous tests were refactored to use `EXPECT_EQ` instead of `ASSERT_EQ`. Each test unit now includes both test for valid and invalid input. Tests of similar functionality were grouped under one test case (for example testing invalid second, minute and hour values).
The additional tests are:
- Is the input 6 characters long
- Is the return value > 0
- Is the input only digits


In part three, we add additional test suite [UartParserTest.cpp](unit_testing/test_cases/UartParserTest.cpp) and write tests for UART message parsing. We test for simple cases of:
- Does the message include a comma
- Does the message contain only letters G, Y or R and numbers
- Is the time parsed from the message not negative

![Screenshot of terminal, showing the tests run. All 10 tests were successful](/images/w5p1_time_and_uart_parser_tests.png)

Above we can see that all 10 tests were succesful.

Finally, the function `time_parse` from TimeParser was implemented in the traffic lights project. I ran into issues while implementing the UartParser and it has not been implemented so far.



## Week 6


Week 6 we dip our toes into test automation with Robot Framework. Robot Framework uses Python, and all dependencies can be installed easily with `pip install -r requirements.txt`. It is recommended to use virtual enviroments. The `requirements.txt` file, alongside with all the Robot Framework tests, can be found in [robot_framework folder](robot_framework/)


In part one, we want to test our `time_parse` function. We start by creating two test cases in [tests.robot](robot_framework/tests.robot). We test for correct and incorrect time by sending a sequence via COM9 port. The nRF board will receive the sequence and send back a response via the same port. This response is then compared to our expected response. If they match, the test will pass.



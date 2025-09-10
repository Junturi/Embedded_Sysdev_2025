// Viikolla 2 tavoittelen pistemäärää 3/3.
// Kaikki kolme tehtäväosiota on toteutettu ja toimii toivotulla tavalla.

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

// Configure buttons
#define BUTTON_0 DT_ALIAS(sw0)
#define BUTTON_1 DT_ALIAS(sw1)
#define BUTTON_2 DT_ALIAS(sw2)
#define BUTTON_3 DT_ALIAS(sw3)
#define BUTTON_4 DT_ALIAS(sw4)

static const struct gpio_dt_spec button_0 = GPIO_DT_SPEC_GET_OR(BUTTON_0, gpios, {0});
static struct gpio_callback button_0_data;

static const struct gpio_dt_spec button_1 = GPIO_DT_SPEC_GET_OR(BUTTON_1, gpios, {1});
static struct gpio_callback button_1_data;

static const struct gpio_dt_spec button_2 = GPIO_DT_SPEC_GET_OR(BUTTON_2, gpios, {2});
static struct gpio_callback button_2_data;

static const struct gpio_dt_spec button_3 = GPIO_DT_SPEC_GET_OR(BUTTON_3, gpios, {3});
static struct gpio_callback button_3_data;

static const struct gpio_dt_spec button_4 = GPIO_DT_SPEC_GET_OR(BUTTON_4, gpios, {4});
static struct gpio_callback button_4_data;

// Confirgure LED pins
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);

// Initialize thread definitions for the LEDs
#define STACKSIZE 500
#define PRIORITY 5

// Initialize red LED thread
void red_led_task(void *, void *, void *);
K_THREAD_DEFINE(red_thread,STACKSIZE, red_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// Initialize green LED thread
void green_led_task(void *, void *, void *);
K_THREAD_DEFINE(green_thread,STACKSIZE, green_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// Initialize yellow LED thread
void yellow_led_task(void *, void *, void *);
K_THREAD_DEFINE(yellow_thread,STACKSIZE, yellow_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// Declare functions and global variables
int initialize_button(void);
int initialize_leds(void);
int led_state = 0; // State machine for the LED sequence
// 1 -> red
// 2 -> yellow
// 3 -> green
// 4 -> pause
int last_led_state = 0; // Save the last state
int direction = 0; // Determine if we move from yellow to red (up) or green (down)
// 1 -> up
// 2 -> down
bool red_led_on = false; // Boolean variable to track if the LED is on or not
bool yellow_led_on = false;
bool green_led_on = false;
bool button_1_pressed = false; // Boolean variable to track if button is pressed or not
bool button_2_pressed = false;
bool button_3_pressed = false;
bool button_4_pressed = false;
bool blink_yellow_led = false; // Boolean variable to track if we want to blink the yellow light or not

// Button interrupt handlers
void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 0 pressed\n");
        if (led_state != 4) { // If the state is not paused
                last_led_state = led_state; // Save the current state of the light sequence
                led_state = 4; // Set the state to paused
        }
        else if (led_state == 4) { // If the state is paused
                led_state = last_led_state; // Set the current state to last state before pausing
        }
        blink_yellow_led = false;
}

void button_1_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 1 pressed\n");
        button_1_pressed = true; // Set the flags
        blink_yellow_led = false;
}

void button_2_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 2 pressed\n");
        button_2_pressed = true;
        blink_yellow_led = false;
}

void button_3_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 3 pressed\n");
        button_3_pressed = true;
        blink_yellow_led = false;
}

void button_4_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 4 pressed\n");
        button_4_pressed = true;  
        if (blink_yellow_led == false) { // Check if yellow LED is alredy blinking
                blink_yellow_led = true;
        }
        else if (blink_yellow_led == true) {
                blink_yellow_led = false;
        }                          
}

int main(void)
{
        // In main, we only initialize the LEDs and button
        initialize_leds();

        int ret = initialize_button();
	if (ret < 0) {
		return 0;
	}

	while (1) {
		k_msleep(10); // sleep 10ms
	}

        return 0;
}

int initialize_button(void) {
        int ret;

        //Initialize button 0
	if (!gpio_is_ready_dt(&button_0)) {
		printk("Error: button 0 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_0, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin button 0\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_0, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin button 0\n");
		return -1;
	}

	gpio_init_callback(&button_0_data, button_0_handler, BIT(button_0.pin));
	gpio_add_callback(button_0.port, &button_0_data);
	printk("Set up button 0 ok\n");

        //Initialize button 1
	if (!gpio_is_ready_dt(&button_1)) {
		printk("Error: button 1 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_1, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin button 1\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_1, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin button 1\n");
		return -1;
	}

	gpio_init_callback(&button_1_data, button_1_handler, BIT(button_1.pin));
	gpio_add_callback(button_1.port, &button_1_data);
	printk("Set up button 1 ok\n");

        //Initialize button 2
	if (!gpio_is_ready_dt(&button_2)) {
		printk("Error: button 2 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_2, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin button 2\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_2, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin button 2\n");
		return -1;
	}

	gpio_init_callback(&button_2_data, button_2_handler, BIT(button_2.pin));
	gpio_add_callback(button_2.port, &button_2_data);
	printk("Set up button 2 ok\n");
	
        //Initialize button 3
	if (!gpio_is_ready_dt(&button_3)) {
		printk("Error: button 3 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_3, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin button 3\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_3, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin button 3\n");
		return -1;
	}

	gpio_init_callback(&button_3_data, button_3_handler, BIT(button_3.pin));
	gpio_add_callback(button_3.port, &button_3_data);
	printk("Set up button 3 ok\n");

        //Initialize button 4
	if (!gpio_is_ready_dt(&button_4)) {
		printk("Error: button 4 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_4, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin button 4\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_4, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin button 4\n");
		return -1;
	}

	gpio_init_callback(&button_4_data, button_4_handler, BIT(button_4.pin));
	gpio_add_callback(button_4.port, &button_4_data);
	printk("Set up button 4 ok\n");

	return 0;
}

// Initialize LEDs
int initialize_leds(void) {

        // Initialize pins
        int ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
        if (ret <0) {
                printk("Error: Red LED configure failed\n");
                return ret;
        }

        ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
        if (ret <0) {
                printk("Error: Green LED configure failed\n");
                return ret;
        }

        // Set LEDs off
        gpio_pin_set_dt(&red, 0); 
        gpio_pin_set_dt(&green, 0);

        led_state = 1; // Set the state of the LED sequence

        printk("LED initialized ok\n");

        return 0;
}

// Task for handling red LED
void red_led_task(void *, void *, void *) {
        printk("Red LED thread started\n");
        while (true) {
                if (led_state == 1) {
                        gpio_pin_set_dt(&red, 1); // Set LED on
                        red_led_on = true; // Track that the LED is on
                        printk("Red on\n");
                        k_sleep(K_SECONDS(1)); // Sleep for 1 second
                        gpio_pin_set_dt(&red, 0); // Set LED off
                        red_led_on = false; // Track that the LED is off
                        printk("Red off\n");
                        direction = 2; // Track if we are moving up (1, towards red) or down (2, towards green)
                        if (led_state == 4) { // If the state is 4 = pause
                                gpio_pin_set_dt(&red, 1); // Set LED on and stay on
                                red_led_on = true;
                                printk("Red on, pausing\n");
                                k_msleep(100); // Prevent busy-looping
                        }
                        else if (led_state != 4) { // If state is not 4
                                led_state = 2; // Move to the next state
                        }
                }
                // Turn red LED on or off
                if (led_state == 4 && button_1_pressed == true) { // If state is 4 and button_1 was pressed 
                        gpio_pin_set_dt(&green, 0); // Turn off other LED colors
                        yellow_led_on = false; // Track that other colors are off
                        green_led_on = false;
                        if (red_led_on == false) { // If the red LED is not on
                                gpio_pin_set_dt(&red, 1); // Set LED on and stay on
                                red_led_on = true;
                                printk("Red on\n");
                        }
                        else if (red_led_on == true) { // If the red LED is on
                                gpio_pin_set_dt(&red, 0); // Set LED off and stay off
                                red_led_on = false;
                                printk("Red off\n");
                        }
                        button_1_pressed = false; // Set the flag
                }
                k_yield(); // Yield and move to the end of the task line
        }
}

void yellow_led_task(void *, void *, void *) {
        printk("Yellow LED thread started\n");
        while (true) {
                if (led_state == 2) {
                        gpio_pin_set_dt(&red, 1);
                        gpio_pin_set_dt(&green, 1);
                        yellow_led_on = true;
                        printk("Yellow on\n");
                        k_sleep(K_SECONDS(1));
                        gpio_pin_set_dt(&red, 0);
                        gpio_pin_set_dt(&green, 0);
                        yellow_led_on = false;
                        printk("Yellow off\n");
                        if (led_state == 4) {
                                gpio_pin_set_dt(&red, 1);
                                gpio_pin_set_dt(&green, 1);
                                yellow_led_on = true;
                                printk("Yellow on, pausing\n");
                                k_msleep(100);
                        }

                        else if (led_state != 4) {
                                // Determine if we move to red or green light next
                                if (direction == 2) {
                                        led_state = 3;
                                }
                                else if (direction == 1) {
                                        led_state = 1;
                                }
                        }
                }
                // Turn yellow LED on or off
                if (led_state == 4 && button_2_pressed == true) { 
                        gpio_pin_set_dt(&red, 0); 
                        gpio_pin_set_dt(&green, 0);
                        red_led_on = false;
                        green_led_on = false;
                        if (yellow_led_on == false) {
                                gpio_pin_set_dt(&red, 1);
                                gpio_pin_set_dt(&green, 1);
                                yellow_led_on = true;
                                printk("Yellow on\n");
                        }
                        else if (yellow_led_on == true) {
                                gpio_pin_set_dt(&red, 0);
                                gpio_pin_set_dt(&green, 0);
                                yellow_led_on = false;
                                printk("Yellow off\n");
                        }
                        button_2_pressed = false;                               
                }
                // Blinking yellow LED mode
                if (led_state == 4 && button_4_pressed == true) { 
                        gpio_pin_set_dt(&red, 0); 
                        gpio_pin_set_dt(&green, 0);
                        red_led_on = false;
                        green_led_on = false;
                        button_4_pressed = false;
                        while (blink_yellow_led == true) {
                                gpio_pin_set_dt(&red, 1);
                                gpio_pin_set_dt(&green, 1);
                                yellow_led_on = true;
                                printk("Yellow on\n");
                                k_sleep(K_SECONDS(1));
                                gpio_pin_set_dt(&red, 0);
                                gpio_pin_set_dt(&green, 0);
                                yellow_led_on = false;
                                printk("Yellow off\n");
                                k_sleep(K_SECONDS(1));
                        }
                }   
                k_yield();
        }
}

void green_led_task(void *, void *, void *) {
        printk("Green LED thread started\n");
        while (true) {
                if (led_state == 3) {
                        gpio_pin_set_dt(&green, 1);
                        green_led_on = true;
                        printk("Green on\n");
                        k_sleep(K_SECONDS(1));
                        gpio_pin_set_dt(&green, 0);
                        green_led_on = false;
                        printk("Green off\n");
                        direction = 1;
                        if (led_state == 4) {
                                gpio_pin_set_dt(&green, 1);
                                green_led_on = true;
                                printk("Green on, pausing\n");
                                k_msleep(100);
                        }
                        else if (led_state != 4) {
                                led_state = 2;
                        }
                }

                // Turn green LED on or off
                if (led_state == 4 && button_3_pressed == true) {
                        gpio_pin_set_dt(&red, 0); 
                        gpio_pin_set_dt(&green, 0);
                        red_led_on = false;
                        yellow_led_on = false;
                        if (green_led_on == false) {
                                gpio_pin_set_dt(&green, 1);
                                green_led_on = true;
                                printk("Green on\n");
                        }
                        else if (green_led_on == true) {
                                gpio_pin_set_dt(&green, 0);
                                green_led_on = false;
                                printk("Green off\n");
                        }  
                        button_3_pressed = false;                             
                }
                k_yield();
        }
}
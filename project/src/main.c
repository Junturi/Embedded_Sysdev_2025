#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

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
int initialize_leds(void);
int led_state = 0; // State machine for the LED sequence
// 1 -> red
// 2 -> yellow
// 3 -> green
int last_led_state = 0; // Save the last state

int main(void)
{
        // In main, we only initialize the LEDs
        initialize_leds();

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
                        printk("Red on\n");
                        k_sleep(K_SECONDS(1)); // Sleep for 1 second
                        gpio_pin_set_dt(&red, 0); // Set LED off
                        printk("Red off\n");
                        last_led_state = led_state;
                        led_state = 2;
                }
                k_yield(); // Yield and move to the end of the task line
        }
}

void yellow_led_task(void *, void *, void *) {
        printk("Yellow LED thread started\n");
        while (true) {
                if (led_state == 2) {
                        gpio_pin_set_dt(&red, 1); // Set LED on
                        gpio_pin_set_dt(&green, 1);
                        printk("Yellow on\n");
                        k_sleep(K_SECONDS(1)); // Sleep for 1 second
                        gpio_pin_set_dt(&red, 0); // Set LED off
                        gpio_pin_set_dt(&green, 0);
                        printk("Yellow off\n");

                        // Determine if we move to red or green light next
                        if (last_led_state == 1) {
                                led_state = 3;
                        }
                        else if (last_led_state == 3) {
                                led_state = 1;
                        }
                        
                }
                k_yield(); // Yield and move to the end of the task line
        }
}

void green_led_task(void *, void *, void *) {
        printk("Green LED thread started\n");
        while (true) {
                if (led_state == 3) {
                        gpio_pin_set_dt(&green, 1); // Set LED on
                        printk("Green on\n");
                        k_sleep(K_SECONDS(1)); // Sleep for 1 second
                        gpio_pin_set_dt(&green, 0); // Set LED off
                        printk("Green off\n");
                        last_led_state = led_state;
                        led_state = 2;
                }
                k_yield(); // Yield and move to the end of the task line
        }
}
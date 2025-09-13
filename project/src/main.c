#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include <zephyr/drivers/uart.h>

// Initialize thread definitions
#define STACKSIZE 500
#define PRIORITY 5

// Configure UART
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

// Initialize UART thread
void uart_task(void *, void *, void *);
K_THREAD_DEFINE(uart_thread,STACKSIZE, uart_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// Create FIFO buffer
K_FIFO_DEFINE(data_fifo);

// Create struct for FIFO data
struct data_t {
        void *fifo_reserved;
        char msg[20];
};

// Initialize dispatcher thread
void dispatcher_task(void *, void *, void *);
K_THREAD_DEFINE(dispatcher_thread,STACKSIZE, dispatcher_task, NULL, NULL, NULL, PRIORITY, 0, 0);

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

// Initialize red LED thread
void red_led_task(void *, void *, void *);
K_THREAD_DEFINE(red_thread,STACKSIZE, red_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// Initialize green LED thread
void green_led_task(void *, void *, void *);
K_THREAD_DEFINE(green_thread,STACKSIZE, green_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// Initialize yellow LED thread
void yellow_led_task(void *, void *, void *);
K_THREAD_DEFINE(yellow_thread,STACKSIZE, yellow_led_task, NULL, NULL, NULL, PRIORITY, 0, 0);

void sequence_task(void *, void *, void *);
K_THREAD_DEFINE(sequence_thread,STACKSIZE, sequence_task, NULL, NULL, NULL, PRIORITY, 0, 0);

void blink_task(void *, void *, void *);
K_THREAD_DEFINE(blink_thread,STACKSIZE, blink_task, NULL, NULL, NULL, PRIORITY, 0, 0);

// Declare functions and global variables
int initialize_button(void);
int initialize_leds(void);
int initialize_uart(void);

// Condition Variables
K_MUTEX_DEFINE(red_mutex);
K_CONDVAR_DEFINE(red_signal);
K_MUTEX_DEFINE(green_mutex);
K_CONDVAR_DEFINE(green_signal);
K_MUTEX_DEFINE(yellow_mutex);
K_CONDVAR_DEFINE(yellow_signal);
K_MUTEX_DEFINE(sequence_mutex);
K_CONDVAR_DEFINE(sequence_signal);
K_MUTEX_DEFINE(blink_mutex);
K_CONDVAR_DEFINE(blink_signal);
K_MUTEX_DEFINE(dispatch_mutex);
K_CONDVAR_DEFINE(dispatch_signal);



// Button interrupt handlers
void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 0 pressed\n");
        k_condvar_broadcast(&sequence_signal);
}

void button_1_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 1 pressed\n");
        k_condvar_broadcast(&red_signal);
}

void button_2_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 2 pressed\n");
        k_condvar_broadcast(&yellow_signal);
}

void button_3_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 3 pressed\n");
        k_condvar_broadcast(&green_signal);
}

void button_4_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        printk("Button 4 pressed\n");
        k_condvar_broadcast(&blink_signal);                  
}

int main(void)
{
        // In main, initialize LEDs, buttons, uart
        int ret = initialize_uart();
	if (ret != 0) {
		printk("UART initialization failed!\n");
		return ret;
	}
        
        initialize_leds();

        ret = initialize_button();
	if (ret < 0) {
		return 0;
	}

	while (1) {
		k_msleep(10); // sleep 10ms
	}

        k_msleep(100);

        return 0;
}

int initialize_uart(void) {
        // Initialize UART
        if (!device_is_ready(uart_dev)) {
                return 1;
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

        printk("LED initialized ok\n");

        return 0;
}

void dispatcher_task(void *, void *, void *) {
        while (true) {
                // Receive dispatcher data from uart_task FIFO
                struct data_t * rec_item = k_fifo_get(&data_fifo, K_FOREVER);
                char sequence[20];
                strncpy(sequence, rec_item->msg, sizeof(sequence) - 1);
                sequence[sizeof(sequence) - 1] = '\0';
                printk("Dispatcher: %s\n", sequence);

                for (int i = 0; i < strlen(sequence); i++) { // Iterate one character at a time
                        char ch = toupper((unsigned char)sequence[i]);   // Uppercase the character
                        switch (ch) {
                                case 'R':
                                        k_condvar_broadcast(&red_signal); // Call red task
                                        break;
                                case 'Y':
                                        k_condvar_broadcast(&yellow_signal); // Call yellow task
                                        break;
                                case 'G':
                                        k_condvar_broadcast(&green_signal); // Call green task
                                        break;
                                default:
                                        printk("Unknown character\n");
                                        break;
                        }
                        k_condvar_wait(&dispatch_signal, &dispatch_mutex, K_FOREVER); // Wait for release signal
                }
                k_free(rec_item);
        }

}

void uart_task(void *, void *, void *) {
        char rc=0;
        char uart_msg[20];
        memset(uart_msg, 0, 20);
        int uart_msg_count = 0;

	while (true) {
		// Ask UART if data available
		if (uart_poll_in(uart_dev,&rc) == 0) {
                        // If character is not newline, add to UART message buffer
                        if (rc != '\r') {
                                uart_msg[uart_msg_count] = rc;
                                uart_msg_count++;
                        }
                        // If character is newline, copy dispatcher data and put to FIFO buffer
                        else {
                                printk("UART msg: %s\n", uart_msg);

                                struct data_t *buf = k_malloc(sizeof(struct data_t));
                                if (buf == NULL) {
                                        return;
                                }

                                // Copy UART message to dispatcher data
                                snprintf(buf->msg, 20, "%s", uart_msg);

                                k_fifo_put(&data_fifo, buf);
                                printk("Data added to FIFO: %s\n", buf->msg);

                                // Clear UART message buffer
                                uart_msg_count = 0;
                                memset(uart_msg, 0, 20);
                        }
		}
		k_msleep(10);
	}
}

// Task for handling red LED
void red_led_task(void *, void *, void *) {
        printk("Red LED thread started\n");
        while (true) {
                // Wait until red signal is sent from dispatcher
                k_condvar_wait(&red_signal, &red_mutex, K_FOREVER);

                gpio_pin_set_dt(&red, 1); // Set LED on
                printk("Red on\n");
                k_sleep(K_SECONDS(1));
                printk("Red off\n");
                gpio_pin_set_dt(&red, 0); // Set LED off

                // Send signal to dispatcher to continue
                k_condvar_broadcast(&dispatch_signal);
        }
}

void yellow_led_task(void *, void *, void *) {
        printk("Yellow LED thread started\n");
        while (true) {
                k_condvar_wait(&yellow_signal, &yellow_mutex, K_FOREVER);
                
                gpio_pin_set_dt(&red, 1);
                gpio_pin_set_dt(&green, 1);
                printk("Yellow on\n");
                k_sleep(K_SECONDS(1));
                gpio_pin_set_dt(&red, 0);
                gpio_pin_set_dt(&green, 0);
                printk("Yellow off\n");

                k_condvar_broadcast(&dispatch_signal);
        }
}

void green_led_task(void *, void *, void *) {
        printk("Green LED thread started\n");
        while (true) {
                k_condvar_wait(&green_signal, &green_mutex, K_FOREVER);

                gpio_pin_set_dt(&green, 1);
                printk("Green on\n");
                k_sleep(K_SECONDS(1));
                gpio_pin_set_dt(&green, 0);
                printk("Green off\n");

                k_condvar_broadcast(&dispatch_signal);
        }
}

void sequence_task(void *, void *, void *) {
        printk("Sequence thread started\n");
        while (true) {
                k_condvar_wait(&sequence_signal, &sequence_mutex, K_FOREVER);
                printk("Go through light sequence 5 times.\n");

                for (int i = 0; i < 5; i++) {
                        gpio_pin_set_dt(&red, 1);
                        k_sleep(K_SECONDS(1));
                        gpio_pin_set_dt(&red, 0);

                        gpio_pin_set_dt(&red, 1);
                        gpio_pin_set_dt(&green, 1);
                        k_sleep(K_SECONDS(1));
                        gpio_pin_set_dt(&red, 0);
                        gpio_pin_set_dt(&green, 0);
                        
                        gpio_pin_set_dt(&green, 1);
                        k_sleep(K_SECONDS(1));
                        gpio_pin_set_dt(&green, 0);
                }

                k_condvar_broadcast(&dispatch_signal);
        }
}

void blink_task(void *, void *, void *) {
        printk("Blink thread started\n");
        while (true) {
                k_condvar_wait(&blink_signal, &blink_mutex, K_FOREVER);
                printk("Blink yellow 5 times.\n");

                for (int i = 0; i < 5; i++) {
                        gpio_pin_set_dt(&red, 1);
                        gpio_pin_set_dt(&green, 1);
                        k_sleep(K_SECONDS(1));
                        gpio_pin_set_dt(&red, 0);
                        gpio_pin_set_dt(&green, 0);
                        k_sleep(K_SECONDS(1));
                }

                k_condvar_broadcast(&dispatch_signal);
        }
}
#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#include <stdbool.h>

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


__attribute__((__interrupt__)) static void interrupt_J3(void);
void init(void);
void respond_to_test(uint32_t response_pin);

/* PIN flags for task D */
volatile bool TEST_A_flag;
volatile bool TEST_B_flag;
volatile bool TEST_C_flag;

void init(void){
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);
    
    cpu_irq_disable();
    INTC_init_interrupts();
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();
    
    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}

void respond_to_test(uint32_t response_pin) {
	gpio_set_pin_low(response_pin);
	busy_delay_us(5);
	gpio_set_pin_high(response_pin);
}

__attribute__((__interrupt__)) static void interrupt_J3(void){
	
	/* Task C Interrupt */
	//if (gpio_get_pin_interrupt_flag(TEST_A)){
		//gpio_clear_pin_interrupt_flag(TEST_A);
		//respond_to_test(RESPONSE_A);
	//}
	//
	//if (gpio_get_pin_interrupt_flag(TEST_B)){
		//gpio_clear_pin_interrupt_flag(TEST_B);
		//respond_to_test(RESPONSE_B);
	//}
	//
	//if (gpio_get_pin_interrupt_flag(TEST_C)){
		//gpio_clear_pin_interrupt_flag(TEST_C);
		//respond_to_test(RESPONSE_C);
	//}
	
	/* Task D interrupt */
	if (gpio_get_pin_interrupt_flag(TEST_A)){
		gpio_clear_pin_interrupt_flag(TEST_A);
		TEST_A_flag = true;
	}
	
	if (gpio_get_pin_interrupt_flag(TEST_B)){
		gpio_clear_pin_interrupt_flag(TEST_B);
		TEST_B_flag = true;
	}
	
	if (gpio_get_pin_interrupt_flag(TEST_C)){
		gpio_clear_pin_interrupt_flag(TEST_C);
		TEST_C_flag = true;
	}
}

int main (void){
    init();
	
	/* Input pins */
	gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
	gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
	gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
	
	/* Output pins */
	gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	/* Interruts */
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
	
   
    while(1){
		
		/* Big while */
		if (TEST_A_flag) {
			respond_to_test(RESPONSE_A);
			TEST_A_flag = false;
		}
		
		if (TEST_B_flag) {
			gpio_set_pin_low(RESPONSE_B);
			busy_delay_us(100);
			gpio_set_pin_high(RESPONSE_B);
			TEST_B_flag = false;
		}
		
		if (TEST_C_flag) {
			respond_to_test(RESPONSE_C);
			TEST_C_flag = false;
		}
		
		/* Busy wait */
		//if (gpio_pin_is_low(TEST_A)) {
			//respond_to_test(RESPONSE_A);
		//}
		//
		//if (gpio_pin_is_low(TEST_B)) {
			//respond_to_test(RESPONSE_B);
		//}
		//
		//if (gpio_pin_is_low(TEST_C)) {
			//respond_to_test(RESPONSE_C);
		//}
		
		/* Toggle led */
        //gpio_toggle_pin(LED0_GPIO);
		//
        //printf("tick\n");
        //
        //busy_delay_ms(500);
    }
}

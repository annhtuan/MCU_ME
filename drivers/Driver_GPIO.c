#include "Driver_GPIO.h"
#include "Driver_Common.h"

// Pin mapping
#define GPIO_MAX_PINS           160U
#define PIN_IS_AVAILABLE(n)     ((n) < GPIO_MAX_PINS)

static ARM_GPIO_SignalEvent_t gpio_event_cb = NULL;


// Setup GPIO Interface
static int32_t GPIO_Setup (ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t event_cb) {
  int32_t result = ARM_DRIVER_OK;
  PORT_Type *base;
  IRQn_Type IRQn;
  if (PIN_IS_AVAILABLE(pin)) {
	    uint32_t port = pin / 32;
	    uint32_t pin_num = pin % 32;
		/*Get port and Clock for port*/
	    switch(port){
	    case 0:
	    	base = PORTA;
	    	IRQn = PORTA_IRQn;
	    	//Clock
	    	PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC(1);
	    	break;
	    case 1:;
	    	base = PORTB;
	    	IRQn = PORTB_IRQn;
	    	//Clock
	    	PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC(1);
	    	break;
	    case 2:
	    	base = PORTC;
	    	IRQn = PORTC_IRQn;
	    	//Clock
	    	PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC(1);
	    	break;
	    case 3:
	    	base = PORTD;
	    	IRQn = PORTD_IRQn;
	    	//Clock
	    	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC(1);
	    	break;
	    }
    	//MUX GPIO
    	base->PCR[pin_num] &= ~PORT_PCR_MUX_MASK;
    	base->PCR[pin_num] |= 	PORT_PCR_MUX(1);

    	//Callback function
    	gpio_event_cb = event_cb;
  } else {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Direction
static int32_t GPIO_SetDirection (ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin)) {
	    uint32_t port = pin / 32;
	    uint32_t pin_num = pin % 32;
	    GPIO_Type * base;

		/*Get port*/
	    switch(port){
	    case 0:
	    	base = PTA;
	    	break;
	    case 1:
	    	base = PTB;
	    	break;
	    case 2:
	    	base = PTC;
	    	break;
	    case 3:
	    	base = PTD;
	    	break;
	    default:
	    	break;
	    }
    switch (direction) {
      case ARM_GPIO_INPUT:
    	  base->PDDR &= ~(1U << pin_num);
    	  break;
      case ARM_GPIO_OUTPUT:
    	  base->PDDR |= (1U << pin_num);
    	  break;
      default:
			result = ARM_DRIVER_ERROR_PARAMETER;
			break;
    }
  } else {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Output Mode
static int32_t GPIO_SetOutputMode (ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin)) {

    switch (mode) {
      case ARM_GPIO_PUSH_PULL:
        break;
      case ARM_GPIO_OPEN_DRAIN:
        break;
      default:
        result = ARM_DRIVER_ERROR_PARAMETER;
        break;
    }
  } else {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Pull Resistor
static int32_t GPIO_SetPullResistor (ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin)) {
	uint32_t port = pin / 32;
	uint32_t pin_num = pin % 32;
	PORT_Type * base;

	/*Get port*/
	switch(port){
		case 0:
			base = PORTA;
			break;
		case 1:
			base = PORTB;
			break;
		case 2:
			base = PORTC;
			break;
		case 3:
			base = PORTD;
			break;
		default:
			break;
		}
    switch (resistor) {
      case ARM_GPIO_PULL_NONE:
      	base->PCR[pin_num] &= 	~PORT_PCR_PE_MASK; 				// Enable pull resistor
        break;
      case ARM_GPIO_PULL_UP:
		base->PCR[pin_num] |= 	PORT_PCR_PE_MASK 				// Enable pull resistor
								|PORT_PCR_PS_MASK;              // Pull-up resistor
        break;
      case ARM_GPIO_PULL_DOWN:
  		base->PCR[pin_num] |= 	PORT_PCR_PE_MASK 				// Enable pull resistor
  								|~(PORT_PCR_PS_MASK);              // Pull-up resistor
        break;
      default:
        result = ARM_DRIVER_ERROR_PARAMETER;
        break;
    }
  } else {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Event Trigger
static int32_t GPIO_SetEventTrigger (ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin)) {
	uint32_t port = pin / 32;
	uint32_t pin_num = pin % 32;
	PORT_Type * base;

	/*Get port*/
	switch(port){
		case 0:
			base = PORTA;
			break;
		case 1:
			base = PORTB;
			break;
		case 2:
			base = PORTC;
			break;
		case 3:
			base = PORTD;
			break;
		default:
			break;
		}
    switch (trigger) {
      case ARM_GPIO_TRIGGER_NONE:
    	  base->PCR[pin_num] &= ~PORT_PCR_IRQC_MASK;
    	  break;
      case ARM_GPIO_TRIGGER_RISING_EDGE:
    	  base->PCR[pin_num] |= PORT_PCR_IRQC(1u << 0);
    	  break;
      case ARM_GPIO_TRIGGER_FALLING_EDGE:
    	  base->PCR[pin_num] |= PORT_PCR_IRQC(0x0A);
        break;
      case ARM_GPIO_TRIGGER_EITHER_EDGE:
    	  base->PCR[pin_num] |= PORT_PCR_IRQC(0x0B);
        break;
      default:
        result = ARM_DRIVER_ERROR_PARAMETER;
        break;
    }
  } else {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Output Level
static void GPIO_SetOutput (ARM_GPIO_Pin_t pin, uint32_t val) {

  if (PIN_IS_AVAILABLE(pin)) {
	uint32_t port = pin / 32;
	uint32_t pin_num = pin % 32;
	GPIO_Type * base;
	/*Get port*/
	switch(port){
	case 0:
		base = PTA;
		break;
	case 1:
		base = PTB;
		break;
	case 2:
		base = PTC;
		break;
	case 3:
		base = PTD;
		break;
	}

	/*Set output value for pin*/
	if (val == 0U)
	{
		base->PCOR = 1 << pin_num;
	}
	else
	{
		base->PSOR = 1 << pin_num;
	}
  }
}
static void GPIO_Toggle (ARM_GPIO_Pin_t pin) {

  if (PIN_IS_AVAILABLE(pin)) {
	uint32_t port = pin / 32;
	uint32_t pin_num = pin % 32;
	GPIO_Type * base;

	/*Get port*/
	switch(port){
	case 0:
		base = PTA;
		break;
	case 1:
		base = PTB;
		break;
	case 2:
		base = PTC;
		break;
	case 3:
		base = PTD;
		break;
	}

	/*Toggle pin*/
	base->PTOR |= (1u << pin_num);
  }
}
// Get GPIO Input Level
static uint32_t GPIO_GetInput (ARM_GPIO_Pin_t pin) {
  uint32_t val = 0U;

  if (PIN_IS_AVAILABLE(pin)) {
	    uint32_t port = pin / 32;
	    uint32_t pin_num = pin % 32;
	    GPIO_Type * base;

		/*Get port*/
	    switch(port){
	    case 0:
	    	base = PTA;
	    	break;
	    case 1:
	    	base = PTB;
	    	break;
	    case 2:
	    	base = PTC;
	    	break;
	    case 3:
	    	base = PTD;
	    	break;
	    }

		val = ((base->PDIR & (uint32_t) (1 << pin_num)) >> pin_num);
  }
  return val;
}

void Get_GPIO_IRQn(ARM_GPIO_Pin_t pin){
	  if (PIN_IS_AVAILABLE(pin)) {
		uint32_t port = pin / 32;
		uint32_t pin_num = pin % 32;
		PORT_Type * base;

		/*Get port*/
		switch(port){
			case 0:
				base = PORTA;
				break;
			case 1:
				base = PORTB;
				break;
			case 2:
				base = PORTC;
				break;
			case 3:
				base = PORTD;
				break;
			default:
				break;
			}
		if ( ((base->PCR[pin_num] & PORT_PCR_ISF_MASK) >> PORT_PCR_ISF_SHIFT) != 0)
		{
			// Clear Flag
			base->PCR[pin_num] |= PORT_PCR_ISF_MASK;
			// Callback Function to call application code
			(*gpio_event_cb)(pin);
		}
	  }
}

// GPIO Driver access structure
ARM_DRIVER_GPIO Driver_GPIO = {
  GPIO_Setup,
  GPIO_SetDirection,
  GPIO_SetOutputMode,
  GPIO_SetPullResistor,
  GPIO_SetEventTrigger,
  GPIO_SetOutput,
  GPIO_GetInput,
  GPIO_Toggle
};

#ifndef DRIVER_GPIO_H_
#define DRIVER_GPIO_H_
#include <stdint.h>
#include "S32K144.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

/****** GPIO specific error codes *****/
#define ARM_GPIO_ERROR_PIN              (ARM_DRIVER_ERROR_SPECIFIC - 1) ///< Specified Pin not available
/**
\brief GPIO Pin
*/
typedef uint32_t ARM_GPIO_Pin_t;

/**
\brief GPIO Direction
*/
typedef enum {
  ARM_GPIO_INPUT,                       ///< Input (default)
  ARM_GPIO_OUTPUT                       ///< Output
} ARM_GPIO_DIRECTION;

/**
\brief GPIO Output Mode
*/
typedef enum {
  ARM_GPIO_PUSH_PULL,                   ///< Push-pull (default)
  ARM_GPIO_OPEN_DRAIN                   ///< Open-drain
} ARM_GPIO_OUTPUT_MODE;

/**
\brief GPIO Pull Resistor
*/
typedef enum {
  ARM_GPIO_PULL_NONE,                   ///< None (default)
  ARM_GPIO_PULL_UP,                     ///< Pull-up
  ARM_GPIO_PULL_DOWN                    ///< Pull-down
} ARM_GPIO_PULL_RESISTOR;

/**
\brief GPIO Event Trigger
*/
typedef enum {
  ARM_GPIO_TRIGGER_NONE,                ///< None (default)
  ARM_GPIO_TRIGGER_RISING_EDGE,         ///< Rising-edge
  ARM_GPIO_TRIGGER_FALLING_EDGE,        ///< Falling-edge
  ARM_GPIO_TRIGGER_EITHER_EDGE          ///< Either edge (rising and falling)
} ARM_GPIO_EVENT_TRIGGER;


///****** GPIO Event *****/
//#define ARM_GPIO_EVENT_RISING_EDGE      (1UL << 0)  ///< Rising-edge detected
//#define ARM_GPIO_EVENT_FALLING_EDGE     (1UL << 1)  ///< Falling-edge detected
//#define ARM_GPIO_EVENT_EITHER_EDGE      (1UL << 2)  ///< Either edge detected (only when hardware cannot distinguish between rising and falling edge)

typedef void (*ARM_GPIO_SignalEvent_t) (ARM_GPIO_Pin_t pin);  /* Pointer to \ref ARM_GPIO_SignalEvent : Signal GPIO Event */
void Get_GPIO_IRQn(ARM_GPIO_Pin_t pin);

/**
\brief Access structure of the GPIO Driver.
*/
typedef struct {
  int32_t  (*Setup)           (ARM_GPIO_Pin_t pin,ARM_GPIO_SignalEvent_t event_cb); ///< Pointer to \ref ARM_GPIO_Setup : Setup GPIO Interface.
  int32_t  (*SetDirection)    (ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction);    ///< Pointer to \ref ARM_GPIO_SetDirection : Set GPIO Direction.
  int32_t  (*SetOutputMode)   (ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode);       ///< Pointer to \ref ARM_GPIO_SetOutputMode : Set GPIO Output Mode.
  int32_t  (*SetPullResistor) (ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor); ///< Pointer to \ref ARM_GPIO_SetPullResistor : Set GPIO Pull Resistor.
  int32_t  (*SetEventTrigger) (ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger);  ///< Pointer to \ref ARM_GPIO_SetEventTrigger : Set GPIO Event Trigger.
  void     (*SetOutput)       (ARM_GPIO_Pin_t pin, uint32_t val);                    ///< Pointer to \ref ARM_GPIO_SetOutput : Set GPIO Output Level.
  uint32_t (*GetInput)        (ARM_GPIO_Pin_t pin);                                  ///< Pointer to \ref ARM_GPIO_GetInput : Get GPIO Input Level.
  void     (*Toggle)       	  (ARM_GPIO_Pin_t pin);
} const ARM_DRIVER_GPIO;



#endif /* DRIVER_GPIO_H_ */

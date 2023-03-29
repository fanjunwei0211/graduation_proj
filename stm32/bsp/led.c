#include "led.h"

void Led_Flash(int count)
{
	static int i=0;
	i++;
	if(i>=count)
	{
		i=0;
		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	}
}


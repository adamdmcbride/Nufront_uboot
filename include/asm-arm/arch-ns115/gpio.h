#ifndef GPIO_H_
#define GPIO_H_

extern unsigned int gpio_pinmux_config(unsigned pin_nr);
extern int gpio_direction_input(unsigned ping_nr);
extern int gpio_direction_output(unsigned ping_nr);
extern int nufront_get_gpio_value(unsigned pin_nr);
extern int nufront_set_gpio_value(unsigned pin_nr,int value);
extern int wkgpio_direction_input(unsigned ping_nr);
extern int wkgpio_direction_output(unsigned ping_nr);
extern int get_wkgpio_value(unsigned pin_nr);
extern int set_wkgpio_value(unsigned pin_nr,int value);
extern int wkgpio_enable(int enable);

#endif

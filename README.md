AVR Utility libraries
=====================

This library contains a set of header only utility classes for native C++ AVR development.

At the core is the file [pin_definitions.hpp](https://github.com/DannyHavenith/avr_utilities/blob/master/avr_utilities/pin_definitions.hpp) which allows Arduino-like pin manipulations with native (2 clock ticks) performance. This library is described in some detail on the [dedicated project page](http://rurandom.org/justintime/index.php?title=Arduino-like_pin_definitions_in_C%2B%2B). An example of the syntax:


```C++
PIN_TYPE( B, 5) led_pin; // LED is attached to pin 5 of port B 

void f()
{
    make_output( led_pin);
    set( led_pin); // LED on
    _delay_ms( 500);
    clear( led_pin); // LED off
}
```

On top of this are a few other utilities and hardware drivers, for example:

Software SPI
------------

Sometimes it's convenient to have an SPI driver on configurable pins. The `bit_banged_spi` template takes care of that. Example use

```C++
#include "avr_utilities/pin_definitions.hpp"
#include "avr_utilities/devices/bitbanged_spi.h"

PIN_TYPE( B, 4) chip_select;
struct spi_pins
{
    PIN_TYPE( D, 4) miso;
    PIN_TYPE( C, 4) mosi;
    PIN_TYPE( D, 3) clk;
};
typedef bitbanged_spi< spi_pins> spi;
```

From here on, you have two choices. You can either only use the static functions of the spi type. In this case you are required to call the static `init()` function before doing anything else:

```C++
void f()
{
    spi::init();
    make_output( chip_select);
    set( chip_select);
    
    // send a message
    clear( chip_select);
    spi::transmit( "hello, world!");
    set( chip_select);
    
    // send a byte and receive one at the same time
    clear( chip_select);
    uint8_t reply = spi::transmit_receive( (uint8_t)42);
    set( chip_select);
    
    // etc..
}
```

Or, at the cost of a few bytes of program space, you can declare an instance of the spi type and use it as a local object, in which case the initialization is done automatically:

```C++
spi my_spi;

void f()
{
    clear( chip_select);
    my_spi.transmit("hi there!");
    set( chip_select);
}
```

HD44780 LCD display
-------------------

This driver can be used to control an attached hd44780 LCD display in 4-bit mode. Usage is as follows:

```C++
#include "avr_utilities/pin_definitions.hpp"
#include "avr_utilities/devices/hd44780.hpp"

struct hd44780_pins
{
    PIN_TYPE( C, 5)         rs;
    PIN_TYPE(D, 2)          rw;
    PIN_TYPE(D, 3)          e;
    PIN_GROUP_TYPE( D, 4, 4) data; // 4 bits starting at D4 (D4, D5, D6, D7) are the data lines.
};
typedef hd44780::lcd<hd44780_pins> lcd;

```

And again, you can choose between using static functions and calling init yourself, or instantiating an object and enjoy auto-initialization. The following example shows only the instantiated version (note: controlling this device may take some more low-level calls):

```C++
lcd my_lcd;

void example()
{
    my_lcd.cls();
    
    // Go to the second line on my lcd display, which starts at address 64.
    // This address may be different on other types of display.
    my_lcd.command_out( hd44780::commands::dd_addr( 64));
    my_lcd.string_out( "Hi Planet!");
}
```

About the mini-boost distribution
---------------------------------
Originally, this library re-implemented small parts of mpl and preprocessor. Now, this
library contains, in sub-directory 'boost' a subset of boost libraries. See the boost/README.md for more information.



/* MM5740 equivalent clone, copyright (C) 2022 Tube Time.

   This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
   International License. To view a copy of this license, visit
   http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to
   Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
*/

#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>


/* Port assignments:
    PORTA = External data port (outputs)
    PORTB
        0 - X9
        1 - X8
        2 - X7
        3 - X6
        4 - X5
        5 - X4
        6 - X3
        7 - X2
    PORTC
        0 - Y2
        1 - Y3
        2 - Y4
        3 - Y5
        4 - Y6
        5 - Y7
        6 - Y8
        7 - Y9
    PORTD
        0 - X1
        1 - SRB (strobe output)
        2 - Repeat clock in
        3 - n/c
        4 - Control key
        5 - Shift key
        6 - Y10
        7 - Y1
*/

#define KEY_COUNT           90  // Total number of possible keys
#define DEBOUNCE_DELAY     200  // 2ms debounce delay
#define STROBE_TIME         50  // 50us strobe pulse
#define ROW_SETTLING_TIME   50  // 50us settling time

bool keyflags[KEY_COUNT] = {0};
bool repeat_history = false;

uint8_t key_rom[] = {
// Row
// 1     2     3     4     5     6     7     8     9   10
// Unshifted
 '8',  '4',  '5',  '1',  '2',  '3',  '0',  '6',  '9', '7', // Col 1
0x0c, 0x0d, 0x1c, 0x1d, 0x0b, 0x0e, 0x20, 0x09, 0x08, '-', // Col 2
 '0', 0x0a,  'P', 0x7f,  ';',  '.',  '/',  'P',  'O', ':', // Col 3
 '9',  'I',  'O',  'K',  'L',  ',',  '.',  'L',  'K', '8', // Col 4
 '6',  'U',  'Y',  'J',  'H',  'M',  'N',  'M',  'N', '7', // Col 5
 '5',  'R',  'T',  'F',  'G',  'V',  'B', 0x18, 0x19, '4', // Col 6
0x12,  'E', 0x13,  'D', 0x14,  'C', 0x15, 0x16, 0x17, '3', // Col 7
0x05,  'W', 0x06,  'S', 0x07,  'X', 0x0f, 0x10, 0x11, '2', // Col 8
0x00,  'Q', 0x1b,  'A', 0x01,  'Z', 0x02, 0x03, 0x04, '1', // Col 9

// Shifted
 '8',  '4',  '5',  '1',  '2',  '3',  '0',  '6',  '9', '7', // Col 1
0x0c, 0x0d, 0x1c, 0x1d, 0x0b, 0x0e, 0x20, 0x09, 0x08, '=', // Col 2
 '0', 0x0a,  '@', 0x7f,  '+',  '.',  '?',  'P',  'O', '*', // Col 3
 ')',  'I',  '_',  '[', 0x5c,  '<',  '>',  'L',  'K', '(', // Col 4
 '&',  'U',  'Y',  'J',  'H',  ']',  '^',  'M',  'N',0x27, // Col 5
 '%',  'R',  'T',  'F',  'G',  'V',  'B', 0x18, 0x19, '$', // Col 6
0x12,  'E', 0x13,  'D', 0x14,  'C', 0x15, 0x16, 0x17, '#', // Col 7
0x05,  'W', 0x06,  'S', 0x07,  'X', 0x0f, 0x10, 0x11,0x22, // Col 8
0x00,  'Q', 0x1b,  'A', 0x01,  'Z', 0x02, 0x03, 0x04, '!', // Col 9

// Control
 '8',  '4',  '5',  '1',  '2',  '3',  '0',  '6',  '9', '7', // Col 1
0x0c, 0x0d, 0x1c, 0x1d, 0x0b, 0x0e, 0x20, 0x09, 0x08, '-', // Col 2
 '0', 0x0a,  'P', 0x7f,  ';',  '.',  '/', 0x10, 0x0f, ':', // Col 3
 '9', 0x09, 0x0f, 0x0b, 0x0c,  ',',  '.', 0x0c, 0x0b, '8', // Col 4
 '6', 0x15, 0x19, 0x0a, 0x08, 0x0d, 0x0e, 0x0d, 0x0e, '7', // Col 5
 '5', 0x12, 0x14, 0x06, 0x07, 0x16, 0x02, 0x18, 0x19, '4', // Col 6
0x12, 0x05, 0x13, 0x04, 0x14, 0x03, 0x15, 0x16, 0x17, '3', // Col 7
0x05, 0x17, 0x06, 0x13, 0x07, 0x18, 0x0F, 0x10, 0x11, '2', // Col 8
0x00, 0x11, 0x1b, 0x01, 0x01, 0x1a, 0x02, 0x03, 0x04, '1', // Col 9

// Shift + Control
 '8',  '4',  '5',  '1',  '2',  '3',  '0',  '6',  '9', '7', // Col 1
0x0c, 0x0d, 0x1c, 0x1d, 0x0b, 0x0e, 0x20, 0x09, 0x08, '=', // Col 2
 '0', 0x0a, 0x00, 0x7f,  '+',  '.',  '?', 0x10, 0x0f, '*', // Col 3
 ')', 0x09, 0x1f, 0x1b, 0x1c,  '<',  '>', 0x0c, 0x0b, '(', // Col 4
 '&', 0x15, 0x19, 0x0a, 0x08, 0x1d, 0x1e, 0x0d, 0x0e,0x27, // Col 5
 '%', 0x12, 0x14, 0x06, 0x07, 0x16, 0x02, 0x18, 0x19, '$', // Col 6
0x12, 0x05, 0x13, 0x04, 0x14, 0x03, 0x15, 0x16, 0x17, '#', // Col 7
0x05, 0x17, 0x06, 0x13, 0x07, 0x18, 0x0F, 0x10, 0x11,0x22, // Col 8
0x00, 0x11, 0x1b, 0x01, 0x01, 0x1a, 0x02, 0x03, 0x04, '!', // Col 9


};

// Output Bit changes for ROM:
// 1, 2, 3, 4,  ,  ,  ,
//             5, 6, 7, 8    - changes for ctrl and shift


// Disables the drive signal on a X column
void disable_column(uint8_t col)
{
    // Disable the current column
    if (col == 0) {
        DDRD &= ~_BV(0);
    } else {
        DDRB &= ~_BV(8 - col);
    }
}

// Enables the drive signal on a X column
void enable_column(uint8_t col)
{
    // Enable the next column
    if (col == 0) {
        DDRD |= _BV(0);
    } else {
        DDRB |= _BV(8 - col);
    }
}

// Read the contents of the Y rows.
uint16_t read_rows()
{
    return ~((((uint16_t)PIND & 0x40) << 3) |
              ((uint16_t)PINC << 1) |
              ((uint16_t)PIND >> 7));
}

// Pulse the strobe output
void strobe()
{
    PORTD |= _BV(1);
    _delay_us(STROBE_TIME);
    PORTD &= ~_BV(1);
}

// Checks for falling edge on the key repeat signal
bool is_repeat()
{
    bool repeat = (PIND & _BV(2));
    if (!repeat && repeat_history) {
        repeat_history = repeat;
        return true;
    } else {
        repeat_history = repeat;
        return false;
    }
}

// Checks a key in a row to ensure it stays down
bool debounce_key(uint8_t row)
{
    int debounce;
    for (debounce = 0; debounce < DEBOUNCE_DELAY; debounce++) {
        _delay_us(10);
        if (!(read_rows() & _BV(row))) {
            return false;
        }
    }
    return true;
}

// Look up the key in the ROM, with shift and control
uint8_t key_lookup(uint8_t scancode)
{
    uint16_t shift = (PIND & _BV(5)) ? 90 : 0;
    uint16_t control = (PIND & _BV(4)) ? 180: 0;
    return key_rom[(uint16_t)scancode + shift + control];
}

// Scan the keyboard matrix
bool scan_keys()
{
    uint8_t x, y, rom_addr, debounce;
    uint16_t row_data;
    bool keydown = false;

    rom_addr = 0;
    for (x = 0; x < 9; x++) {
        enable_column(x);
        _delay_us(ROW_SETTLING_TIME);
        row_data = read_rows();
        // Sense logic also uses a counter.
        for (y = 0; y < 10; y++) {
            if (row_data & _BV(y)) {
                if (!keyflags[rom_addr] & debounce_key(y)) {
                    PORTA = key_lookup(rom_addr);
                    strobe();
                    keyflags[rom_addr] = true;  // Key is down, update memory so we don't emit it again

                    // Continue scanning for more keys
                }
            } else {
                keyflags[rom_addr] = false;  // Key is not down, update memory
            }
            // Detect if any key is down
            if (keyflags[rom_addr]) keydown = true;
            rom_addr++;
        }
        disable_column(x);
    }
    return keydown;
}

// Note: Emit strobe pulse for one clock period (around 50us).

void main(void)
{
    // Set up ports
    DDRA = 0xff; // Output port
    PORTA = 0x00; // No character

    DDRB = 0x00; // Column drivers (X) are off by default
    PORTB = 0x00; // No pullup resistors for column drivers

    DDRC = 0x00; // Row connections (Y) are inputs
    PORTC = 0xff; // Pullup resistors turned on

    DDRD = _BV(1) | _BV(3); // SRB and n/c are outputs
    PORTD = _BV(2) | _BV(6) | _BV(7); // Pullups for REPEAT, Y10 and Y1

    while(1) {
        if (scan_keys() && is_repeat()) {
            strobe(); // pulse strobe again
        }
    }
}

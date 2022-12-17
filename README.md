# Replica Datanetics Keyboard

Do you need a Datanetics ASCII keyboard for your computer project (such as a replica Apple 1)? Don't want to pay high prices for an original Datanetics keyboard? Then consider building this replica.

![Replica Datanetics keyboard photo](https://github.com/schlae/replica-datanetics/blob/main/images/replica-datanetics.jpg)

This is not the first replica Datanetics keyboard. Mike Willegal [designed one](http://www.willegal.net/appleii/apple1-datanetics.htm). In fact, I started with his design and redid it using KiCad. Instead of using the expensive, fragile, and hard-to-find MM5740 keyboard chip, I switched to a more common microcontroller: the ATMega644P. I also removed the output buffer chips--just in case you want to redo my firmware and implement some other keyboard protocol, you'll be able to control the pin direction.

Here are the design files. The BOM does not include manufacturer part numbers because it uses very common through hole parts that you might already have. The key switches are standard Cherry MX and are available from a wide variety of sources.

[Schematic](https://github.com/schlae/replica-datanetics/blob/main/Apple1Keyb.pdf)

[Bill of Materials](https://github.com/schlae/replica-datanetics/blob/main/Apple1Keyb.csv)

[Fab Files](https://github.com/schlae/replica-datanetics/blob/main/fab/Apple1Keyb_Rev1.zip)

[Water Slide Decal Graphics](https://github.com/schlae/replica-datanetics/blob/main/keycaps.svg)

## Fabrication and Assembly Notes

The circuit board is a 2-layer board, 11.75 by 5.5 inches (298.5 x 139.7mm). No special edge connector plating or bevel is needed. To make the board look like the original, order the board "bare bones" with no solder mask or silkscreen.

**Caution: There is a "matrix" via area in the upper left corner of the board with 76 non-plated vias. The drill locations for these are included in the "Apple1Keyb-NPTH.drl" file in the fab package. The User Comments layer also includes a note to the fab vendor to NOT plate these holes. You may want to add an extra comment to your fab vendor when you put in the order that these 76 holes must not be plated.**

To assemble the board, just solder in the components. I'd recommend using a socket for the U1 microcontroller so that you can reprogram it later. The NE555 timer (U2) in the original design was meant to provide the clock for the MM5740 chip, but isn't necessary in this design. You can install it or leave it out.

The second NE555 timer (U3) is necessary for the key repeat function.

## Key Caps

The keyboard design uses standard Cherry MX key switches and keycaps are available from numerous sources. For my own build, I used DSA keycaps along with an 8U spacebar from PMK. I looked into a number of ways of putting the legends on the keycaps but I ended up having some water slide decals custom made. See the file above if you want to use this same file, but I'd recommend putting the legends closer together so they'll fit on a smaller sheet, saving you money.

## Matrix Via Area

The Datanetics design has four key switches that are not connected to the key scanning matrix and are brought out to individual pins on the edge connector. If you're building an Apple 1, you'll want to use two of these keys for the reset and clear screen functions. 

If you want keys to be included in the scanning matrix, you'll want to solder a fine wire through the appropriate via in the upper left corner of the board. To find out which key will be generated, look up the row and column number in the key\_rom array in firmware source code. You can, of course, customize the generated key by editing the source and recompiling.

## Firmware

The firmware is designed for the ATMega644P microcontroller, but it should be possible to modify it to work with other 40-pin microcontrollers in the ATMega family. The prebuilt firmware behaves like a stock MM5740.

[Prebuilt Firmware](https://github.com/schlae/replica-datanetics/blob/main/fw/apple1keyb.hex)

Be sure to set the fuse bits of the ATMega644P according to the fuses.conf file:

```
lfuse = 0x62
hfuse = 0xdd
efuse = 0xfc
```

You can build the firmware yourself pretty easily. Under Ubuntu, install the `gcc-avr` package, and build the firmware by going to the directory and typing `make`.

If you have a minipro programmer (TL866 etc) you can program the device by typing `make fuses` followed by `make prog`. For this to work correctly, you'll need the [minipro software](https://gitlab.com/DavidGriffith/minipro) installed. You could also use the Windows minipro software from the manufacturer, or another programmer entirely.

## Mechanical Brackets

[Mike Willegal's site](http://www.willegal.net/appleii/apple1-datanetics.htm) has drawings for a mechanical bracket.

## Connecting to an Apple 1

If you want to connect this keyboard to an Apple 1, you can use the following wiring diagram. The connector to use is a 30-pin 0.156" edge connector, something like EDAC 305-030-520-202 or equivalent. The DIP plug is an insulation displacement connector, something like CW CWR-130-16-0001 or equivalent.

| Edge connector pin | Description | DIP plug pin |
|--------------------|-------------|--------------|
| A | No connect (was -12V) | 11 (-12V) |
| B | Data bit 6 | 7 (B6) |
| C | Data bit 5 | 6 (B5) |
| D | Data bit 4 | 2 (B4) |
| E | Data bit 1 | 5 (B1) |
| F | Data bit 2 | 4 (B2) |
| H | Data bit 3 | 3 (B3) |
| J | Reset key | 1 (Reset) |
| K | Reset key | 9 (Gnd) |
| L | No connect (was OE) | No connect |
| M | Strobe | 14 (Str) |
| N | Data bit 7 | 8 (B7) |
| P | Data bit 8 | No connect |
| R | No connect (was data bit 9) | No connect |
| S | Ground | 9 |
| 1 | No connect | |
| 2 | No connect | |
| 3 | No connect | |
| 4 | +5V | 15, 16 |
| 5 | No connect | |
| 6 | No connect | |
| 7 | No connect | |
| 8 | Extra key 4 | 12 (Clr) |
| 9 | Extra key 4 | 16 |
| 10 | Extra key 3 | |
| 11 | Extra key 3 | |
| 12 | Extra key 1 | |
| 13 | Extra key 1 | |
| 14 | Extra key 2 | |
| 15 | Extra key 2 | |

## License
This work is licensed under a Creative Commons Attribution-ShareAlike 4.0
International License. See [https://creativecommons.org/licenses/by-sa/4.0/](https://creativecommons.org/licenses/by-sa/4.0/).

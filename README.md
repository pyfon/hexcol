# Hexcol

Hexcol allows you to display a block of colour with a given hex code.
This program is only compatible with terminals which support true 24 bit colour

## Compiling

1. `$ cc -o hexcol main.c`
2. `$ ./hexcol`

## Usage
```
  hexcol [options] [colours...]

  -r, --rectange <Width>[xHeight]       Define colour rectangle dimensions
  -h, --help                            Print this help message and exit

  [colours...] are a list of one or more 24-bit hex colour codes.
  Codes can be optionally prefixed with '0x' or '#'.
  Eg hexcol -r 40x6 005bbb 0xFFD500
```

![Preview Image](https://github.com/pyfon/hexcol/blob/main/png/md.png)

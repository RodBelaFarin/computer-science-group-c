#ifndef lookUpTable
#define lookUpTable


#define A 10
#define B 11
#define C 12
#define D 13
#define E 14
#define F 15
#define G 16
#define H 17
#define I 18
#define J 19
#define K 20
#define L 21
#define M 22
#define N 23
#define O 24
#define P 25
#define Q 26
#define R 27
#define S 28
#define T 29
#define U 30
#define V 31
#define W 32
#define X 33
#define Y 34
#define Z 35
#define UNDERSCORE 36

#define ZERO	0
#define ONE 	1
#define TWO 	2
#define THREE 	3
#define FOUR 	4
#define FIVE 	5
#define SIX 	6
#define SEVEN 	7
#define EIGHT 	8
#define NINE 	9

/*
#define UP1 	0
#define UP2 	1
#define NUM1 	2
#define NUM2 	3
#define COL1 	4
#define COL2 	5
*/


uint8_t num2segTable[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};


const uint16_t nib[] = 
{
	0x5559,     // '0'
    0x0118,     // '1'
    0x1e11,     // '2
    0x1b11,     // '3
    0x0b50,     // '4
    0x1b41,     // '5
    0x1f41,     // '6
    0x0111,     // '7
    0x1f51,     // '8
    0x1b51,     // '9'
	0x0f51,     // 'A' (+ 'a')
    0x3991,     // 'B' (+ 'b')
    0x1441,     // 'C' (+ 'c')
    0x3191,     // 'D' (+ 'd')
    0x1e41,     // 'E' (+ 'e')
    0x0e41,     // 'F' (+ 'f')
    0x1d41,     // 'G' (+ 'g')
    0x0f50,     // 'H' (+ 'h')
    0x2080,     // 'I' (+ 'i')
    0x1510,     // 'J' (+ 'j')
    0x8648,     // 'K' (+ 'k')
    0x1440,     // 'L' (+ 'l')
    0x0578,     // 'M' (+ 'm')
    0x8570,     // 'N' (+ 'n')
    0x1551,     // 'O' (+ 'o')
    0x0e51,     // 'P' (+ 'p')
    0x9551,     // 'Q' (+ 'q')
    0x8e51,     // 'R' (+ 'r')
    0x9021,     // 'S' (+ 's')
    0x2081,     // 'T' (+ 't')
    0x1550,     // 'U' (+ 'u')
    0x4448,     // 'V' (+ 'v')
    0xc550,     // 'W' (+ 'w')
    0xc028,     // 'X' (+ 'x')
    0x2028,     // 'Y' (+ 'y')
    0x5009,     // 'Z' (+ 'z')
	0x1000

};


/*
const uint8_t sym[] =
{
0x02,	//up1
0x20,	//up2
0x04,	//num1
0x40,	//num2
}; 
*/

#endif

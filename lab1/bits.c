/* 
 * CS:APP Data Lab 
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#include "btest.h"
#include <limits.h>

/*
 * Instructions to Students:
 *
 * STEP 1: Fill in the following struct with your identifying info.
 */
team_struct team =
    {
        /* Team name: Replace with either:
      Your login ID if working as a one person team
      or, ID1+ID2 where ID1 is the login ID of the first team member
      and ID2 is the login ID of the second team member */
        "517030910079",
        /* Student name 1: Replace with the full name of first team member */
        "CHU Yuxuan",
        /* Login ID 1: Replace with the login ID of first team member */
        "517030910079",

        /* The following should only be changed if there are two team members */
        /* Student name 2: Full name of the second team member */
        "",
        /* Login ID 2: Login ID of the second team member */
        ""};

#if 0
/*
 * STEP 2: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.
#endif

/*
 * STEP 3: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the btest test harness to check that your solutions produce 
 *      the correct answers. Watch out for corner cases around Tmin and Tmax.
 */
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 9.0.0.  Version 9.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, fourth edition, plus
   Amd. 1  and Amd. 2 and 273 characters from forthcoming  10646, fifth edition.
   (Amd. 2 was published 2016-05-01,
   see https://www.iso.org/obp/ui/#iso:std:iso-iec:10646:ed-4:v1:amd:2:v1:en) */
/* We do not support C11 <threads.h>.  */
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x)
{
  //the most significant bit of x is its sign bit, -x = ~x + 1
  //and -0 = 0
  //take out the MSB of (x|-x)
  return ~((x | (~x + 1)) >> 31) & 1;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x)
{
  int m1 = 0x11 | (0x11 << 8);
  int mask = m1 | (m1 << 16);
  int s = x & mask;
  s += x >> 1 & mask;
  s += x >> 2 & mask;
  s += x >> 3 & mask;
  s = s + (s >> 16);
  mask = 0xf | (0xf << 8);
  s = (s & mask) + ((s >> 4) & mask);
  return (s + (s >> 8)) & 0x3f;
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x)
{
  //use arithmatic right shift
  return (x << 31) >> 31;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n)
{
  int mask = x >> 31;
  int tmp = (x ^ mask) + (mask & 1);
  int m = ~(((1 << 31) >> n) << 1);
  tmp = (tmp >> n) & m;
  tmp = (tmp ^ mask) + (mask & 1);
  return tmp;
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 2
 */
int evenBits(void)
{
  //left shift
  int word = 0x55 + (0x55 << 8);
  word += word << 16;
  return word;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n)
{
  //first right shift n-1 bits
  //if x can be represented as an n-bit, two's complement interger, the left part should be
  //0xffffffff or 0x0
  int a = x >> (n + (~0));
  int b = a + ((a >> 31) & 1);
  return !b;
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n)
{
  //use masking operation
  int mask = 0xff << (n << 3);
  int ans = x & mask;
  ans = (ans >> (n << 3)) & 0xff;
  return ans;
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y)
{
  //two cases
  //case 1:x and y are the same sign, so no overflow happen, just calculate x - y > 0
  //case 2:x and y are not the same sign. overflow may happen.but obviously the positive one is bigger than the other
  int sign1 = x >> 31;
  int sign2 = y >> 31;

  int same_sign = !(sign1 ^ sign2) & (!(((~y) + x) >> 31)); //Be careful!!!!!should use ~y instead of ~y+1 because x > y strictly

  int not_same_sign = (sign1 ^ sign2) & !(x >> 31) & (y >> 31);

  return (same_sign | not_same_sign);
}
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x)
{
  //check the most significant bit of x
  return !((x >> 31) & 1);
}
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNotEqual(int x, int y)
{
  //use Xor
  //x ^ x = 0
  return !(!(x ^ y));
}
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 60
 *   Rating: 4
 */
int isPower2(int x)
{
  //first calculate the num of 1 in x(binary form)
  //copy the code from bitCount()
  int flag;
  int m1 = 0x11 | (0x11 << 8);
  int mask = m1 | (m1 << 16);
  int s = x & mask;
  s += x >> 1 & mask;
  s += x >> 2 & mask;
  s += x >> 3 & mask;
  s = s + (s >> 16);
  mask = 0xf | (0xf << 8);
  s = (s & mask) + ((s >> 4) & mask);
  s = (s + (s >> 8)) & 0x3f;

  //the result should be 000001
  flag = ((s >> 5) ^ 1) & ((s >> 4) ^ 1) & ((s >> 3) ^ 1) & ((s >> 2) ^ 1) & ((s >> 1) ^ 1);

  //negative number and 0 are not power of 2
  return flag & (~((x >> 31) & 1)) & (s & 1);
}
/* 
 * leastBitPos - return a mask that marks the position of the
 *               least significant 1 bit. If x == 0, return 0
 *   Example: leastBitPos(96) = 0x20
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 4 
 */
int leastBitPos(int x)
{
  //the binary form of -x, -x = ~x + 1
  //will carry 1 bit at the least significant 1 bit
  return x & (~x + 1);
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 1 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3 
 */
int logicalShift(int x, int n)
{
  //make a mask like this: 00....11111 totally n zeros
  int mask = (((~1 + 1) ^ (1 << 31)) >> n) + (1 << (31 + (~n + 1)));
  return (x >> n) & mask;
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y)
{
  int sign1 = x >> 31;
  int sign2 = y >> 31;
  int sum = x + y;
  int sum_sign = sum >> 31;

  //three cases: normal, positive overflow or negative overflow
  int posi_over = ((sign1 ^ 1) & (sign2 ^ 1) & (sum_sign ^ 0)) << 31; //if positive overflow this is 0x80000000, otherwise 0
  int nega_over = ((sign1 ^ 0) & (sign2 ^ 0) & (sum_sign ^ 1)) << 31; //if negative overflow this is 0x80000000, otherwise 0

  return ((posi_over >> 31) ^ posi_over) |
         (nega_over) |
         (sum & ~(posi_over >> 31) & ~(nega_over) >> 31);
}
/* 
 * tc2sm - Convert from two's complement to sign-magnitude 
 *   where the MSB is the sign bit
 *   You can assume that x > TMin
 *   Example: tc2sm(-5) = 0x80000005.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int tc2sm(int x)
{
  //take out the most significant bit of x and rightshift 31
  //for a positive mask is 00000000
  //for a negative mask is ffffffff
  int mask = (x & (1 << 31)) >> 31;

  //remove the most significant bit
  mask = mask ^ (x & (1 << 31));

  //if x is negative x should add 1,while positive needn't
  //so add the most significant bit of x
  return (x ^ mask) + (mask & 1);
}

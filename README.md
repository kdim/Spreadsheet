Spreadsheet
===========

Simple spreadsheet calculator to show my coding ability for employer.

It's not using any 3side library except Google Mock/Test because I got used to it.
This code was formated with high influence of google coding style. I'm sorry if this style
isn't habitual. Certainly I will use coding style that will be in use of employer.

File of sheet should contain lines of values divided by tabulation. Number may contain dot (2.3 
for example), string should start with ('), expressions should start with '=' and can contain
constant, other cell names, operators: +, -, *, /; functions: cos, sin; and (, ). It will be
calculated in natural math order.

This code can:
* Make spreadsheet of string, float-point and expression value
* In expression available +,-,*,/,cos,sin with mathematical order of execution
* Detects many errors|typos in expressions
* Can use in interactive mode
* There is not much testing file because most of test was in second project with google test

There are many simplification to get code small and simple and sometimes because of not using boost etc.
* It was implement uplift parsing just to know how good or bad I remember this part of CS. I know where to read how to make it better&faster. But it's not demonstration of how good I can write parsers.
* Float point number is allowed only in form XXX.YYY with no locale settings and printed in the same way
* Internally project using wide characters but input and output is straightforward ascii.
* Many errors detected but there is no human readable error messages. But it can be implemented fast.
* File names shouldn’t contain spaces or be quoted.
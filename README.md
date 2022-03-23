# B++ or bee++
The good and old B, but better, smarter, easier and maybe faster. B++ is a compiled, kind of functional, half-notyped, low-level programming language.

## Disclaimer
B++ unfortunatelly is not retro-compatible with the old codes - at least not them all. You still are able to compile some examples, but most of them are not possible, both by the changes B++ does and by the fact the compiler is not targeted to the 36-bit mainframes. 

## B++
B++ is Influenced by B, C, Jai, Odin, Rust, Fortran, and Haskell. The syntax, simplicity and utility of B/C, the philosophy and design principles of Jai, Odin and Rust, and some paradigm ideas from Fortran and Haskell. At the end it's just a personal project that I hope someone will find interesting.

## What was B?
[B](https://en.wikipedia.org/wiki/B_(programming_language)) is the father (or mother) of C. Things like treat arrays as pointers and use incrementation or decrementation on pointers came from B (actually from [BCPL](https://en.m.wikipedia.org/wiki/BCPL), but meeeh). It was a lang written by the legendary, smart, greater, dear [Dennis Ritchie](https://en.wikipedia.org/wiki/Dennis_Ritchie) for the 36-bit mainframes (aka really lit old school computers).

## What's new?
Glad you asked. Now we have structures (no more variables named with dots), because now we have a (really weak) type system, and enums!
```c
struct foo {
    x, y;
};
auto bar = foo{6, 7};
printf("%d*n", 48 + bar.x); // prints '6'

enum {
    firsday , seconday ,
    thirday , fourthday,
    fifthday, sixthday
};
printf("today is %d*n", firsday)
```

Fixed that old and bothering problem, the `+=` operator. B used to type `x =+ 4` to sum `4` to `x`, we fixed it just like C, now to write the equivalent to this you need to use the `+=` operator.

Some life quality improvements that [leushenko's](https://github.com/Leushenko/ybc) version of B brings, such as short functions and no need to redefine extern functions on every local function.
```c
extrn printf;

sum(x, y) x + y; // returns x + y

main(){
    extrn sum; // sum is local, so you need to redefine
    printf("%d\n", sum(8, 234)); // printf is global, no need to redefine; prints 242
};
```

Also single line multiple assignment, like:
```c
auto x, y, z = 4, 5, 6;
z, y, x = 5, 6, 7;
```
And much more!

## Progress in
### IMPLEMENTATION
Status: work in progress (25% completed)
- [x] Load source
    - [x] loader
    - [x] lexer
- [ ] Prossessing
    - [ ] syntax analysis (check semantics)
    - [ ] parser (check if everything makes sense and gen final ast)
    - [ ] subcompilations (require libs and extern files)
- [ ] Codegen
    - [ ] optimizations
    - [ ] asm generation
    - [ ] assembling and linking
### DEVELOPMENT
Status: work in progress (60% completed)
- [x] Basic syntax
    - [ ] reserved keywords
    - [x] default formating
- [ ] Design
    - [x] philosophy (radically changed)
    - [ ] how to implement
        - [ ] frontend (still studying compilers)
        - [x] backend (32-bit NASM)
### Documentation
Status: work in progress (33% completed)
- [ ] Tutorial
- [x] Code commenting
- [ ] Additional code documentation

## Principles of Design
B++ is not meant to be a "big agenda language", but instead be a tool for me and my needs. *A problematic language for problematic people*. I don't want to be the new overcoming lang that will replace C, but a replacement of C for me. How **I** needed C to be. I usually type things wrong, forget to change things when copying-pasting and I really fell that I need some hard rules to work properly. B++ fits there. So these are the principles of design and the problems I have that they solve. These are the 3 rocks that over I build my church.

### 1: Readability, consistency and simplicity
Code must be understandable for both humans and computers. And code must be simple as possible - No things like the GNU libc or the Kernel Linux that have thousands of macros that sounds like deep black magic - and easy to read and understand. The language also need to be concise and minimalistic to keep things easy to learn and inplement. Also, easy to keep documented, because documentation is the pillar of all good programs. That implies in:
    * few keywords in the language.<br/>
    * simple and concise semantics and grammar.<br/>
    * no pre-compilation macros or ambiguous desugar code.<br/>
    * no "1 problem, multiple solutions" approach, but instead "thousands of problems, one solution".<br/>

### 2: Safe when testing, fast when running
I want a program that instead of crash and obligate me to debug hours and hours, just to figure out that an array was being read out of bounds, awarns me about where, when and how an invalid read/write happened. But also, when everything is cleaned up and well optimized, I want to make a unsafe-checked program that runs as fast as possible. That inplies in:
    * typesafe and fixed and simple ruleset for all types during compile-time.<br/>
    * memory safe during run-time in debug mode.<br/>
    * massive optimization while producing understandable ASM code in release mode.<br/>

### 3: Reliable and lightweight
Also, the code must be eficient and well developed by the user, to then be well optimized and implemented by the compiler. Both user and program should be reliable for each other. That implies in:
    * total control over optimizations.<br/>
    * compiler complaining about ambiguous or expensive algorithms.<br/>
    * simple yet complete error messages about errors in compile and run-times.<br/>

## Why B?
I'm an old soul trapped in a young body. I like floppy disks, the idea of a mainframe and old languages (such as Fortran and B). And I like the idea of the roots of C. From where we came from. Also, it's a cannonic language in one of my favorite cartoons.

<img src="advanced bee++ coding.gif">

## Building B++
Currently the B++ compiler only suports linux. To build from source you need only gcc >= 4.6.
* Run `build.sh` for debugging (this mode adds address sanitizers and disable optimizations);
* Run `build_release.sh` for release compilation (optimized).

## Special thanks
Thanks [Ginger Bill](https://twitter.com/TheGingerBill) and [Jonathan Blow](https://twitter.com/Jonathan_Blow) for being my programming-language designer heroes, and [rui314](https://github.com/rui314) for making a good compiler building tutorial, the [chibicc](https://github.com/rui314/chibicc).

## Learning B++
Ok, so do you want to code your own little mess with B++? Cool. But first, what do you want to know?
- [Just show me the basics](doc/Tutorial/BASIC.md)
- [An overview of all is a good start](doc/Tutorial/INTERMEDIARY.md)
- [I want to implement my own B++ compiler in B++](doc/Tutorial/ADVANCED.md)
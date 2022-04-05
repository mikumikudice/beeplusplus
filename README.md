# B++ or bee++
The good and old B, but better, smarter and maybe faster. B++ is a compiled, kind of functional, pseudo no-typed, low-level programming language.

## Disclaimer
B++ unfortunatelly is not retro-compatible with the old codes - at least not them all. You still are able to compile some examples, but most of them are not possible, both by the changes B++ does and by the fact the compiler is not targeted to the 36-bit mainframes. 

# Why are you creating B++?
I think I'm done with C. But I still like some of it's features (weak typing, low-level controll, etc), as much as I hate some others (Undefined Behavior, ABI conflicts, etc). I want something new yet old-like. I want to create something that fits better than C for my daily work. At the end it's just a personal project that I hope someone will find interesting.

## Where do B++ come from?
B++ is Influenced by B, C, Jai, Odin, Rust, Pony and Haskell. The syntax, simplicity and utility of B/C, the philosophy and design principles and ideas from Jai, Odin, Rust and Pony and some paradigm ideas from Haskell.

## What was B?
[B](https://en.wikipedia.org/wiki/B_(programming_language)) is the C programming language's mother. Treat arrays as pointers and increment or decrement pointers came from B (actually from [BCPL](https://en.m.wikipedia.org/wiki/BCPL), but _meeeh_). It was a lang written by our dear [Dennis Ritchie](https://en.wikipedia.org/wiki/Dennis_Ritchie) for the 36-bit mainframes (aka really lit old school computers).

## What's new?
Glad you asked. Now we have structures (no more variables named with dots!), because now we have a (really weak) type system, and enums!
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

Fixed that old and bothering problem, the `=+` operator. B used to type `x =+ 4` to sum `4` to `x`, we fixed it just like C. In B++, to write the equivalent to this you need to use the `+=` operator.

Some life quality improvements that [leushenko's](https://github.com/Leushenko/ybc) implementation of the B compiler brings, cuminating in, for example, short, single-lined functions.
```c
extrn printf;

sum(x, y) x + y; // returns x + y

main(){
    extrn printf, sum; // everytime you access an outer scope, you need to use extrn
    printf("%d\n", sum(8, 234)); // prints 242
};
```

The multiple assignment syntax:
```c
auto x, y, z = 4, 5, 6;
z, y, x = 5, 6, 7;
```

Destructive read, borrowed from [Ponylang](https://github.com/ponylang/ponyc):
```c
auto x = 4, y = 3, z = 2;
x = y = x; // x = 3 and y = 4
z = x = z; // z = 3 and x = 2
z = y = x; // z = 4 and y = 2
```

And much more!

## Progress in
### IMPLEMENTATION
Status: work in progress (37.5% completed)
- [x] Load source
    - [x] loader
    - [x] lexer
- [ ] Prossessing
    - [ ] syntax analysis (check semantics)
    - [ ] parser (check if everything is in the right place)
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
    - [x] philosophy
    - [ ] how to implement
        - [ ] frontend (still studying compilers)
        - [x] backend (32-bit NASM)
### Documentation
Status: work in progress (33% completed)
- [ ] Tutorial (grows as language design decisions are taken)
- [ ] Code commenting (work in progress)
- [ ] Additional code documentation

## Principles of Design
B++ is not meant to be a "big agenda language", but instead be a tool for me and my needs. *A problematic language for problematic people*. I don't want to be the new overcoming lang that will replace C, but a replacement of C for me. How **I** needed C to be. I usually type things wrong, forget to change things when copying-pasting and I really feel that I need some hard rules to work properly. B++ fits there. So these are the principles of design and the problems I have that they solve. These are the 3 rocks that over I'll build my church.

### 1: Readability, consistency and simplicity
Code must be understandable for both humans and computers. And code must be simple as possible and easy to read and understand - no things like the GNU libc or the Kernel Linux that have thousands of macros that sounds like deep black magic. The language also need to be concise and minimalistic to keep things easy to learn and implement. Also, easy to keep documented, because documentation is the pillar of all good programs. That implies in:
* few keywords in the language.
* simple and concise semantics and grammar.
* no pre-compilation macros or ambiguous desugar code.
* no "1 problem, multiple solutions" approach, but instead "thousands of problems, one solution".

### 2: Safe when testing, fast when running
 I want a program that instead of crash and obligate me to debug hours and hours, just to figure out that an array was being read out of bounds, warns me about where, when and how an invalid read/write happened. But also, when everything is cleaned up and well optimized, I want to make a unsafe-checked program that runs as fast as possible. That implies in:
* typesafe and fixed and simple ruleset for all types during compile-time.
* memory safe during run-time in debug mode.
* massive optimization while producing understandable NASM code in release mode.

### 3: Reliable and lightweight
 Also, the code must be efficient and well developed by the developer, to then be well optimized and implemented by the compiler. Both developer and program should be reliable for each other. That implies in:
* total control over optimizations.
* compiler complaining about ambiguous or expensive algorithms.
* simple yet complete error messages about errors in compile and run-times.

## Why B?
 I'm an old soul trapped in a young body. I like floppy disks, the idea of a mainframe and old languages (such as Fortran and B). And I like the idea of the roots of C. From where we came from. Also, it's a canonic language in one of my favorite cartoons.

<img src="advanced bee++ coding.gif">

## Building B++
 Currently the B++ compiler only suports linux. To build from source you need only gcc >= 4.6.
* Run `build.sh` for debugging (this mode adds address sanitizers and disable optimizations);
* Run `build_release.sh` for release compilation (optimized).

## Special thanks
 Thanks [Ginger Bill](https://twitter.com/TheGingerBill) and [Jonathan Blow](https://twitter.com/Jonathan_Blow) for being my programming-language designer heroes, and [Rui Ueyama](https://github.com/rui314) for making a good compiler building tutorial, the [chibicc](https://github.com/rui314/chibicc).

## Learning B++
 Ok, so do you want to code your own little mess with B++? Cool. But first, what do you want to know?
- [Just show me the basics](doc/Tutorial/BASIC.md)
- [An overview of all is a good start](doc/Tutorial/INTERMEDIARY.md)
- [I want to implement my own B++ compiler in B++](doc/Tutorial/ADVANCED.md)
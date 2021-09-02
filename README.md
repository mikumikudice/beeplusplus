# B++, bee or incremented B
The good and old B, but better, smarter, easier and maybe faster. B++ is a compiled, almost pure functional, notyped, safe, tiny and lit programming language. _All hail B!_

## B++
B++ is Influenced by B, C, Fortran, Rust, Pony and Haskell. The syntax from B/C, old school vibe and math philosofy from Fortran, safety from Rust and Pony and implementation of pure fuctional programming (or almost it) from Haskell.

## What is B?
[B](https://en.wikipedia.org/wiki/B_(programming_language)) is the father (or mother) of C. Things like treat arrays as pointers and use incrementation or decrementation on pointers came from B. It was a lang written by the good man, legendary, smart, greater, dear [Dennis Ritchie](https://en.wikipedia.org/wiki/Dennis_Ritchie) for the 36-bit mainframes (aka really lit old school computers).

## What's new?
Glad you asked. Now we have structures (no more variables named with dots), implemented behind-the-scenes as a array which items have aliases.
```C
struct foo {
    x, y;
} bar;

auto egg = bar{6, 7};
putc(48 + egg.x); // equivalent to 48 + egg[0] | prints '6'
```
Some memory and runtime safety from Pony and Rust (see #Principles of design), like safe division, looped read from pointers, and 0-filled uninitialized data.
```C
auto x, y, z = {2, 4, 8};
printf("%d\n", x); // prints zero
printf("%d\n", x / y); // prints zero
printf("%d\n", z[5]); // prints 4, because 5 % (len - 1) is 1
printf("%d\n", z[4][2]); // prints 2, because z[4][2] == z[4 + 2] == z[(4 + 2) % (len(z) - 1)] == z[0]
```
Fixed that old and bothering problem fixed by C, but better. I mean the `+=` operator. B used to use `x =+ 4` to sum `4` to `x`, thing that C does better, using `x += 4` to express it. But this change only affects arithimetic binary operators, like sum, division, etc. What let us do things like `x =<= 4` (`x = (x <= 4)`). Awesome, right?

Some life qualities from [leushenko's](https://github.com/Leushenko/ybc) version of B brings, such as short functions, no need to use `extrn` to just invoke functions, etc.
```c
sum(x, y) x + y; // returns x + y

main(){
    printf("%d\n", sum(8, 234)); // prints 242
}
```
## Progress in
### IMPLEMENTATION
Status: work in progress (25% completed)
- [x] Load source
    - [x] loader
    - [x] lexer
- [ ] Prossessing
    - [ ] syntax analysis
    - [ ] parser
    - [ ] semantic analysis
- [ ] Codegen
    - [ ] optimizations
    - [ ] asm generation
    - [ ] assembling and linking
### DEVELOPMENT
Status: work in progress (60% completed)
- [x] Basic syntax
    - [x] reserved keywords
    - [x] default formating
- [ ] Design
    - [x] philosophy
    - [ ] how to implement
        - [ ] frontend (still deciding)
        - [ ] backend

## Principles of Design
The design is simple. We must grant 4 things, the above one is more important than the bellow one and so on.

### Readability
Code must be understandable both for humans and computers. Nothing is more important than a well-writen code. Documentation is the pillar of all good programs, so keep it documented!

### Efficiency
Efficiency is very important, either for the programmer or for the end user. Code must be easy to be implemented _and_ must be well optimized. Speed must be sacrificed in favor of readability. Do things fast is important, but understand what is going on is even more.

### Safety
Code must be safe. Memory safe, type safe and BIOS proof ("Besta Ignorante Operando o Sistema", or "Ignorant Beast Operating the System", in english). The code must be capable of handling common and expectable problems, such as invalid input, low memory avaliable, etc, and the programmer must be able to avoid invalid memory addresses, invalid read / write and mostly undefined behaviour. Safety is important, but if we need to do dangerous things at least do it efficiently.

### Simplicity
And finally, code must be simple to understand and to read. Clean, short and simple code. Things can get more complicated if we need to to keep things safe.

## Why B?
I'm an old soul trapped in a young body. I like floppy disks, the idea of a mainframe and old languages (such as Fortran and B). And I like the idea of the roots of C. From where we came from. Also, it's a cannonic language in one of my favorite cartoons.

<img src="advanced bee++ coding.gif">

## Learning B++
Ok, so do you want to code your own little mess with B++? Cool. But first, let's just find out a really important thing: what kind of programmer are you?
- [I'm a begginer. Like, I don't even know what is programming](doc/Tutorial/BEGINNER.md)
- [I know python or something like this, but C scares me](doc/Tutorial/INTERMEDIARY.md)
- [I'm a pro user of the sorcering machines, just tell me how it works](doc/Tutorial/ADVANCED.md)
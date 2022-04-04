# The basic tutorial
With this you'll learn how to get started. If you are _really_ interested to know all the main features, you can read the [intermediary](INTERMEDIARY.md) tutorial. After that, when you get used to the lang, you can check the [advanced](ADVANCED.md) one to learn some lit stuff like really low-level things.

## The common place
The things you may already know.

### Disclaimer
You'll see several times functions like `putc`, `getc`, `puts`. All those need to be included in your file when writing in real world (see [external code](#External-code) for further explanation). But for reasons of simplicity and didactic, we omitted the external requirement code.

### Hello world
Like, C, but _Better_.
```c
main(){
    putc('hi!*n');
};
```

Note that:
* scape char is star (`*`).<br/>
* the function scope ends with a semicolon (`;`).<br/>
* the literal `'hi!*n'` is a _char_ literal, not a string.<br/>

### Comments
Single lined and multi-lined comments.
```pony
// hello
foo = 45;
/*
    Oh, hello there!
    Nice to meet you
    /*
        nested blocks works too. Yay!
    */
*/
```

### Types definition
 There are only three types in B, that all are subsets of each other. All of them 32 bits (double word). They are `char`, `pntr` (pointer) and `auto` (unsigned integer). You can assign a `char` to `auto`, but not the opposite. A `pntr` can only be assigned by an address (`&char`, `&auto` or `&pntr`) or by another pointer. And finally, `char` can only be assigned with literals, a pointer value (see [pointers](#pointers)) or another `char`. You can also define (multi-dimensional) arrays (both C-like and modern ones) of these types.
```c
char _hi = 'hi!';
auto foo, bar = 4, 5; // multiple assignments are also valid
pntr the = &foo;

bar = _hi; // is a little strange but you can
_hi = foo + 3; // not that safe, so it's not allowed

the = &_hi; // also allowed
the = _hi;  // when a non-address value is assigned to a pointer, it decays into a variable

the = 0xa145b6; // what the heck, extremely prohibited! Only pointers or unary & expressions
```

Note that:
* pointers don't have to be initialized on declaration, but they may not be used before initializing. This is ensured at compile-time. The default value of pointers is `nil`. See [Pointers](BASIC.md#pointers) for further explanation.<br/>

### Literals
It's just a short topic to talk about strings, essentially.

As you'll see in the [fallowing sections](#structures), the string type is not a type actually, but instead a built-in structure. When you type something between double quotes, you are making a literal instance of the structure `string`. Therefore, it has the fields `.len` and `.cap` as well. So the fallowing is totally valid:
```
auto len = "hello!".len;
pntr str = "hi!";
```

Please also note that:
* literal strings are immutable, just like in C.<br/>
* `.len` and `.cap` of literal strings are allways the same.<br/>
* these strings are stored in the `.rodata` section of the produced NASM code (check the [advanced tutorial](ADVANCED.md) for further information).<br/>

It's also a good idea to emphasize that the last bit of the 32 avaliable for each variable are used to indicate if the value of a pointer is a function / structure (see more on [pointers](#pointers)) or a variable address, or a distinct value or a normal one when it's used by a variable, so at the end you have actually 31-bit variables.

### Arrays
The default array in B++, that you may be end using most, is a record of various items of the same type. You can gather a couple of variables together in a single namespace, accessing them by index, like that:
```c
auto[4] nums = {2, 3, 5, 7, 11};                         // default arrays

auto spam = nums[4];     // valid
nums[5] = 13;            // invalid. write out of bounds
nums[nums[3]] = 16;      // invalid too
nums[nums.len - 1] = 13; // you also can access it's length
```

You may also want to use sometimes C-like arrays, that are "pointers for multiple items". You should not use that unless you are dealing with C code or low level stuff, but if you really need to, here's how:
```c
auto[^] fibonacci = {0, 1, 1, 2, 3, 5, 8};

fibonacci = fibonacci[1]; // valid, but please avoid that
fibonacci[3] = 4;         // naturally valid
fibonacci[6] = 13;        // invalid. write out of bounds
                          // (this kind of thing may happen in release mode, but not in debug)
```

Note that:
* there is no dynamic arrays by default.<br/>
* the size of c-like arrays are deduced by the assignment, therefore, can't be defined without initialization.<br/>
* only default arrays has the `len` field, because these are implemented as structures behind-the-scenes.<br/>

Also, it's good to assert that in debug mode, the compiler will work like this with C-like pointers overflow:
```c
auto[^] myarr = {1, 3, 1, 7, 8, 8};
auto index = 4;

myarr[index] = 9; // valid, because 4 <= sizeof(myarr) / sizeof(myarr[0])
index = 7;
myarr[index] = 3; // it should be illegal, right? But how could the compiler check it at compile-time?
```

In cases where happen an overflow/underflow, the index is _sameflowed_ to a valid index, therefore `myarr[7] == myarr[1]`. But again, this only happens in debug compilation. Release presumes that your program is safe and avoid this waste of computation.

Please also note that:
* in release mode, you may still get segfaults due to invalid read/write.<br/>
* you can ask the compiler to add a warning to your bound reads to prompt to you when something like that happens, just set the flag `safety.RW.warning = true` in your header comment (see [compilation flags](#compilation-flags) for further information).<br/>
* arrays and strings are the only built-in structures and the only piece of code with run-time
abstractions (you cannot write on indexes greater than `.cap`)<br/>

## Slightly different
Things you may know how work, but it may look a little different to you.

### Structures
Structures are like arrays, but the items are indexed by name, can have default values, and may be of different types. Being so, you can also address an structure with numeric indexes instead of fields. It's not usual, but possible. This is how you can use structures in B++:
```c
struct string { // no typedef, because there are no user types
    char[cap] val; // this is super valid
    auto len, cap;
};

pntr foo = string{"hello", 5, 6};
foo.val[5] = '!';

printf("%a*n", foo.val);
```

Also, you can use a simple sugar code to access the field `val` of any structure by treating it as a simple variable, just like that:
```c
pntr foo = string{.len = 4, .cap = 8};
foo = "hey!";
printf("%s*n", foo);
```

Note that:
* once structures don't create types, you need a pointer to hold your instances. Also, it's important to say that your structures are passed by reference on assignments, not by value.<br/>
* actually, this code won't compile because the structure `string` is a built-in structure in B++ (this and the array "type" are the only built-in structures).<br/>
* as deducible by the last point, you can't redefine structures' fields in B++, except for the `val` field. <br/>
* `printf` by default treat the corresponding argument to `%s` as a structure. If you're giving a C-like string, use the `%a` instead. It stands for ASCII.<br/>
* this kind of special treatment, the string literal and the enum sentence are the only code-sugars in B++.

### Constants
Constant values are immutable read-only fields that any function can access from inner scopes. To define one, just put the namespace followed by the value, just like that:
```c
myarr {1, 2, 4, 5, 7, 8, 9};
foo, bar 4, 7; // illegal, no multiple assignment on constants

main (){ // the function syntax is just a constant assignment: ``main :: (){}``
    extrn myarr; // "capture" extern namespace
    auto x = myarr[0];
    auto y = other[3];
    auto myarr = 4; // invalid, no redefinitions are allowed

    myfun (x){ return x + 4; }; // nested functions are allowed too
    x = myfun(x);

    pntr pow = (x){ return x * x; }; // lambdas are completely allowed
};
```

 Note that:
* to access outer scopes' namespaces you may use `extrn`, this allow us to know for sure if an function is pure, what values a piece of code need to work when refactoring and kind of creates a type of local encapsulation. This idea is borrowed from Jai (thanks John).<br/>
* functions can only access global fields with extrn.<br/>
* functions are pointers and as such you may treat them.<br/>
* in REAL world, this code would never compile, since it has illegal code, but for didactic reasons let's act as it would do so.<br/>

### Enums and distincts
Distinct values are just like other values when talking about size and arithmetic (despite their value being limited to 16-bit values), but in boolean expressions is when it shines. By turning a distinct value you are saying "hey, compiler. Please do not treat this value as a number when comparing to other things. Instead, treat it as a constant token". Fallowing are the uses for this:
```c
firsday dist 1;
...
sixthday dist 6;

auto today = firsday;

if today == 1 {
    puts("today is the first day of the week!"); // doesn't print
} elif today == firsday {
    puts("today is firsday!"); // prints normally
};

if today * 1 == 1 {
    puts("today is the first day of the week!"); // prints too, because 1 * 1 = 1;
};
```

 Note that:
* you may noticed a new thing: the if/else blocks without parenthesis (also the `elif` keyword). You can check these on [control-structures](BASIC.md#control-structures).<br/>
* doing mathematics on distinct values works normally, resulting in non distinct values.<br/>

 Enums are basically the same thing. Actually, just a short way to define multiple context-related distinct values, just like that:
```c
enum { // no typedef, it's like normal global values
    firsday = 1,
    seconday , thirday ,
    fourthday, fifthday,
    sixthday ,
};

auto today = fourthday;
printf("today is the %dth day of the week, %t*n", today, today);
```

 Note that:
* the distinct values, as constant tokens, can be represented as strings using `getval`. In this case, it's getting read by the `printf` function by referring to it as `%t`. It stands for "token".

You can also define the interval factor of the enum items, like this:
```c
enum * 3 {
    _3x1 = 1,
    _3x2, _3x3,
    _3x4, _3x5
};
printf("%d up to %d*n", _3x1, _3x5); // prints "1 up to 15"
```

### Control structures
Control structures are the way you can make decisions in your program. Simply doing various operations in a row all the way down every time the return is pressed is really boring. Using `if`s and `for` loops, however, is kinda cool. This is how you can use:

#### IF-ELIF-ELSE and SWITCH blocks
```c
auto x = 4, y = 5, z = 6;

if(x == 3){
    printf("true"); // as you may already know
};

if y != x {
    printf("also true"); // as is supposed to be used
} else {
    printf("false too");
};

if auto p = getc(); p == 'y' { // you can define variables within the if-else chain
    printf("you typed yes (%c)*n", p);
} else {
    printf("you typed no (%c)*n", p); // p is visible for other related-blocks too
};

if x - 1 > 0 { x -= 1; }; // no increment/decrement operator by the way
elif x - 2 > 0 { x -= 2; }; // elif is the unambiguous version of `else if`
else { printf("x is %d*n", x); };
```

And if you are ~Swedish~ a Switch enjoyer (not the console), you can replace the if-else chain with `switch`:
```c
auto x = 4;

switch(x){
    1: puts("one");
    2, 3, 5, 7: puts("prime");
    _: puts("composite");
};
```
 
Note that:
* there is no `case` or `default` keywords. Instead, labels defines the cases. The empty case (`_`) replaces the default case.<br/>
* there is no need to use `break`, because there is no fallthrough. Also, you may have noticed that you can handle multiple cases using commas.<br/>
* there is the `break` keyword, but it's used in the next section.<br/>

#### FOR loop
 The `for` loop has two syntaxes, that you may already know. These are the repetition and interaction modes. The repetition is the fallowing one:
```c
for auto c = 0; c < 256; c += 1 { // just like the c for loop
    printf("the %dth UTF-8 char is %c*n", c, c);
};
```
 Note that:
* you may use the `auto` or `char` keywords when using this mode, because unlike the next one, you can't infer the type from the value (actually you can, but a good automatic inference is hard to implement and sometimes also hard to understand).<br/>

The interaction mode is like this:
```c
auto list = {1, 1, 2, 3, 5, 8, 13};

for i in list { // no need to use `auto`
    printf("%d*n", i);
};
```
 Note that:
* it's not needed to define `i` with `auto` or something like that because it is already intended to be a definition, and the type is inferred from `list`, so no need for unnecessary keywords.<br/>
* only these two modes are allowed in B++. The compiler will complain if you don't fallow any of these syntaxes.<br/>

You can use some magic asm jumps and do these basic things too:
```c
for auto n = 1; n < 100; n += 1 {
    if n % 2 == 0 next; // skip to the next loop
    printf("%d*n", n);

    if n == 99 break; // stop the loop
};
```

Editor's note: `next` is superior to `continue` because `next` gives the idea of the next interaction, `continue`, in the other hand, gives the idea of simply continue to executing normally, what's counter intuitive.

### Pointers
Pointers, as the name suggest and you may know, are a way to access an specific memory address i.e. a variable value. By getting its address, you can change its value anywhere in your code simply by reference. The fallowing is an example on how to use pointers in B++:
```c
auto foo = 4;
pntr bar = &foo;
foo = 5;

if(bar == foo){ // pointer decays to its value 
    puts("true!");
} else {
    puts("something's wrong, I can feel it");
};

bar = 6; // assigning to foo (bar's pointed memory), not bar itself

if(foo == 6){
    puts("also true!");
} else {
    puts("I'm sorry, dave. I'm afraid that it's impossible");
};
```

Please note that:
* there is no `*ptr` or `*(&var)` and things like that. Once pointers have strict assignment, it's safe to remove these things, "because the garbage of a pointer is the teasure of a variable".<br/>
* there is no arithmetic with default pointers.<br/>
* if you are a little smart you may have asked "can I pick the address of a pointer?", and the answer is YES!<br/>
* `nil` is the only built-in dist in B++. See [Enums and distincts](BASIC.md#enums-and-distincts) for more information.<br/>

Actually, as pointers can hold variables addresses, structures and functions, you can do all the things you do with the three with pointers:
```c
pntr fun = () puts("hello!");
pntr str = string{"hi!", 3, 4};

struct point {
    x, y;
};
pntr stt = point{3, 5};

auto x = 4;
pntr var = &x;

/* Once `fun.val` is the pointer to the function in the
 * assembly, the printf function reads it as a function
 * pointer, and as it can't be represented as ASCII, it
 * prints "function" instead
 */
printf("%a*n", fun.val);

// prints "pointer" for the same reason
printf("%s*n", var);

// prints "h"
printf("%c*n", str[0]);

// Prints 2, the number of fields of stt
printf("%d*n", stt.len);

/* indexing something that is not a structure results in the
 * value itself, so this prints "4"
 */
printf("%d*n", var[2][1]);
```

Note that:
* `.val` is always the pointer's value, i.e. the C-like pointer to the array, function or variable.<br/>
* `.len` is allways the number of fields (excluding the `.val`, `.len` and `.cap` fields).<br/>
* `.cap` is always zero for structures other than string and array.<br/>
* This is a read-safety feature that is not intend to be used by the programmer in arbitrary ways.<br/>

To pass a pointer to a function as a argument, you need to mark that parameter as so:
```c
take_my_ptr([myptr]){
    myptr[2] = "2";
};

main(){
    extrn take_my_ptr;

    pntr[4] foo = {"1", "1", "3", "5"};
    take_my_ptr(foo);
    
    printf("%r", foo); // prints "1, 2, 3, 5"
};
```

Note that:
* "%r" stands for record and prints arrays.<br/>

# External code
To can include functions and constants from the standard library, other B++ files or from assembly files (no FFI for now). Everytime you put an `extrn` statement at the global scope you are asking the compiler for first search in the standard library, then for other files in the same directory, and at the end for assembly files in the same directory. Once there are no global variables (only within functions) in B++, it is always safe to import extern B++ code because it will work as a library or module. For example:

in `main.bi`:
```c
extrn printl from "my-stdlib.bi";
extrn printf from "stdio";

main(){
    extrn printf, printl;

    printf("running program!*n");
    printl("calling extern function!");
};
```

in `my-stdlib.bi`:
```c
extrn strcat from "stdstr";
extrn puts;

printl([str]){
    extrn strcat, puts;

    pntr out = strcat("[printing from my-stdlib] ", str);
    puts(out);
};
```

This code prints:
    running program!
    [printing from my-stdlib] calling extern function!

Note that:
* Both `stdio` and `stdstr` are not needed to be specified once it's all considered the standard library, so the compiler would already search for those anyway.<br/>

# Compilation flags
In B++ you don't pass the compilation configuration to the compiler by the command line, you pass it through the file itself, by doing this:
```c
/* #config
output.name   = "main.bin"
output.format = "NASM"
safety.RW.warning = true
stdlib.path = "/home/mmd/programs/B++/stdlib/"
*/
extrn println from "mylib" print; // rename println as print

main(){
    extrn print;
    print("hello world!");
};
```

The configuration fields avaliable are:
* ``output``: this field only holds other fields
    * ``.name``: the output file name.
    * ``.format``: the output file format; either "binary", "object file" or "NASM".

* ``safety``: this field also only holds other fields:
    * ``.enabled``: sets the activation state of the safety features.
    * ``.forced``: enables the safety features also in release mode when true.
    * ``.RW``: another field that hold other fields:
        * `.warning`: warns you about read/writes out of bounds.

* ``stdlib``: this field also only holds other fields:
    * ``enabled``: tells the compiler if it should allow the file require constants from the standard library.
    * ``path``: tells the compiler where to look for the standard library. 
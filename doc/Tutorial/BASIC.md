# The basic tutorial
 With this you'll learn how to get started. If you are really interested to know all the main features, you can read the [intermediary](INTERMEDIARY.md) tutorial. After that, when you get used to the lang, you can check the [advanced](ADVANCED.md) tutorial to learn some lit stuff like really low level things.

## The common place
 The things you may already know.

### Disclaimer
 You'll see several times functions like `putc`, `getc`, `puts`. All those need to be included in your file when writing in real world (see [external code](#External-code) for further explanation). But for reasons of simplicity and didactic, we omitted the external requirement code.

### Hello world
 Like, C, but **B**etter.
```c
main(){
    putc('hi!*n');
};
```

 Note that:
* scape char is star (`*`).<br/>
* the function scope ends with a semicolon (`;`).<br/>
* the literal `'hi!*n'` is a _char_ literal, not an string.<br/>

### Comments
 Single lined and multi-lined comments.
```pony
// hello
auto foo = 5;
/*
    Oh, hello there!
    Nice to meet you
    /*
        nested blocks works too. Yay!
    */
*/
```

### Types definition
 There is tree types in B, that all are subsets of each other. All of them 32 bits (double word) They are `char`, `pntr` (pointer) and `auto` (integer). You can assign a `char` to `auto`, but not the opposite. A `pntr` can only be assigned by an address (`&char`, `&auto` or `&pntr`) or by another pointer. And finally, `char` can only be assigned with literals or another `char`. You can also make arrays (both C-like and modern ones) of these types.
```c
char _hi = 'hi!';
auto foo, bar = 4, 5; // multiple assignments are also valid
pntr the = &foo;

bar = _hi; // is a little strange but you can
_hi = foo + 3; // not that safe, so it's not allowed

the = &_hi; // also allowed
the^ = _hi; // ok

the = bar; // not allowed too
the = a145b6h; // what the heck, extremely prohibited!
```

 Note that:
* pointers don't have to be initialized on declaration, but they may not be used before initializing. This is ensured at compile-time. The default value of pointers is `nil`. See [Pointers](BASIC.md#pointers) for further explanation.<br/>
* there is no arithmetic with default pointers.<br/>

### Arrays
 The default array in B++, that you may be using most, is a record of data of the same type. You can gather a couple of variables together in a single namespace, accessing them by index, like that:
```c
auto [4]nums = {2, 3, 5, 7, 11};                         // default arrays

auto spam = nums[4];     // valid
nums[5] = 13;            // invalid. write out of bounds
nums[nums[3]] = 16;      // invalid too
nums[nums.len - 1] = 13; // you also can access it's length
```

 You may also want to use sometimes C-like arrays, that are "pointers for multiple items". You should not use that unless you are dealing with C code or low level stuff, but if you really need to, here's how:
```c
auto [^]fibonacci = {0, 1, 1, 2, 3, 5, 8};

fibonacci = fibonacci[1]; // valid, but please avoid that
fibonacci[3] = 4;         // naturally valid
fibonacci[6] = 13;        // invalid. write out of bounds
                          // (this kind of thing may happen in release mode, but not in debug)
```

Note that:
* there is no dynamic arrays by default.<br/>
* the size of c-like arrays are deduced by the assignment, therefore, can't be defined without initialization.<br/>
* only default arrays has the `len` field, because these are implemented as structures behind-the-scenes.<br/>

## Slightly different
 Things you may know how work, but it may look a little different to you.

### Structures
 Structures are like arrays, but the items are indexed by name, can have default values, and may be of different types. Being so, you can also address an structure with numeric indexes instead of fields. It's not usual, but possible. This is how you can use structures in B++:
```c
struct string { // no typedef, because there are no user types
    pntr val;
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
* `printf` by default treat the corresponding argument to `%s` as a structure. If you're giving and literal string, use the `%a` instead. It stands for ASCII.<br/>
* this kind of special treatment and the enum sentence are the only code-sugars in B++.

### Constants
 Constant values are immutable read-only fields that any function can access from inner scopes. To define one, just put the namespace followed by the value, just like that:
```c
myarr {1, 2, 4, 5, 7, 8, 9};
foo, bar 4, 7; // illegal, no multiple assignment on constants

main (){ // the function syntax is just a constant assignment 
    extrn myarr, other; // "capture" extern namespaces
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
* functions are pointers and as such you may treat them.<br/>
* in REAL world, this code would never compile, since it has illegal code, but for didactic reasons let's act as it would do so.<br/>

### Enums and distincts
 Distinct values are just like other values when talking about size and arithmetic, but in boolean expressions is when it shines. By turning a constant value you are saying "hey, compiler. Please do not treat this value as a number when comparing to other things. Instead, treat it as a constant token". Fallowing are the uses for this:
```c
firsday dist 1;
...
sixthday dist 6;

auto today = firsday;
if today == 1 puts("today is the first day of the week!"); // doesn't print
elif today == firsday puts("today is firsday!"); // prints normally

if today * 1 == 1 puts("today is the first day of the week!"); // prints too, because 1 * 1 = 1;
```

 Note that:
* you may noticed a new thing: the if/else blocks without parenthesis (also the `elif` keyword). You can check these on [control-structures](BASIC.md#control-structures).<br/>
* doing mathematics on distinct values works normally, resulting in non distinct values.<br/>

 Enums are basically the same thing. Actually, just a short way to define multiple context-related distinct values, just like that:
```
enum { // no typedef, it's like normal global values
    firsday = 1,
    seconday , thirday ,
    fourthday, fifthday,
    sixthday ,
}

auto today = fourthday;
printf("today is the %dth day of the week, %t*n", today, today);
```

 Note that:
* the distinct values, as constant tokens, can be represented as strings using `getval`. In this case, it's getting read by the `printf` function by referring to it as `%t`. It stands for "token".

### Control structures
 Control structures are the way you can make decisions in your program. Simply doing various operations in a row all the way down every time the return is pressed is really boring. Using `if`s and `for` loops, however, is kinda cool. This is how you can use:

#### IF-ELIF-ELSE and SWITCH blocks
```c
auto x = 4, y = 5, z = 6;

if(x == 3) printf("true"); // as you may already know
else printf("false");

if y != x {
    printf("also true"); // as is supposed to be used

} else printf("false too"); // `else` is part of the `if` statement

if z == x + 2 printf("equal"); // also valid but not recommended for big statements

if auto p = getc(); p == 'y' { // you can define variables within the if-else chain
    printf("yes");

} else printf("no");

if x - 1 {x -= 1;} // no increment/decrement operator by the way
elif x - 2 {x -= 2;} // elif is the unambiguous version of `else if`
else printf("x is %d*n", x);
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
* you may use the `auto` or any other definition keyword when using this mode, because unlike the next one, you can't infer the type from the value (actually you can, but a good automatic inference is hard to implement and sometimes also hard to understand).<br/>

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

if(bar^ == foo) puts("true!");
else puts("something's wrong, I can feel it");

bar^ = 6;

if(foo == 6) puts("also true!");
else puts("I'm sorry, dave. I'm afraid that it's impossible");
```

 Note that:
* if you are a little smart you may have asked "can I pick the address of a pointer?", and the answer is YES!<br/>
* `nil` is the only built-in dist in B++. See [Enums and distincts](BASIC.md#enums-and-distincts) for more information.<br/>
* As in Odin, "type on the left, usage on the right", so to access a pointer you use the ``^`` operator on the **right**.

# External-code
Temere verba Haec verba temere sine causa sunt. Sine ratione aliud esse quam spatium vacuum replere. Hic sistere potes. Abi nunc aliud vide. Ite nunc, ite. Vale.
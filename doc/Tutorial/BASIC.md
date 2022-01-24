# The basic tutorial
With this you'll learn how to get started. If you are really interested to know all the main features, you can read the [intermediary](INTERMEDIARY.md) tutorial. After that, when you get used to the lang, you can check the [advanced](ADVANCED.md) tutorial to learn some lit stuff like really low level things.

## The common place
The things you already know.

### Hello world
Like, C, but **B**etter.
```c
main(){
    putc('hi!*n');
};
```
Note that:
* scape char is star (`*`);<br/>
* the function scope ends with a semicolon (`;`);<br/>
* the literal `'hi!*n'` is a _char_ literal, not an string;<br/>

### Comments
Single lined and multilined comments.
```c
// hello
auto foo = 5;
/*
    Oh, hello there!
    Nice to meet you
    /*
        nested blocks works too (please ignore
        the fact that in markdown it does not)
    */
*/
```
### Types definition
There is tree types in B, that all are subsets of each other. They are `char` (double word), `pntr` (also double word) and `auto` (32 bits). You can assign a `char` to `auto`, but not the opposite. A `pntr` can only be assigned by an address (`&char` or `&auto`) or by another pointer. `char` can only be assigned with literals or another `char`. You can also make arrays (both C-like and modern ones) of these types.
```
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
* pointers must be initialized on declaration and may be pointing to some address.<br/>
* there is no arithmetic with default pointers.<br/>

### Arrays
Just as was said, there are C-like arrays (that also works as C-like strings) and modern arrays, with length and capacity. These are also used as modern strings. Actually, the modern arrays are structures behind-the-scenes. This is how you use them.
```
auto [4]nums = {2, 3, 5, 7, 11};                         // default arrays
char [^]pron = {'you', 'he', 'she', 'it', 'we', 'they'}; // c-like arrays

auto spam = nums[4]; // valid
pron[2] = 'his';     // valid

nums[5] = 13;      // invalid, write out of scope
pron[0] = 'yours'; // invalid, 'their' is 5 bytes wide, even if it "fits" in the memory slice

pron[nums[3]] = 'your'; // invalid, index out of bounds
pntr myarr = &pron;     // invalid, because pron is already an pointer
```

Note that:
* there is no dynamic arrays by default.<br/>
* the size of c-like arrays are deduced by the assignment, therefore, can't be defined without initialization<br/>
* only default arrays may be used with `length`, but any can be used with `sizeof` and `typeof`.<br/>
* you can get a default pointer's address, but not from a pointer array.<br/>
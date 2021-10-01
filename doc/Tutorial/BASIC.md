### Hello world
Like, C, but **B**etter.
```c
main(){
    putc('hi!*n');
};
```
Scape char is star (`*`);

### Comments
Single lined and multilined comments.
```c
// hello
auto foo = 5;
/*
    Oh, hello there
    Good to meet you
    /*
        nested works too (ignore that in markdown it doesn't)
    */
*/
```
### Variables
Define with auto, assign with equals.
```c
auto foo, bar;
foo = 66;
bar = 'B';

printf("%d*n", foo == bar); // prints 1
```
Notype means that everything is treated as a 32-bit integer value (no floats by default), but these values can be represented by literal numbers, char literals and addresses.

You can use short assignment to variables like you do in Lua and Python:
```c
auto x, y, z = 1, 3, 5;
printf("%d %d %d*n", z, y, x); // prints 5, 3, 1
```
C-like assigment works too
```c
auto x = 4, x = 5;
```
### operators
Arithimetic operators:
```c
auto a, b, c, d, e;
auto x = 1, y = 2, z = 4;
a = x + y; // a = 3
b = z - y; // b = 2
c = y * y; // c = 4
d = z / y; // d = 2
e = y % z; // e = 2
```
Boolean operators:
```c
auto a, b, c, d;
auto x = 1, y = 1, z = 0;
a = x and y; // a = 1
b = x && z;  // "and" and "&&" are the same; b = 0
c = y or z;  // c = 1
a = z || z;  // both are the same; d = 0
```
Bitwise operators
```c
auto a, b, c, d, e, f;
auto x, y, z = 0b101, 0xff0, 07007;
a = ~z;     // a = 0b000111111000, 0x1F8 or 00770
b = x & z;  // b = 0b000000000101, 0x005 or 00005
c = x | y;  // c = 0b111111110101, 0xff5 or 07765
d = y ^ z;  // d = 0b000111110111, 0x1f7 or 00767
e = y >> x; // e = 0b000001111111, 0x07f or 00177
f = x << x; // f = ob000010100000, 0x0A0 or 00240
```
Assign operators
```c
auto j, k, l;
auto m, n, o = 0007, 0070, 0700;
// arithmetic
j += o;
j -= m;     // j = 00671
j *= 8;     // j = 06710
j /= 4;     // j = 01562

// simple (destructive) assignment
m = j = m;  // m = 01562 (old value); j = 00007

// bitwise
m =& j;     // m = 00002
k =| j;     // k = 00002
j =^ n;     // j = 00072
k =<< 2;    // k = 00200
k =>> 1;    // k = 00020

// boolean
l =&& j;    // l = 00000
l =|| n;    // l = 00070

l++;        // l = 00071
m--;        // m = 00001
```
### I/O
You have four main functions for I/O operations. These are:
```c
auto inp;

print("hello!"); // prints a string to the console
inp = getc();    // reads input of user
putc(inp);       // prints a single char to the console
flush();         // clears stdin and flushes stdout (flush here means "make my text apear!")
```

### Control flow
If, else and elif, everything you need to think.
```c
if(c = getc(); c == 'y'){ // local variable like go
    print("confirmed*n");
};
elif c == 'n' { // no parentheses is also valid
    print("refused*n");
};
else print("invalid choice");
```
For loops, you have the good and old for loop. `for` can be used to interact with ranges and can act as an infinite loop:
```c
for auto i = 0; i < 4; i++ { // no parentheses is valid here too
    putc(48 + i);
};
putc('*n');

for auto t = 0; 1 {
    printf("hello");
    if t == 3 break;
};
```
You also have `switch`es, that tries to match values and do something if it do so:
```c
switch auto x = 'hi'; x {                   // you can define variables within it too
    'hey'  : print("yo!*n");        break;  // there is no `case`
    'sup'  : print("what's up?*n"); break;
    'hi'   : print("hello!*n");     break;
    default: break;
}
```
### Functions and constants
Anything out of a function (in the global scope) must be a constant value. That is, use the fallowing syntax:
```
[name] [value]
```
Like
```c
buzz () putc('hi!*n');   // buzz: name | () ...: value
fazz() {putc('bye*n');}; // valid too

cmpl[2] "hello", "bye bye";
primes[5] {2, 3, 5, 7, 11}; // both are valid; with or without brackets
```
You can ommit the `return` keyword if it's the last statlement.
```c
mul(a, b) a * b;

main(){
    printf("%d*n", mul(4, 4)); // prints 16
};
```
### Structures and arrays
Arrays are pointers to multiple addresses. The compiler and the compiled program keep track of the length of every array, so you'll never read invalid locations.
```c
auto arr[] = {2, 4, 6};

putc(48 + arr[0]); // prints '2'
putc(48 + arr[1]); // prints '4'
putc(48 + arr[5]); // prints '6'; 5 % length == 2
```
Structures are like macros, they define a list of words that will later by replaced by indexes.
```c
struct person {
    age, name;
};

auto alexa = person {19, "alexa"};
printf("%s*n", alexa.name); // same of alexa[1]

struct car {
    owner, model;
};

auto modelx = car {alexa, "model x"};
printf("%d*n", modelx.owner.age); // same of modelx[0][0]
```
When indexing a not-array value, you'll always get the value itself. It works something like "get the n-th value of this literal array", but the array has only one value, that is the array itself.

### Enums and distinct values
You can define an compile-time keyword with a numeric value using enums. They will be treated as numbers in arithmetic expressions, but will not return true when compared with numeric values on boolean expressions, e.g.
```c
enum {              // no types, no typedef, no enum name
    JAN, FEB, MAR,
    APR, MAY, JUN,
    JUL, AGO, SEP,
    OCT, NOV, DEC
};

auto day, num, res = FEB, 2;      // res is not assigned

if day == FEB print("true");      // prints true
if day == num print("also true"); // doesn't print

res = day * num;
printf("%d*n", res); // prints 4
```
You can also define it in a shorter way, defining a value as _distinct_. Use the fallowing syntax to do so:
```c
null dist 0;

auto ptr = null;
if data = alloc(1, 4); data != 0 ptr = data;

if ptr == null
    print("an error happened while allocating memory");
else
    print("allocated 4 bytes successfully");
```
Actually alloc already does something like that. If the allocation fails, it returns nil, a built-in distinct 0. Noteworthy even both nil and null being distinct 0's, they aren't the same, so `null == nil` is false.

### High-order functions and call external code
You can pass a function as parameter to another function with no problem, but first you have to declare it as a local value in the current context.
```c
sum(a, b) a + b;

mul(a, b, s){
    for(auto t, out = 0, 0; t <= b; t++){
        out = s(out, a);
    };
    return out;
};

main(){
    extrn sum;
    auto x, y, z = 5, 3, 0;
    z = mul(5, 3, sum);

    printf("%d*n", z); // prints '15'
};
```
To use a fuction from another file you do the same thing.
```c
// main.bi
extrn puts

main(){
    puts("hello world");
};
```
In another file
```c
// puts.bi
puts(str){
    for(auto c = 0; c < length(str); c++){
        putc(str[c]);
    };
    putc('*n');
};
```
Then compile them with `bi main.bi puts.bi`.

### Forced variable kind
There is no type in B++, but there are _kinds_. Actually 4 kinds: value, array, struct and function. The compiler keep track of the kinds based on the first assignment. Once defined as an array, a namespace can not hold any other kind of data. The only kinds that can be switched are struct and arrays, because struct is a subkind of array. You can assign a number to a numeric-kind variable, but not to an array one; you can assign an array to a struct, but not vice-versa.
Also, you can force a variable to be of an expecific kind using tags, e.g.:
```c
getmemsize([array]arr) return length(arr) * sizeof(arr[0]);

main(){
    auto size1 = 0, size2 = 0;
    auto nums[4] = {1, 2, 3, 4};

    size1 = getmemsize(nums); // ok, nums is an array
    size2 = getmemsize(size1); // error! size1's kind is value
};
```
Making dynamic assignment, when a variable sometimes is of a kind and sometimes of another kind, is allowed only when it doesn't envolve the function kind. Because both arrays and structures are treated in the same way, and treating a integer as well isn't problematic at all, because as said befoce, indexing a not-array value returns the value itself. Your code may not work as you expected, but your program wont crash.
To help catch bugs related to it you enable the `--trackidx` flag, that will warn you when an variable is being indexed by a overflowed value or when this variable isn't an array or struct. The tracked indexing flag in action:
```c
print_nth_itm(arr, idx){
    printf("itm %d: %d\n", idx, arr[idx]);
};

main(){
    auto foo[3], bar[2], egg = {4, 8, 16}, {3, 5}, 8;
    print_nth_itm(foo, 2);
    print_nth_itm(bar, 3);
    print_nth_itm(egg, 1);
};
```
The output:
```
itm 2: 16
[warning: "bar" was overflowed by 2 at main.bi:08] itm 3: 5
[warning: "egg" was indexed (it's not an array) at main.bi:09] itm 1: 8
```
It's not meant to you use it in real applications, only for debugging.
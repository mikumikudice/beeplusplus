/* control flow statements */
extrn puts, printf;

main(){
    auto foo = 4;
    auto bar = 3;
    if foo == bar {
        puts("false!");
    } else {
        puts("true!");
    };

    if auto x = foo + bar; x > 4 {
        puts("x is greater than 4");
    } elif x < 7 {
        puts("x is under 7");
    };

    for auto x = 0; x < 5; x += 1 {
        printf("now x is %d*n", x);
    };
};
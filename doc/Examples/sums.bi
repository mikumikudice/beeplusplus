/* do some basic math */
extrn printf;

main([args]){
    auto x = args[0];
    auto y = args[1];

    x += y + (3 - 1);
    x /= 3;

    printf("%d %d*n", x, y);
};
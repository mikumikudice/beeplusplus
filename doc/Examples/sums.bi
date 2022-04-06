/* do some basic math */
extrn printf;

main(){
    auto x = 3;
    auto y = 4;
    x += y + (3 - 1);
    x /= 3;

    printf("%d %d*n", x, y);
};
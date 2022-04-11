/* do some basic math */
extrn printf;

main([args]){
    // check if the given inputs are valid
    if args.len >= 3 and
    typeof(args[1]) == 0 and typeof(args[2]) == 0 {
        auto x = args[1];
        auto y = args[2];

        x += y + (3 - 1);
        x /= 3;

        printf("%d %d*n", x, y);
    };
};
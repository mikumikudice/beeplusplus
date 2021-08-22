set -e
gcc -g ./src/bi.c -o ./bin/bi.bin -fsanitize=leak,address
set -e
gcc -g bi.c -o bi.bin -fsanitize=leak,address
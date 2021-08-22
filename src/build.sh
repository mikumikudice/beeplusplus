set -e
gcc -g bi.c -o bi -fsanitize=leak,address
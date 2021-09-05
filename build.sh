set -e
gcc -g -Wall -Wno-dangling-else -Wno-unused-variable ./src/bi.c -o ./bin/bi -fsanitize=leak,address
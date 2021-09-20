set -e
gcc -g -Wall -Wno-dangling-else -Wno-format -Wno-unused-variable ./src/bi.c -o ./bin/bi -fsanitize=leak,address
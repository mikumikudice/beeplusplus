set -e
mkdir -p ./bin/debug
gcc -g -Wall -Wno-dangling-else -Wno-discarded-qualifiers -Wno-format -Wno-unused-variable ./src/bi.c -o ./bin/debug/bi -fsanitize=leak,address
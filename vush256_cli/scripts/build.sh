
rm -fr ./../build
mkdir ./../build

gcc -Wall -Wextra -Werror -std=gnu11 -I ./../../ -o ./../build/vush256.bin ./../source/main.c

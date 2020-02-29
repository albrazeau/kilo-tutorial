/*
This is a text editor!
I have no idea what I am doing!

https://viewsourcecode.org/snaptoken/kilo/

left off at https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#press-ctrl-q-to-quit
*/

/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/

// maintain the original terminal structure
struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
    // error handling
    perror(s);
    exit(1);
}

void disableRawMode() {
    // called with atexit(), returns the terminal back to normal
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {  
    // turns off many option to transform the terminal into raw mode
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/

int main(){
    // enable raw mode immediately, sets disable raw at exit
    enableRawMode();

    // read character input with error handling, print to screen
    while(1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        // quit the program by typing 'q'
        if (c == 'q') break;
    }

    return 0;
}

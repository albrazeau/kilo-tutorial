/*
This is a text editor.
I have no idea what I am doing!

https://viewsourcecode.org/snaptoken/kilo/

left off at https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#clear-the-screen
*/

/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

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

char editorReadKey() {
    // wait for a single key press, and return it
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) !=1) {
        if (nread == -1 && errno != EAGAIN) die ("read");
    }
    return c;
}

void editorProcessKeypress() {
    // wait for a keypress, and handle it
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
        exit(0);
        break;
    }
}

/*** init ***/

int main(){
    // enable raw mode immediately, sets disable raw at exit
    enableRawMode();

    // read character input with error handling, print to screen
    while(1) {
        editorProcessKeypress();
    }

    return 0;
}

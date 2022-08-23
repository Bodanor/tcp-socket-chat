#include <ncurses.h>

#define ctrl(x) ((x) & 0x1f)
#define STARTX_INPUT 0
#define STARTY_INPUT (LINES - HEIGHT_INPUT - 1)
#define WIDTH_INPUT COLS
#define HEIGHT_INPUT 3

#define STARTX_CHAT 0
#define STARTY_CHAT 1
#define WIDTH_CHAT COLS
#define HEIGHT_CHAT LINES - HEIGHT_INPUT -2

WINDOW *create_newwin(int height, int width, int starty, int startx);
void redraw_input_dialog(WINDOW **oldwin, int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

int main(int argc, char *argv[])
{	
    char buffer[500];
    WINDOW *input_win, *chat_win;
    int ch;
    int i;
    initscr();
    cbreak();
    

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        return -1;
    }

    mvprintw(LINES - 1, COLS - 20, "Press ctr-c to exit");

    refresh();
    chat_win = create_newwin(HEIGHT_CHAT, WIDTH_CHAT, STARTY_CHAT, STARTX_CHAT);
    input_win = create_newwin(HEIGHT_INPUT, WIDTH_INPUT, STARTY_INPUT, STARTX_INPUT);
    wmove(input_win, 1, 2);
    wprintw(input_win, "msg : ");
    move(0, 0);

    printw("Not connected !");
    wrefresh(input_win);
    wrefresh(chat_win);
    refresh();

    i = 0;
    while ((ch = wgetch(input_win)) != ctrl('c')){

        switch(ch){
            case 0xA:
                redraw_input_dialog(&input_win, HEIGHT_INPUT, WIDTH_INPUT, STARTY_INPUT, STARTX_INPUT);
                i = 0;
                break;
            case 127:
                buffer[--i] = '\0';
                redraw_input_dialog(&input_win, HEIGHT_INPUT, WIDTH_INPUT, STARTY_INPUT, STARTX_INPUT);
                wprintw(input_win, buffer);
                

                break;
            default:
                buffer[i++] = ch;
                break;
        }
    }
    
    destroy_win(input_win);
    endwin();			/* End curses mode		  */
    return 0;
}

 void redraw_input_dialog(WINDOW **oldwin, int height, int width, int starty, int startx)
 {
    destroy_win(*oldwin);
    *oldwin = create_newwin(height, width, starty, startx);
    wmove(*oldwin, 1 , 2);
    wprintw(*oldwin, "msg : ");

 }

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    wrefresh(local_win);		/* Show that box 		*/
    box(local_win, 0, 0);
    return local_win;
}

void destroy_win(WINDOW *local_win)
{
    	/* box(local_win, ' ', ' '); : This won't produce the desired
  	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window.  */
    
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');

	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
 	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window */
    
    wrefresh(local_win);
    delwin(local_win);
}

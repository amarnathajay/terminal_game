#include <ncurses.h>
#include <string>
#include <iostream>
#include <unistd.h>

#include "game.h"

#define ESC 27
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

using namespace std;

WINDOW* wnd;

struct
{
	vec2i pos;
	char disp_char;
} player;

int init()
{
	wnd = initscr();
	cbreak();
	noecho();
	clear();
	refresh();
	keypad(wnd, true);
	nodelay(wnd, true);
	curs_set(0);

	if(!has_colors())
	{
		endwin();
		cout << "ERROR: There is no support for colors in the terminal" << endl;
		exit(1);
	}

	start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	wbkgd(wnd, COLOR_PAIR(1));

	attron(A_BOLD);
	box(wnd, 0, 0);
	attroff(A_BOLD);

	return 0;
}

void run()
{
	player.disp_char = '0';
	player.pos = {10, 5};

	int in_char;
	bool exit_requested = false;

	while(1)
	{
		in_char = wgetch(wnd);
		mvaddch(player.pos.y, player.pos.x, ' ');
		
		switch(in_char)
		{
			case ESC:
			{
				exit_requested = true;
			} break;
			
			case 'w':
			{
				player.pos.y -= 1;
			} break;

			case 's':
			{
				player.pos.y += 1;
			} break;

			case 'a':
			{
				player.pos.x -= 1;
			} break;

			case 'd':
			{
				player.pos.x += 1;
			} break;
			
			default:
			{
				//Dont move
			} break;
		}
		mvaddch(player.pos.y, player.pos.x, player.disp_char);
		refresh();

		if(exit_requested) break;

		usleep(10000);


	}
}

void close()
{
	endwin();
}

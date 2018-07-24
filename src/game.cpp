#include <ncurses.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <cmath>

#include "game.h"
#include "ObjectField.h"

#define ESC 27
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

using namespace std;

WINDOW* main_wnd;
WINDOW* game_wnd;

rect game_area;
rect screen_area;

vec2ui cur_size;

ObjectField asteroids;
ObjectField stars;

struct 
{
	vec2i pos;
	rect bounds;
	char disp_char;
	int energy;
} player;

int init()
{
	srand(time(0));
	main_wnd = initscr();
	cbreak();
	noecho();
	clear();
	refresh();
	curs_set(0);

	start_color();
	screen_area = {{0, 0}, {80, 24}};

	int infopanel_height = 4;

	game_wnd = newwin(screen_area.height() - infopanel_height - 2,
						screen_area.width() - 2,
						screen_area.top() + 1,
						screen_area.left() + 1);

	main_wnd = newwin(screen_area.height(), screen_area.width(), 0, 0);

	game_area = {{0, 0}, {screen_area.width() - 2, screen_area.height() - infopanel_height - 4}};
	
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);

	keypad(main_wnd, true);
	keypad(game_wnd, true);

	nodelay(main_wnd, true);
	nodelay(game_wnd, true);

	if(!has_colors()) 
	{
		endwin();
		printf("ERROR: Terminal does not support color.\n");
		exit(1);
	}

	return 0;
}

void run()
{
	int tick;
	
	//init player
	player.disp_char = '0';
	player.pos = {10, 5};

	//keep objects in the game area
	asteroids.setBounds(game_area);
	stars.setBounds(game_area);

	int in_char = 0;
	bool exit_requested = false;
	bool game_over = false;

	const std::vector<std::string> story_text = {
		"Just another Monday, and you're on your way to work...",
		"When suddenly...",
		"You realize you left the oven on!",
		"Take a shortcut through that asteroid field!",
		"Get back to the house before your planet explodes!"
	};

	std::string score = "test";
	int cure_score;

	size_t story_part = 0;
	size_t story_position = 0;

	wattron(main_wnd, A_BOLD);
	box(main_wnd, 0, 0);
	wattroff(main_wnd, A_BOLD);

	wmove(main_wnd, game_area.bot() + 3, 1);
	whline(main_wnd, '-', screen_area.width() - 2);

	wrefresh(main_wnd);
	wrefresh(game_wnd);

	tick = 0;

	while(1)
	{
		werase(game_wnd);

		in_char = wgetch(main_wnd);
		in_char = tolower(in_char);

		//mvwprintw(main_wnd, 24, 5, score);

		if(tick % 50 == 0)
		{
			stars.update();
		}

		for(auto s : stars.getData())
		{
			mvwaddch(game_wnd, s.getPos().y, s.getPos().x, '.');
		}

		if(story_position < story_text[story_part].length())
		{
			wattron(main_wnd, A_BOLD); // makes text added BOLD
			mvwaddch(main_wnd, 20, 5 + story_position, story_text[story_part][story_position]);
			wattroff(main_wnd, A_BOLD); // turns off BOLD effects

			story_position++;
		}

		if(in_char == ' ')
		{
			story_part++;
			story_position = 0;
			mvwhline(main_wnd, 20, 1, ' ', screen_area.width() - 2);
		}

		else if(in_char == ESC)
		{
			exit_requested = true;
			break;
		}

		if(story_part >= story_text.size()) break;
		//usleep(10000);

		switch(in_char)
		{
			case ESC:
			{
				exit_requested = true;
			} break;
			
			case 'w':
			case KEY_UP:
			{
				player.pos.y -= 1;
			} break;

			case 's':
			case KEY_DOWN:
			{
				player.pos.y += 1;
			} break;
			
			case 'a':
			case KEY_LEFT:
			{
				player.pos.x -= 1;
			} break;

			case 'd':
			case KEY_RIGHT:
			{
				player.pos.x += 1;
			} break;

			default:
			{
				//Dont move
            } break;
        }
		if(tick % 7 ==0)
		{
			stars.update();
		}

		for(auto s : stars.getData())
		{
			mvwaddch(game_wnd, s.getPos().y, s.getPos().x, '.');
		}

		if(tick > 100 && tick % 20 == 0)
		{
			asteroids.update();
		}

		for(auto a : asteroids.getData())
		{
			wattron(game_wnd, A_BOLD);
			mvwaddch(game_wnd, a.getPos().y, a.getPos().x, '*');
			wattroff(game_wnd, A_BOLD);
		}

		player.bounds = { { player.pos.x - 1, player.pos.y }, { 3, 1 } }; // player is 3x1

		for(size_t i = 0; i < asteroids.getData().size(); i++) 
		{
			if(player.bounds.contains(asteroids.getData().at(i).getPos()))
			{
				asteroids.erase(i);
			}
		}

		wattron(game_wnd, A_BOLD);
		mvwaddch(game_wnd, player.pos.y, player.pos.x, player.disp_char);
		wattroff(game_wnd, A_BOLD);

		wattron(game_wnd, A_ALTCHARSET);
		mvwaddch(game_wnd, player.pos.y, player.pos.x - 1, ACS_LARROW);
		mvwaddch(game_wnd, player.pos.y, player.pos.x + 1, ACS_RARROW);

		if((tick % 10) / 3)
		{
			wattron(game_wnd, COLOR_PAIR(tick % 2 ? 3 : 4));
			mvwaddch(game_wnd, player.pos.y + 1, player.pos.x, ACS_UARROW);
			wattroff(game_wnd, COLOR_PAIR(tick % 2 ? 3 : 4));
		}

		wattroff(game_wnd, A_ALTCHARSET);

		wrefresh(main_wnd);
		wrefresh(game_wnd);

		if(exit_requested || game_over) break;
		tick++;

		usleep(10000);
	};
	/*uint_fast16_t maxx, maxy;

	//get the boundaries for the window
	getmaxyx(wnd, maxy, maxx);

	rect game_area = {{0, 0}, {maxx, maxy}};
	stars.setBounds(game_area);

	while(1)
	{

		in_char = wgetch(wnd);
		mvaddch(player.pos.y, player.pos.x, ' ');
		
		for(auto s : stars.getData())
		{
			mvaddch(s.getPos().y, s.getPos().x, ' ');
		}

		stars.update();
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

        for(auto s : stars.getData())
		{
			mvaddch(s.getPos().y, s.getPos().x, '*');
		}

		//refresh();

		if(exit_requested) break;

		usleep(10000);

		refresh();

	}*/
}

void close()
{
	endwin();
}

#include "Curses.hpp"

#include <ncurses.h>

namespace graphics
{
	short ColorPair::gCounter = 0;

	ColorPair::ColorPair(Color fg, Color bg)
		: index{++gCounter}, fg{fg}, bg{bg} 
	{
		if (gCounter == 1) 
		{
			[[unlikely]]
			// if (!has_colors())
			// 	throw std::runtime_error("Terminal doesn't support colors!");
			start_color();
		}
		init_pair(index, static_cast<short>(fg), static_cast<short>(bg));
	}

	void init()
	{
		initscr();
		noecho();
		cbreak();
	}

	void shutdown()
	{
		endwin();
	}

	void cursor_set(bool enable)
	{
		curs_set(enable);
	}

	Window::Window(WINDOW* win)
		: win{win} {}

	Window::Window(int h, int w, int y, int x)
	{
		win = newwin(h, w, y, x);
	}

	Window::~Window()
	{
		if (win != ::stdscr)
			delwin(win);
	}

	Window::Window(Window&& window)
		: win{window.win}
	{
		window.win = nullptr;
	}

	Window& Window::operator=(Window&& win)
	{
		this->~Window();
		new(this) Window(std::move(win));
		return *this;
	}

	void Window::on(const ColorPair &color)
	{
		wattron(win, COLOR_PAIR(color.index));		
	}

	void Window::off(const ColorPair& color)
	{
		wattroff(win, COLOR_PAIR(color.index));		
	}

	void Window::keypad(bool enable)
	{
		::keypad(win, enable);
	}

	int Window::width() const 
	{
		return getmaxx(win);
	}

	int Window::height() const 
	{ 
		return getmaxy(win);
	}

	Size Window::size() const
	{
		Size sz;
		getmaxyx(win, sz.height, sz.width);
		return sz;
	}

	Pos Window::pos() const
	{
		Pos res;
		getbegyx(win, res.y, res.x);
		return res;
	}

	void Window::refresh()
	{
		if (win == ::stdscr) ::refresh();
		else wrefresh(win);
	}

	void Window::addchar(char ch) 
	{
		waddch(win, ch);
	}

	void Window::mvaddchar(int y, int x, char ch) 
	{
		mvwaddch(win, y, x, ch);
	}

	void Window::print(std::string_view str)
	{
		wprintw(win, "%s", str.data());
	}

	void Window::mvprint(int y, int x, std::string_view str)
	{
		mvwprintw(win, y, x, "%s", str.data());
	}

	void Window::background(const ColorPair& color)
	{
		wbkgd(win, COLOR_PAIR(color.index));
	}

	std::optional<Key> Window::getkey()
	{
		auto key = wgetch(win);
		if (key == ERR)
			return std::nullopt;
		return static_cast<Key>(key);
	}

	void Window::nodelay(bool enable)
	{
		::nodelay(win, enable);
	}
	
#ifdef border
#undef border
#endif
	void Window::border(const Border &border)
	{
		wborder(win, border.ls, border.rs, border.ts, border.bs,
				border.tl, border.tr, border.bl, border.br);
	}

#ifdef clear
#undef clear
#endif
	void Window::clear()
	{
		wclear(win);
	}

	Window stdscr()
	{
		return Window(::stdscr);
	}

	void beep()
	{
		::beep();
	}
}

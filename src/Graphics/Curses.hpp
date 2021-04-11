#pragma once

#include <string_view>
#include <optional>
#include <chrono>

#include <fmt/core.h>

typedef struct _win_st WINDOW;

namespace graphics
{
	/**
	 * @brief Represents terminal's color
	 */
	enum class Color : short
	{
		BLACK = 0,
		RED = 1,
		GREEN = 2,
		YELLOW = 3,
		BLUE = 4,
		MAGENTA = 5,
		CYAN = 6,
		WHITE = 7,
	};

    /**
	 * @brief Represents curses' color pair paradigm.
	 */
	class ColorPair
	{
	private:
		static short gCounter;
		short index;

	public:
		const Color fg;
		const Color bg;
		/**
		 * @brief Create a color pair.
		 * @param fg foreground color
		 * @param bg background color
		 * @throw std::runtime_error when colors are not 
		 * supported on current terminal
		 */
		ColorPair(Color fg, Color bg);

		friend class Window;
	};

	enum class Key : int
	{
		A = 'A',
		B = 'B',
		C = 'C',
		D = 'D',
		E = 'E',
		F = 'F',
		G = 'G',
		H = 'H',
		I = 'I',
		J = 'J',
		K = 'K',
		L = 'L',
		M = 'M',
		N = 'N',
		O = 'O',
		P = 'P',
		Q = 'Q',
		R = 'R',
		S = 'S',
		T = 'T',
		U = 'U',
		W = 'W',
		X = 'X',
		Y = 'Y',
		Z = 'Z',
		a = 'a',
		b = 'b',
		c = 'c',
		d = 'd',
		e = 'e',
		f = 'f',
		g = 'g',
		h = 'h',
		i = 'i',
		j = 'j',
		k = 'k',
		l = 'l',
		m = 'm',
		n = 'n',
		o = 'o',
		p = 'p',
		q = 'q',
		r = 'r',
		s = 's',
		t = 't',
		u = 'u',
		w = 'w',
		x = 'x',
		y = 'y',
		z = 'z',
		_1 = '1',
		_2 = '2',
		_3 = '3',
		_4 = '4',
		_5 = '5',
		_6 = '6',
		_7 = '7',
		_8 = '8',
		_9 = '9',
		_0 = '0',
		SPACE = ' ',
		CODE_YES	= 0400,
		MIN		= 0401,
		BREAK	= 0401,
		SRESET	= 0530,
		RESET	= 0531,
		DOWN	= 0402,
		UP		= 0403,
		LEFT	= 0404,
		RIGHT	= 0405,
		HOME	= 0406,
		BACKSPACE	= 0407,
		F0		= 0410,
		F1 = 0410 + 1,
		F2 = 0410 + 2,
		F3 = 0410 + 3,
		F4 = 0410 + 4,
		F5 = 0410 + 5,
		F6 = 0410 + 6,
		F7 = 0410 + 7,
		F8 = 0410 + 8,
		F9 = 0410 + 9,
		F10 = 0410 + 10,
		F11 = 0410 + 11,
		F12 = 0410 + 12,
		DL = 		0510,
		IL = 		0511,
		DC = 		0512,
		IC = 		0513,
		EIC = 		0514,
		CLEAR = 	0515,
		EOS = 		0516,
		EOL = 		0517,
		SF = 		0520,
		SR = 		0521,
		NPAGE = 	0522,
		PPAGE = 	0523,
		STAB = 	0524,
		CTAB = 	0525,
		CATAB = 	0526,
		ENTER = 	0527,
		PRINT = 	0532,
		LL = 		0533,
		A1 = 		0534,
		A3 = 		0535,
		B2 = 		0536,
		C1 = 		0537,
		C3 = 		0540,
		BTAB = 	0541,
		BEG = 		0542,
		CANCEL = 	0543,
		CLOSE = 	0544,
		COMMAND = 	0545,
		COPY = 	0546,
		CREATE = 	0547,
		END = 		0550,
		EXIT = 	0551,
		FIND = 	0552,
		HELP = 	0553,
		MARK =	0554,
		MESSAGE =	0555,
		MOVE =	0556,
		NEXT =	0557,
		OPEN =	0560,
		OPTIONS =	0561,
		PREVIOUS =	0562,
		REDO =	0563,
		REFERENCE =	0564,
		REFRESH =	0565,
		REPLACE =	0566,
		RESTART =	0567,
		RESUME =	0570,
		SAVE =0571,
		SBEG =0572,
		SCANCEL =0573,
		SCOMMAND =0574,
		SCOPY =	0575,
		SCREATE =0576,
		SDC = 0577,
		SDL = 0600,
		SELECT =0601,
		SEND =	0602,
		SEOL =	0603,
		SEXIT =	0604,
		SFIND =	0605,
		SHELP =	0606,
		SHOME =	0607,
		SIC = 0610,
		SLEFT =	0611,
		SMESSAGE =0612,
		SMOVE =	0613,
		SNEXT =0614,
		SOPTIONS =0615,
		SPREVIOUS =0616,
		SPRINT =0617,
		SREDO =0620,
		SREPLACE =0621,
		SRIGHT =0622,
		SRSUME =0623,
		SSAVE =0624,
		SSUSPEND =0625,
		SUNDO =0626,
		SUSPEND =0627,
		UNDO =0630,
		MOUSE =0631,
		RESIZE =0632,
		EVENT =0633,
		MAX = 0777,
	};

    /**
	 * @brief Initialize underlying curses library
	 */
	void init();
	/**
	 * @brief Deinitialize underlying curses library
	 * @note Call this function at the end of program
	 */
	void shutdown();
	/**
	 * @brief Toggle cursor visibility
	 */
	void cursor_set(bool enable);

	struct Size { int height; int width; };
	struct Pos { int y; int x; };

	struct Border
	{
		char ls, rs, ts, bs, 
			tl, tr, bl, br;
	};

	/**
	 * @brief A thin wrapper around WINDOW from `ncurses.h`
	 */
	class Window
	{
	private:
		WINDOW* win;

		Window(WINDOW* win);

	public:
		/**
		 * @brief Create window
		 */
		Window(int h, int w, int y, int x);
		~Window();
		Window(Window&& win);
		Window& operator=(Window&& win);

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		/**
		 * @brief Use @a color for printing, same to attron(COLOR_PAIR(...))
		 */
		void on(const ColorPair& color);
		/**
		 * @brief Disable @a color for printing, same to attroff(COLOR_PAIR(...))
		 */
		void off(const ColorPair& color);
		void keypad(bool enable);
		[[nodiscard]]
		int width() const;
		[[nodiscard]]
		int height() const;
		[[nodiscard]]
		Size size() const;
		[[nodiscard]]
		Pos pos() const;
		/**
		 * @brief Equivalent to wrefresh() from `ncurses.h`
		 */
		void refresh();
		void addchar(char ch);
		void mvaddchar(int y, int x, char ch);
		void print(std::string_view str);
		/**
		 * @brief Same to wprintw from `ncurses.h` but with {fmt} library's formatting.
		 */
		template<typename... A>
		void print(std::string_view fmt, A&&... args)
		{
			print(fmt::format(fmt, std::forward<A>(args)...));
		}
		void mvprint(int y, int x, std::string_view str);
		/**
		 * @brief Same to mvwprintw from `ncurses.h` but with {fmt} library's formatting.
		 */
		template<typename... A>
		void mvprint(int y, int x, std::string_view str)
		{
			mvprint(y, x, str);
		}
		void background(const ColorPair& color);
		/**
		 * @brief Get a char from keyboard(equivalent to @a wgetch()).
		 * @detail If @a nodelay is set to true then
		 * if no key was pressed returns @a null else this
		 * function blocks current thread until user presses
		 * a key.
		 */
		std::optional<Key> getkey();
		void nodelay(bool enable);
		void border(const Border& border);
		void clear();

		friend Window stdscr();
	};

	/**
	 * @brief Same to stdscr from `ncurses.h`
	 */
	[[nodiscard]]
	Window stdscr();

	void beep();
}

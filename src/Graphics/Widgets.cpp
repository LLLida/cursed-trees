#include "Widgets.hpp"

namespace graphics
{
	void widgets::print_in_middle(Window& window, int y, std::string_view str)
	{
		int cols = window.width();
		int x = (cols - str.size()) >> 1;
		window.mvprint(y, x, str);
	}

	void widgets::status_load(Window& window, int y, int x, int frameNo)
	{
		switch(frameNo % 5)
		{
			case 0:
				window.mvprint(y, x, "[=  ]");
				break;
			case 1:
				window.mvprint(y, x, "[== ]");
				break;
			case 2:
				window.mvprint(y, x, "[===]");
				break;
			case 3:
				window.mvprint(y, x, "[ ==]");
				break;
			case 4:
				window.mvprint(y, x, "[  =]");
				break;
		}
	}

	widgets::LinePrinter::LinePrinter(int w, int y, int x)
		: Window{1, w, y, x}
		, frame_counter{0}
	{
		
	}

	void widgets::LinePrinter::draw(int space_between_parts)
	{
		int width = Window::width();
		int sz = message.size();
		std::string_view view = message;
		clear();
		if (width < sz) /* Message doesn't fit the window so we scroll it
						 * horizontally.
						 */
		{
            /* Left part */
			int left_offset = frame_counter % width;
			view.remove_prefix(left_offset);

            /* Right part */
			int right_offset = (sz << 1) - left_offset + space_between_parts - width;
			if (right_offset > 0)
			{
				std::string_view rview = message;
				rview.remove_suffix(right_offset);
				mvprint(0, width - rview.size(), rview);
			}
		}
		mvprint(0, 0, view);
		refresh();
		frame_counter++;
	}

	void widgets::LinePrinter::draw(const ColorPair& color, int space_between_parts)
	{
		on(color);
		draw(space_between_parts);
		off(color);
	}

	widgets::PropertyLine::PropertyLine(int w, int y, int x)
		: Window(1, w, y, x)
	{
		
	}

	void widgets::PropertyLine::draw()
	{
		Window::clear();
		for (auto&& [pos, str] : properties)
				mvprint(0, pos, str);
		refresh();
	}

	widgets::Screen::Screen()
		: header_line(stdscr().width(), 0, 0)
		, surface(stdscr().height() - 2, stdscr().width(), 1, 0)
		, message_line(stdscr().width(), stdscr().height() - 1, 0)
	{
		
	}

	void widgets::Screen::draw()
	{
		message_line.draw();
		header_line.draw();
		surface.refresh();
	}

	void widgets::Screen::resize()
	{
		auto w = stdscr().width();
		auto h = stdscr().height();
		header_line.resize(1, w);
		surface.resize(h - 2, w);
		message_line.move(h -1, 0);
		message_line.resize(1, w);
		message_line.print("Resized screen to [{}, {}].", w, h);
	}

	widgets::VerticalScrollBar::VerticalScrollBar(int height, int y, int x)
		: Window(height, 1, y, x)
		, changed(false)
	{
		
	}

	void widgets::VerticalScrollBar::setValues(int top_, int bottom_)
	{
		top = top_;
		bottom = bottom_;
		changed = true;
	}

	void widgets::VerticalScrollBar::draw()
	{
		if (!changed)
			return;
		changed = false;

		clear();
		mvaddchar(0, 0, '^');
		for (int i = top; i <= bottom; i++)
			mvaddchar(i, 0, '|');
		mvaddchar(height() - 1, 0, 'v');
		refresh();
	}

	widgets::HorizontalScrollBar::HorizontalScrollBar(int width, int y, int x)
		: Window(1, width, y, x)
		, changed(false)
	{
		
	}

	void widgets::HorizontalScrollBar::setValues(int left_, int right_)
	{
		left = left_;
		right = right_;
		changed = true;
	}

	void widgets::HorizontalScrollBar::draw()
	{
		if (!changed)
			return;
		changed = false;

		clear();
		mvaddchar(0, 0, '<');
		for (int i = left; i <= right; i++)
			mvaddchar(0, i, '-');
		mvaddchar(0, width() - 1, '>');
		refresh();
	}
}

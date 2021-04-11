#pragma once

#include "Curses.hpp"

#include <string>
#include <map>

namespace graphics
{
	inline namespace widgets
	{
        /**
		 * @brief Print a message(@a str) in middle of line(@a y) in @a window.
		 */
		void print_in_middle(Window& window, int y, std::string_view str);
        /**
		 * @brief Print a formatted message(@a str) in middle of line(@a y) in @a window.
		 */
		template<typename... A>
		void print_in_middle(Window& window, int y, std::string_view fmt, A&&... args)
		{
			print_in_middle(window, y, fmt::format(fmt, std::forward<A>(args)...));
		}
		void status_load(Window& window, int y, int x, int frameNo);

		class LinePrinter : public Window
		{
		private:
			std::string message;
			unsigned frame_counter;

		public:
			LinePrinter(int w, int y, int x);

			void draw(int space_between_parts = 3);
			void draw(const ColorPair& color, int space_between_parts = 3);

			template<typename... A>
			void print(std::string_view fmt, A&&... args)
			{
				message = fmt::format(fmt, std::forward<A>(args)...);
				clear();
				frame_counter = 0;
			}
		};

		class PropertyLine : public Window
		{
		public:
			std::map<int, std::string> properties;

			PropertyLine(int w, int y, int x);

			void draw();
		};

		class Screen
		{
		public:
			PropertyLine header_line;
			Window surface;
			LinePrinter message_line;

			Screen();

			void draw();
		};

		class VerticalScrollBar : public Window
		{
		private:
			int top;
			int bottom;
			bool changed;

		public:
			VerticalScrollBar(int height, int y, int x);
			void setValues(int top, int bottom);
			void draw();
		};

		class HorizontalScrollBar : public Window
		{
		private:
			int left;
			int right;
			bool changed;

		public:
			HorizontalScrollBar(int width, int y, int x);
			void setValues(int left, int right);
			void draw();
		};
	}
}

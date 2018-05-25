/**
 * Created by universal on 8/1/17.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */


#include "Textbox.hpp"
#include "../../util/SDL_helper.hpp"

class SDL_helper;

Textbox::Textbox(int8_t id, int x, int y, int w, int h, std::string text, Dialog *parent)
{
	SDL_Rect temp = { x, y, w, h };
	init(parent, temp, id);
	set_text(text);
	m_focused = false;
}

Textbox::~Textbox()
{
	close();
}

void Textbox::close(void)
{
	GuiElement::close();
	m_cut_text.clear();
	m_focused = false;
}

void Textbox::draw_foreground(void)
{
	GuiElement::draw_foreground();
	int cursor_pos = get_left() + 2;

	if (!m_cut_text.empty())
	{
		cursor_pos += get_helper()->util_text_utf8_dim(&m_cut_text).w;
		get_helper()->util_text_utf8(&m_cut_text, get_left() + 2, get_top() + 2,
			get_helper()->palette()->white());
	}

	if (!m_composition.empty())
	{
		SDL_Rect temp = get_helper()->util_text_utf8_dim(&m_cut_text);
		get_helper()->util_text_utf8(&m_composition, get_left() + 2 + temp.w, get_top() + 2,
			get_helper()->palette()->blue());
		cursor_pos += temp.w;
	}

	SDL_Rect temp = { cursor_pos, get_top() + 2, 2, get_dimensions()->h - 4 };
	get_helper()->util_fill_rect(& temp, get_helper()->palette()->white());
}

void Textbox::draw_background(void)
{
	get_helper()->util_fill_rect(get_dimensions(), get_helper()->palette()->gray());
	if (m_focused)
	{
		get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->light_gray());
	}
	else
	{
		get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->dark_gray());
	}
}

void Textbox::handle_events(SDL_Event * event)
{
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			m_focused = is_mouse_over(event->button.x, event->button.y);
			if (m_focused)
			{
				SDL_StartTextInput();
				SDL_SetTextInputRect(get_dimensions());
			}
			else
			{
				SDL_StopTextInput();
				SDL_SetTextInputRect(get_dimensions());
			}
		}
	}
	else if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_v && get_helper()->is_ctrl_down())
		{
			if (SDL_HasClipboardText())
			{
				
				append_text(std::string(SDL_GetClipboardText()));
			}
		}
		else if (event->key.keysym.sym == SDLK_BACKSPACE)
		{
			if (!m_text.empty())
			{
				m_text.pop_back();
				set_text(m_text);
			}
		}
		else if (event->key.keysym.sym == SDLK_RETURN)
		{
			m_composition.clear();
		}
	}
	else if (event->type == SDL_TEXTINPUT)
	{
		append_text(std::string(event->text.text));
	}
	else if (event->type == SDL_TEXTEDITING)
	{
		m_composition = event->edit.text;
	}
}

void Textbox::set_text(std::string s)
{
	m_text = s;
	m_cut_text = m_text;
	/*
		We have to leave space for the composition
		which is the currently written text through
		the IME (Only for Japanese, Chinese etc.
	*/
	get_helper()->util_cut_string(m_cut_text.append(m_composition), get_dimensions()->w - 22, false);
}

void Textbox::append_text(std::string s)
{
	set_text(m_text.append(s));
}
#ifndef WEBVIEW_HPP
#define WEBVIEW_HPP

#include "helpers.hpp"

/** @brief Control that displays web content from a URL. */
class WebView : public Control
{
public:
	string url;
	WebView() : url() {}
	/** @brief Draws this control and, where applicable, its child controls. */
	void draw(Display *display) override
	{
		(void)display;
	}
};
#endif

#ifndef WEBVIEW_HPP
#define WEBVIEW_HPP

#include "helpers.hpp"

/** @brief Represents the WebView type. */
class WebView : public Control
{
public:
	string url;
	WebView() : url() {}
	/** @brief draw operation. */
	void draw(Display *display) override
	{
		(void)display;
	}
};
#endif

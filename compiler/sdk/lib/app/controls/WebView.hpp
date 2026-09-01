#ifndef WEBVIEW_HPP
#define WEBVIEW_HPP

#include "helpers.hpp"

class WebView : public Control
{
public:
	string url;
	WebView() : url() {}
	void draw(Display *display) override
	{
		(void)display;
	}
};
#endif

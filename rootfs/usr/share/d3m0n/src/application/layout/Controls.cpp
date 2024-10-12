#include "layoutHandler.h"


#include "./Controls/GeneralAttributes.cpp"
#include "./Controls/Window.cpp"
#include "./Controls/Text.cpp"
#include "./Controls/Image.cpp"
#include "./Controls/TextBox.cpp"
#include "./Controls/Button.cpp"
#include "./Controls/Rect.cpp"



template<typename T>
void parseAttributes(T* ctrl, string controlAttributes, void (&function)(T* ctrl2, string key2, string value2)) {
    istringstream iss(controlAttributes);
    string token;
    string token2;

    // extract  attributes from the attribute string
    bool FirstLine = true;
    while (getline(iss, token, ';')) {
        // create pairs of key=value
        istringstream line(token);

        string Key;
        while (getline(line, token2, '=')) {
            if(FirstLine) {
                Key = delete_space(token2.c_str());
            }
            else
            {
                function(ctrl, Key, token2);
                // cout << " Value: " << token2 << endl;
            }
            FirstLine=!FirstLine;   
        }
        
    }
}

Control* loadControl(string controlName, string controlAttributes) {
    Control* controlElement = nullptr;

    if(controlName == "Window") {
		layout::Window(controlName, controlAttributes);
	}
	else if(controlName=="Text") {
        layout::Text* temp = new layout::Text(controlAttributes.c_str());
        controlElement = temp->Get();
	    parseAttributes(temp, controlAttributes, layout::Text::parseAttributes);
	}
	// else if(controlName=="RawHtml") {
	// 	return layout.RawHtml(controlAttributes);
	// }
	else if(controlName=="TextBox") {
		layout::TextBox* temp = new layout::TextBox(controlAttributes.c_str());
        controlElement = temp->Get();
	    parseAttributes(temp, controlAttributes, layout::TextBox::parseAttributes);
	}
	// else if(controlName=="ListView") {
	// 	return layout.ListView(controlAttributes, this, ctrl as ListView);
	// }
	// else if(controlName=="ProgressBar") {
	// 	return layout.ProgressBar(controlAttributes, this, ctrl as ProgressBar);
	// }
	// else if(controlName=="CheckBox") {
	// 	return layout.CheckBox(controlAttributes, this, ctrl as CheckBox);
	// }
	// else if(controlName=="RadioButton") {
	// 	return layout.RadioButton(controlAttributes, this, ctrl as RadioButton);
	// }
	else if(controlName=="Rect") {
		layout::Rect* temp = new layout::Rect(controlAttributes.c_str());
        controlElement = temp->Get();
	    parseAttributes(temp, controlAttributes, layout::Rect::parseAttributes);
	}
	// else if(controlName=="Switch") {
	// 	return layout.Switch(controlAttributes, this, ctrl as CheckBox);
	// }
	else if(controlName=="Image") {
		layout::Image* temp = new layout::Image(controlAttributes.c_str());
        controlElement = temp->Get();
	    parseAttributes(temp, controlAttributes, layout::Image::parseAttributes);
	}
	else if(controlName=="Button") {
		layout::Button* temp = new layout::Button(controlAttributes.c_str());
        controlElement = temp->Get();
	    parseAttributes(temp, controlAttributes, layout::Button::parseAttributes);
	}
	// else if(controlName=="Vscroll") {
	// 	return layout.Vscroll(controlAttributes, this, ctrl as Panel);
	// }
	// else if(controlName=="Hscroll") {
	// 	return layout.Hscroll(controlAttributes, this, ctrl as Panel);
	// }
	// else if(controlName=="RoundButton") {
	// 	return layout.RoundButton(controlAttributes, this, ctrl as RoundButton);
	// }
    return controlElement;
}
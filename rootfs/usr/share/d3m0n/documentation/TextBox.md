# TextBox

## TextBox special attributes
    TextBox:
        text_align="[top/top_left/top_right/bottom/bottom_left/bottom_right/left/right/center]";
        font_size="10";
	    type="password";
	    content="text";
     	    bold="[true/false]";

You can also use [general attributes](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/GeneralAttributes.md) on this control

### Description
`text_align` (string): set's text position (top/top_left/top_right/bottom/bottom_left/bottom_right/left/right/center)

`font_size` (float): sets the TextBox text font size

`type` (string): can be equal to **password** if the TextBox content needs to be hidden

`content` (string): text of the TextBox

`bold` (bool) define if text should be displayed as bold

# Text

## Text special attributes
    Text:
        text_align="[top/top_left/top_right/bottom/bottom_left/bottom_right/left/right/center]";
        font_size="10";
	    content="text";

You can also use [general attributes](GeneralAttributes.md) on this control

### Description
`text_align` (string): set's text position (top/top_left/top_right/bottom/bottom_left/bottom_right/left/right/center)

`font_size` (float): sets Text font size

If `font_size` is not set and `width` and/or `height` are set, the text size is auto-computed to fit the provided bounds.

`content` (string): text of the Text control

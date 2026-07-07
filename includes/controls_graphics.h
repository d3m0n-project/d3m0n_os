#ifndef CONTROLS_GRAPHICS_H
#define CONTROLS_GRAPHICS_H

#include "controls.h"
#include "log.h"

#define SCROLLBAR_SIZE				10
#define SCROLLBAR_THUMB_MIN_HEIGHT	30

void	ctrl_draw_textbox(t_control *control);
void	ctrl_draw_listview(t_control *control);
void	ctrl_draw_progressbar(t_control *control);
void	ctrl_draw_checkbox(t_control *control);
void	ctrl_draw_radiobutton(t_control *control);
void	ctrl_draw_webview(t_control *control);
void	ctrl_draw_rect(t_control	*control);
void	ctrl_draw_switch(t_control	*control);
void	ctrl_draw_text(t_control	*control);
void	ctrl_draw_image(t_control	*control);
void	ctrl_draw_roundbutton(t_control	*control);
void	ctrl_draw_button(t_control	*control);
void	ctrl_draw_hscroll(t_control	*control);
void	ctrl_draw_vscroll(t_control	*control);

/* helpers */
int		control_text_auto_font_size(const t_control *control);
void	control_text_metrics(const char *text, int *out_max_len, int *out_lines);
void	compute_inner_rect(const t_control *control, int *out_x, int *out_y, int *out_w, int *out_h);
void	compute_text_position(int anchor, int box_x, int box_y, int box_w, int box_h, int text_w, int text_h, int *out_x, int *out_y);


#endif
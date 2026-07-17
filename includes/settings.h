#ifndef SETTINGS_H
#define SETTINGS_H

#include "types.h"

#define	STRING_SIZE	128

#define CONF_FIELDS(X) \
	X(STR, wallpaper,						"default.bmp") \
	X(INT, theme,							0, 0, 1) \
	X(INT, time_mode,						0, 0, 1) \
	X(STR, launcher,						"com.4re5.d3m0n.system.launcher") \
	X(STR, icon_pack,						"material-design-icons") \
	X(STR, icon_airplane_mode,				"device.airplanemode_active") \
	X(STR, icon_wifi_lock,					"device.wifi_lock") \
	X(STR, icon_wifi_off,					"device.signal_wifi_off") \
	X(STR, icon_wifi_level0,				"device.signal_wifi_0_bar") \
	X(STR, icon_wifi_level1,				"device.signal_wifi_1_bar") \
	X(STR, icon_wifi_level2,				"device.signal_wifi_2_bar") \
	X(STR, icon_wifi_level3,				"device.signal_wifi_3_bar") \
	X(STR, icon_wifi_level4,				"device.signal_wifi_4_bar") \
	X(STR, icon_wifi,						"notification.wifi") \
	X(STR, icon_wifi_lock_level1,			"device.signal_wifi_1_bar_lock") \
	X(STR, icon_wifi_lock_level2,			"device.signal_wifi_2_bar_lock") \
	X(STR, icon_wifi_lock_level3,			"device.signal_wifi_3_bar_lock") \
	X(STR, icon_wifi_lock_level4,			"device.signal_wifi_4_bar_lock") \
	X(STR, icon_battery_20,					"device.battery_20") \
	X(STR, icon_battery_30,					"device.battery_30") \
	X(STR, icon_battery_50,					"device.battery_50") \
	X(STR, icon_battery_60,					"device.battery_60") \
	X(STR, icon_battery_80,					"device.battery_80") \
	X(STR, icon_battery_90,					"device.battery_90") \
	X(STR, icon_battery_100,				"device.battery_full") \
	X(STR, icon_battery_alert,				"device.battery_alert") \
	X(STR, icon_battery_charging_20,		"device.battery_charging_20") \
	X(STR, icon_battery_charging_30,		"device.battery_charging_30") \
	X(STR, icon_battery_charging_50,		"device.battery_charging_50") \
	X(STR, icon_battery_charging_60,		"device.battery_charging_60") \
	X(STR, icon_battery_charging_80,		"device.battery_charging_80") \
	X(STR, icon_battery_charging_90,		"device.battery_charging_90") \
	X(STR, icon_battery_charging_100,		"device.battery_charging_full") \
	X(STR, icon_battery_standard,			"device.battery_std") \
	X(STR, icon_battery_unknown,			"device.battery_unknown") \
	X(STR, icon_bluetooth,					"device.bluetooth") \
	X(STR, icon_bluetooth_connected,		"device.bluetooth_connected") \
	X(STR, icon_bluetooth_disabled,			"device.bluetooth_disabled") \
	X(STR, icon_bluetooth_searching,		"device.bluetooth_searching") \
	X(STR, icon_cellular_level0,			"device.signal_cellular_0_bar") \
	X(STR, icon_cellular_level1,			"device.signal_cellular_1_bar") \
	X(STR, icon_cellular_level2,			"device.signal_cellular_2_bar") \
	X(STR, icon_cellular_level3,			"device.signal_cellular_3_bar") \
	X(STR, icon_cellular_level4,			"device.signal_cellular_4_bar") \
	X(STR, icon_cellular_no_internet0,		"device.signal_cellular_connected_no_internet_0_bar") \
	X(STR, icon_cellular_no_internet1,		"device.signal_cellular_connected_no_internet_1_bar") \
	X(STR, icon_cellular_no_internet2,		"device.signal_cellular_connected_no_internet_2_bar") \
	X(STR, icon_cellular_no_internet3,		"device.signal_cellular_connected_no_internet_3_bar") \
	X(STR, icon_cellular_no_internet4,		"device.signal_cellular_connected_no_internet_4_bar") \
	X(STR, icon_cellular_off,				"device.signal_cellular_off") \
	X(STR, icon_cellular_no_sim,			"device.signal_cellular_no_sim") \
	X(STR, icon_cellular_null,				"device.signal_cellular_null") \
	X(STR, icon_cellular_alt,				"device.signal_cellular_alt") \
	X(STR, icon_sd_storage,					"device.sd_storage") \
	X(STR, icon_settings,					"action.settings") \
	X(STR, icon_search,						"action.search") \
	X(STR, icon_home,						"action.home") \
	X(STR, icon_lock,						"action.lock") \
	X(STR, icon_lock_open,					"action.lock_open") \
	X(STR, icon_power,						"action.power_settings_new") \
	X(STR, icon_backup,						"action.backup") \
	X(STR, icon_restore,					"action.restore") \
	X(STR, icon_sync,						"notification.sync") \
	X(STR, icon_sync_disabled,				"notification.sync_disabled") \
	X(STR, icon_cached,						"action.cached") \
	X(STR, icon_account,					"action.account_circle") \
	X(STR, icon_visibility,					"action.visibility") \
	X(STR, icon_visibility_off,				"action.visibility_off") \
	X(STR, icon_favorite,					"action.favorite") \
	X(STR, icon_favorite_border,			"action.favorite_border") \
	X(STR, icon_error,						"alert.error") \
	X(STR, icon_warning,					"alert.warning") \
	X(STR, icon_notification,				"alert.notification_important") \
	X(STR, icon_info,						"action.info") \
	X(STR, icon_help,						"action.help") \
	X(STR, icon_bug_report,					"action.bug_report") \
	X(STR, icon_alarm,						"action.alarm") \
	X(STR, icon_alarm_on,					"action.alarm_on") \
	X(STR, icon_alarm_off,					"action.alarm_off") \
	X(STR, icon_schedule,					"action.schedule") \
	X(STR, icon_today,						"action.today") \
	X(STR, icon_event,						"action.event") \
	X(STR, icon_history,					"action.history") \
	X(STR, icon_play,						"av.play_arrow") \
	X(STR, icon_pause,						"av.pause") \
	X(STR, icon_stop,						"av.stop") \
	X(STR, icon_next,						"av.skip_next") \
	X(STR, icon_previous,					"av.skip_previous") \
	X(STR, icon_fast_forward,				"av.fast_forward") \
	X(STR, icon_fast_rewind,				"av.fast_rewind") \
	X(STR, icon_volume_up,					"av.volume_up") \
	X(STR, icon_volume_down,				"av.volume_down") \
	X(STR, icon_volume_mute,				"av.volume_mute") \
	X(STR, icon_volume_off,					"av.volume_off") \
	X(STR, icon_mic,						"av.mic") \
	X(STR, icon_mic_off,					"av.mic_off") \
	X(STR, icon_headset,					"hardware.headset") \
	X(STR, icon_speaker,					"hardware.speaker") \
	X(STR, icon_camera,						"image.camera") \
	X(STR, icon_camera_alt,					"image.camera_alt") \
	X(STR, icon_photo,						"image.photo") \
	X(STR, icon_photo_camera,				"image.photo_camera") \
	X(STR, icon_flash_on,					"image.flash_on") \
	X(STR, icon_flash_off,					"image.flash_off") \
	X(STR, icon_videocam,					"av.videocam") \
	X(STR, icon_videocam_off,				"av.videocam_off") \
	X(STR, icon_phone,						"communication.phone") \
	X(STR, icon_call,						"communication.call") \
	X(STR, icon_call_end,					"communication.call_end") \
	X(STR, icon_contacts,					"communication.contacts") \
	X(STR, icon_chat,						"communication.chat") \
	X(STR, icon_message,					"communication.message") \
	X(STR, icon_email,						"communication.email") \
	X(STR, icon_phone_locked,				"notification.phone_locked") \
	X(STR, icon_phone_missed,				"notification.phone_missed") \
	X(STR, icon_phonelink_ring,				"communication.phonelink_ring") \
	X(STR, icon_add,						"content.add") \
	X(STR, icon_remove,						"content.remove") \
	X(STR, icon_clear,						"content.clear") \
	X(STR, icon_create,						"content.create") \
	X(STR, icon_save,						"content.save") \
	X(STR, icon_send,						"content.send") \
	X(STR, icon_paste,						"content.content_paste") \
	X(STR, icon_sort,						"content.sort") \
	X(STR, icon_filter,						"content.filter_list") \
	X(STR, icon_delete,						"action.delete") \
	X(STR, icon_place,						"maps.place") \
	X(STR, icon_navigation,					"maps.navigation") \
	X(STR, icon_my_location,				"maps.my_location") \
	X(STR, icon_gps_fixed,					"device.gps_fixed") \
	X(STR, icon_gps_searching,				"device.gps_not_fixed") \
	X(STR, icon_gps_location_searching,		"device.location_searching") \
	X(STR, icon_gps_off,					"device.gps_off") \
	X(STR, icon_location_searching,			"device.location_searching") \
	X(STR, icon_location_disabled,			"device.location_disabled") \
	X(STR, icon_brightness_low,				"device.brightness_low") \
	X(STR, icon_brightness_medium,			"device.brightness_medium") \
	X(STR, icon_brightness_high,			"device.brightness_high") \
	X(STR, icon_brightness_auto,			"device.brightness_auto") \
	X(STR, icon_network_cell,				"device.network_cell") \
	X(STR, icon_network_wifi,				"device.network_wifi") \
	X(STR, icon_data_usage,					"device.data_usage") \
	X(STR, icon_nfc,						"device.nfc") \
	X(STR, icon_sim_card,					"hardware.sim_card") \
	X(STR, icon_memory,						"hardware.memory") \
	X(STR, icon_keyboard,					"hardware.keyboard") \
	X(STR, icon_keyboard_up,				"hardware.keyboard_arrow_up") \
	X(STR, icon_keyboard_down,				"hardware.keyboard_arrow_down") \
	X(STR, icon_keyboard_left,				"hardware.keyboard_arrow_left") \
	X(STR, icon_keyboard_right,				"hardware.keyboard_arrow_right") \
	X(STR, icon_smartphone,					"hardware.smartphone") \
	X(STR, icon_checkbox_checked,			"toggle.check_box") \
	X(STR, icon_checkbox_unchecked,			"toggle.check_box_outline_blank") \
	X(STR, icon_radio_checked,				"toggle.radio_button_checked") \
	X(STR, icon_radio_unchecked,			"toggle.radio_button_unchecked") \
	X(STR, icon_toggle_on,					"toggle.toggle_on") \
	X(STR, icon_toggle_off,					"toggle.toggle_off") \
	X(STR, icon_brush,						"image.brush") \
	X(STR, icon_backspace,					"content.backspace")

#define	DECL_STR(member, def)					char	member[STRING_SIZE];
#define	DECL_INT(member, def, min, max)			int		member;
#define	DECL(kind, ...)							DECL_##kind(__VA_ARGS__)
#define	OFFSET_INT(member, def, min, max)		{#member, OFFSETOF(t_conf, member), TYPE_INT},
#define	OFFSET_STR(member, def)					{#member, OFFSETOF(t_conf, member), TYPE_STRING},
#define	OFFSET(kind, ...)						OFFSET_##kind(__VA_ARGS__)

typedef enum
{
	TYPE_STRING,
	TYPE_INT
}	e_conf_type;

typedef struct s_conf {
	CONF_FIELDS(DECL)
}	t_conf;


enum {
    CONF_FIELD_COUNT =
	#define COUNT(...) +1
		0 CONF_FIELDS(COUNT)
	#undef COUNT
};

typedef struct s_offset
{
	const char	*key;
	size_t		offset;
	e_conf_type	type;
}	t_offset;

extern t_offset	conf_offsets[];

typedef struct s_range
{
	int	min;
	int	max;
}	t_range; // range (included)

typedef struct s_conf_elem
{
	const char		*name;
	void			*config_ptr;
	e_conf_type		type;
	t_range			range;
	void			*default_value;
}	t_conf_elem;


int		parse_config(t_conf *config);
t_conf	*get_config();
char	*get_setting(const char *path, const char *key, int	*not_found);
int		set_setting(const char *path, const char *key, const char *value);

#endif
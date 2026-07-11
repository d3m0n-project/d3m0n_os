#ifndef SETTINGS_H
#define SETTINGS_H

#include "types.h"

#define	STRING_SIZE	128

#define CONF_FIELDS(X) \
	X(STR, wallpaper,					"wallpaper",						"default.bmp") \
	X(INT, theme,						"theme",							0, 0, 1) \
	X(INT, time_mode,					"time_mode",						0, 0, 1) \
	X(STR, launcher,					"launcher",							"com.4re5.d3m0n.system.launcher") \
	X(STR, icon_pack,					"icon_pack",						"material-design-icons") \
	X(STR, icon_airplane_mode,			"icon_airplane_mode",				"device.airplanemode_active") \
	X(STR, icon_wifi_lock,				"icon_wifi_lock",					"device.wifi_lock") \
	X(STR, icon_wifi_off,				"icon_wifi_off",					"device.signal_wifi_off") \
	X(STR, icon_wifi_level0,			"icon_wifi_level0",					"device.signal_wifi_0_bar") \
	X(STR, icon_wifi_level1,			"icon_wifi_level1",					"device.signal_wifi_1_bar") \
	X(STR, icon_wifi_level2,			"icon_wifi_level2",					"device.signal_wifi_2_bar") \
	X(STR, icon_wifi_level3,			"icon_wifi_level3",					"device.signal_wifi_3_bar") \
	X(STR, icon_wifi_level4,			"icon_wifi_level4",					"device.signal_wifi_4_bar") \
	X(STR, icon_wifi,					"icon_wifi",						"notification.wifi") \
	X(STR, icon_wifi_lock_level1,		"icon_wifi_lock_level1",			"device.signal_wifi_1_bar_lock") \
	X(STR, icon_wifi_lock_level2,		"icon_wifi_lock_level2",			"device.signal_wifi_2_bar_lock") \
	X(STR, icon_wifi_lock_level3,		"icon_wifi_lock_level3",			"device.signal_wifi_3_bar_lock") \
	X(STR, icon_wifi_lock_level4,		"icon_wifi_lock_level4",			"device.signal_wifi_4_bar_lock") \
	X(STR, icon_battery_20,				"icon_battery_20",					"device.battery_20") \
	X(STR, icon_battery_30,				"icon_battery_30",					"device.battery_30") \
	X(STR, icon_battery_50,				"icon_battery_50",					"device.battery_50") \
	X(STR, icon_battery_60,				"icon_battery_60",					"device.battery_60") \
	X(STR, icon_battery_80,				"icon_battery_80",					"device.battery_80") \
	X(STR, icon_battery_90,				"icon_battery_90",					"device.battery_90") \
	X(STR, icon_battery_100,			"icon_battery_100",					"device.battery_full") \
	X(STR, icon_battery_alert,			"icon_battery_alert",				"device.battery_alert") \
	X(STR, icon_battery_charging_20,	"icon_battery_charging_20",			"device.battery_charging_20") \
	X(STR, icon_battery_charging_30,	"icon_battery_charging_30",			"device.battery_charging_30") \
	X(STR, icon_battery_charging_50,	"icon_battery_charging_50",			"device.battery_charging_50") \
	X(STR, icon_battery_charging_60,	"icon_battery_charging_60",			"device.battery_charging_60") \
	X(STR, icon_battery_charging_80,	"icon_battery_charging_80",			"device.battery_charging_80") \
	X(STR, icon_battery_charging_90,	"icon_battery_charging_90",			"device.battery_charging_90") \
	X(STR, icon_battery_charging_100,	"icon_battery_charging_100",		"device.battery_charging_full") \
	X(STR, icon_battery_standard,		"icon_battery_standard",			"device.battery_std") \
	X(STR, icon_battery_unknown,		"icon_battery_unknown",				"device.battery_unknown") \
	X(STR, icon_bluetooth,				"icon_bluetooth",					"device.bluetooth") \
	X(STR, icon_bluetooth_connected,	"icon_bluetooth_connected",			"device.bluetooth_connected") \
	X(STR, icon_bluetooth_disabled,		"icon_bluetooth_disabled",			"device.device/bluetooth_disabled") \
	X(STR, icon_bluetooth_searching,	"icon_bluetooth_searching",			"device.device/bluetooth_searching") \
	X(STR, icon_cellular_level0,		"icon_cellular_level0",				"device.signal_cellular_0_bar") \
	X(STR, icon_cellular_level1,		"icon_cellular_level1",				"device.signal_cellular_1_bar") \
	X(STR, icon_cellular_level2,		"icon_cellular_level2",				"device.signal_cellular_2_bar") \
	X(STR, icon_cellular_level3,		"icon_cellular_level3",				"device.signal_cellular_3_bar") \
	X(STR, icon_cellular_level4,		"icon_cellular_level4",				"device.signal_cellular_4_bar") \
	X(STR, icon_cellular_no_internet0,	"icon_cellular_no_internet0",		"device.signal_cellular_connected_no_internet_0_bar") \
	X(STR, icon_cellular_no_internet1,	"icon_cellular_no_internet1",		"device.signal_cellular_connected_no_internet_1_bar") \
	X(STR, icon_cellular_no_internet2,	"icon_cellular_no_internet2",		"device.signal_cellular_connected_no_internet_2_bar") \
	X(STR, icon_cellular_no_internet3,	"icon_cellular_no_internet3",		"device.signal_cellular_connected_no_internet_3_bar") \
	X(STR, icon_cellular_no_internet4,	"icon_cellular_no_internet4",		"device.signal_cellular_connected_no_internet_4_bar") \
	X(STR, icon_cellular_off,			"icon_cellular_off",				"device.signal_cellular_off") \
	X(STR, icon_cellular_no_sim,		"icon_cellular_no_sim",				"device.signal_cellular_no_sim") \
	X(STR, icon_cellular_null,			"icon_cellular_null",				"device.signal_cellular_null") \
	X(STR, icon_cellular_alt,			"icon_cellular_alt",				"device.signal_cellular_alt") \
	X(STR, icon_sd_storage,				"icon_sd_storage",					"device.sd_storage") \
	X(STR, icon_settings,				"icon_settings",					"action.settings") \
	X(STR, icon_search,					"icon_search",						"action.search") \
	X(STR, icon_home,					"icon_home",						"action.home") \
	X(STR, icon_lock,					"icon_lock",						"action.lock") \
	X(STR, icon_lock_open,				"icon_lock_open",					"action.lock_open") \
	X(STR, icon_power,					"icon_power",						"action.power_settings_new") \
	X(STR, icon_backup,					"icon_backup",						"action.backup") \
	X(STR, icon_restore,				"icon_restore",						"action.restore") \
	X(STR, icon_sync,					"icon_sync",						"notification.sync") \
	X(STR, icon_sync_disabled,			"icon_sync_disabled",				"notification.sync_disabled") \
	X(STR, icon_cached,					"icon_cached",						"action.cached") \
	X(STR, icon_account,				"icon_account",						"action.account_circle") \
	X(STR, icon_visibility,				"icon_visibility",					"action.visibility") \
	X(STR, icon_visibility_off,			"icon_visibility_off",				"action.visibility_off") \
	X(STR, icon_favorite,				"icon_favorite",					"action.favorite") \
	X(STR, icon_favorite_border,		"icon_favorite_border",				"action.favorite_border") \
	X(STR, icon_error,					"icon_error",						"alert.error") \
	X(STR, icon_warning,				"icon_warning",						"alert.warning") \
	X(STR, icon_notification,			"icon_notification",				"alert.notification_important") \
	X(STR, icon_info,					"icon_info",						"action.info") \
	X(STR, icon_help,					"icon_help",						"action.help") \
	X(STR, icon_bug_report,				"icon_bug_report",					"action.bug_report") \
	X(STR, icon_alarm,					"icon_alarm",						"action.alarm") \
	X(STR, icon_alarm_on,				"icon_alarm_on",					"action.alarm_on") \
	X(STR, icon_alarm_off,				"icon_alarm_off",					"action.alarm_off") \
	X(STR, icon_schedule,				"icon_schedule",					"action.schedule") \
	X(STR, icon_today,					"icon_today",						"action.today") \
	X(STR, icon_event,					"icon_event",						"action.event") \
	X(STR, icon_history,				"icon_history",						"action.history") \
	X(STR, icon_play,					"icon_play",						"av.play_arrow") \
	X(STR, icon_pause,					"icon_pause",						"av.pause") \
	X(STR, icon_stop,					"icon_stop",						"av.stop") \
	X(STR, icon_next,					"icon_next",						"av.skip_next") \
	X(STR, icon_previous,				"icon_previous",					"av.skip_previous") \
	X(STR, icon_fast_forward,			"icon_fast_forward",				"av.fast_forward") \
	X(STR, icon_fast_rewind,			"icon_fast_rewind",					"av.fast_rewind") \
	X(STR, icon_volume_up,				"icon_volume_up",					"av.volume_up") \
	X(STR, icon_volume_down,			"icon_volume_down",					"av.volume_down") \
	X(STR, icon_volume_mute,			"icon_volume_mute",					"av.volume_mute") \
	X(STR, icon_volume_off,				"icon_volume_off",					"av.volume_off") \
	X(STR, icon_mic,					"icon_mic",							"av.mic") \
	X(STR, icon_mic_off,				"icon_mic_off",						"av.mic_off") \
	X(STR, icon_headset,				"icon_headset",						"hardware.headset") \
	X(STR, icon_speaker,				"icon_speaker",						"hardware.speaker") \
	X(STR, icon_camera,					"icon_camera",						"image.camera") \
	X(STR, icon_camera_alt,				"icon_camera_alt",					"image.camera_alt") \
	X(STR, icon_photo,					"icon_photo",						"image.photo") \
	X(STR, icon_photo_camera,			"icon_photo_camera",				"image.photo_camera") \
	X(STR, icon_flash_on,				"icon_flash_on",					"image.flash_on") \
	X(STR, icon_flash_off,				"icon_flash_off",					"image.flash_off") \
	X(STR, icon_videocam,				"icon_videocam",					"av.videocam") \
	X(STR, icon_videocam_off,			"icon_videocam_off",				"av.videocam_off") \
	X(STR, icon_phone,					"icon_phone",						"communication.phone") \
	X(STR, icon_call,					"icon_call",						"communication.call") \
	X(STR, icon_call_end,				"icon_call_end",					"communication.call_end") \
	X(STR, icon_contacts,				"icon_contacts",					"communication.contacts") \
	X(STR, icon_chat,					"icon_chat",						"communication.chat") \
	X(STR, icon_message,				"icon_message",						"communication.message") \
	X(STR, icon_email,					"icon_email",						"communication.email") \
	X(STR, icon_phone_locked,			"icon_phone_locked",				"notification.phone_locked") \
	X(STR, icon_phone_missed,			"icon_phone_missed",				"notification.phone_missed") \
	X(STR, icon_phonelink_ring,			"icon_phonelink_ring",				"communication.phonelink_ring") \
	X(STR, icon_add,					"icon_add",							"content.add") \
	X(STR, icon_remove,					"icon_remove",						"content.remove") \
	X(STR, icon_clear,					"icon_clear",						"content.clear") \
	X(STR, icon_create,					"icon_create",						"content.create") \
	X(STR, icon_save,					"icon_save",						"content.save") \
	X(STR, icon_send,					"icon_send",						"content.send") \
	X(STR, icon_paste,					"icon_paste",						"content.content_paste") \
	X(STR, icon_sort,					"icon_sort",						"content.sort") \
	X(STR, icon_filter,					"icon_filter",						"content.filter_list") \
	X(STR, icon_delete,					"icon_delete",						"action.delete") \
	X(STR, icon_place,					"icon_place",						"maps.place") \
	X(STR, icon_navigation,				"icon_navigation",					"maps.navigation") \
	X(STR, icon_my_location,			"icon_my_location",					"maps.my_location") \
	X(STR, icon_gps_fixed,				"icon_gps_fixed",					"device.gps_fixed") \
	X(STR, icon_gps_searching,			"icon_gps_searching",				"device.gps_not_fixed") \
	X(STR, icon_gps_location_searching,	"icon_gps_location_searching",		"device.location_searching") \
	X(STR, icon_gps_off,				"icon_gps_off",						"device.gps_off") \
	X(STR, icon_location_searching,		"icon_location_searching",			"device.location_searching") \
	X(STR, icon_location_disabled,		"icon_location_disabled",			"device.location_disabled") \
	X(STR, icon_brightness_low,			"icon_brightness_low",				"device.brightness_low") \
	X(STR, icon_brightness_medium,		"icon_brightness_medium",			"device.brightness_medium") \
	X(STR, icon_brightness_high,		"icon_brightness_high",				"device.brightness_high") \
	X(STR, icon_brightness_auto,		"icon_brightness_auto",				"device.brightness_auto") \
	X(STR, icon_network_cell,			"icon_network_cell",				"device.network_cell") \
	X(STR, icon_network_wifi,			"icon_network_wifi",				"device.network_wifi") \
	X(STR, icon_data_usage,				"icon_data_usage",					"device.data_usage") \
	X(STR, icon_nfc,					"icon_nfc",							"device.nfc") \
	X(STR, icon_sim_card,				"icon_sim_card",					"hardware.sim_card") \
	X(STR, icon_memory,					"icon_memory",						"hardware.memory") \
	X(STR, icon_keyboard,				"icon_keyboard",					"hardware.keyboard") \
	X(STR, icon_keyboard_up,			"icon_keyboard_up",					"hardware.keyboard_arrow_up") \
	X(STR, icon_keyboard_down,			"icon_keyboard_down",				"hardware.keyboard_arrow_down") \
	X(STR, icon_keyboard_left,			"icon_keyboard_left",				"hardware.keyboard_arrow_left") \
	X(STR, icon_keyboard_right,			"icon_keyboard_right",				"hardware.keyboard_arrow_right") \
	X(STR, icon_smartphone,				"icon_smartphone",					"hardware.smartphone") \
	X(STR, icon_checkbox_checked,		"icon_checkbox_checked",			"toggle.check_box") \
	X(STR, icon_checkbox_unchecked,		"icon_checkbox_unchecked",			"toggle.check_box_outline_blank") \
	X(STR, icon_radio_checked,			"icon_radio_checked",				"toggle.radio_button_checked") \
	X(STR, icon_radio_unchecked,		"icon_radio_unchecked",				"toggle.radio_button_unchecked") \
	X(STR, icon_toggle_on,				"icon_toggle_on",					"toggle.toggle_on") \
	X(STR, icon_toggle_off,				"icon_toggle_off",					"toggle.toggle_off")

#define	DECL_STR(member, key, def)				char member[STRING_SIZE];
#define	DECL_INT(member, key, def, min, max)	int member;
#define	DECL(kind, ...)							DECL_##kind(__VA_ARGS__)
#define	OFFSET_INT(member, key, def, min, max)	{key, OFFSETOF(t_conf, member), TYPE_INT},
#define	OFFSET_STR(member, key, def)			{key, OFFSETOF(t_conf, member), TYPE_STRING},
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
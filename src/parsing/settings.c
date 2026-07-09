#include "parsing.h"
#include "memory.h"
#include "get_next_line.h"
#include "libft.h"
#include "settings.h"

typedef enum
{
	TYPE_STRING,
	TYPE_INT
}	e_conf_type;

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

int		parse_config(t_conf *config)
{
	t_conf_elem	CONFIG_SCHEMA[] = {
		{"wallpaper", &config->wallpaper, TYPE_STRING, {0}, "default.bmp"},
		{"theme", &config->theme, TYPE_INT, {.min=0, .max=1}, &(int){0}},
		{"splash_time", &config->splash_time, TYPE_INT, {.min=0, .max=5000}, &(int){3000}},
		{"time_mode", &config->time_mode, TYPE_INT, {.min=0, .max=1}, &(int){0}},
		{"launcher", &config->launcher, TYPE_STRING, {0}, "com.4re5.d3m0n.system.launcher"},

		// icons
		{"icon_pack", &config->icon_pack, TYPE_STRING, {0}, "material-design-icons"},
		{"icon_airplane_mode", &config->icon_airplane_mode, TYPE_STRING, {0}, "device.airplanemode_active"},
		{"icon_wifi_lock", &config->icon_wifi_lock, TYPE_STRING, {0}, "device.wifi_lock"},
		{"icon_wifi_off", &config->icon_wifi_off, TYPE_STRING, {0}, "device.signal_wifi_off"},
		{"icon_wifi_level0", &config->icon_wifi_level0, TYPE_STRING, {0}, "device.signal_wifi_0_bar"},
		{"icon_wifi_level1", &config->icon_wifi_level1, TYPE_STRING, {0}, "device.signal_wifi_1_bar"},
		{"icon_wifi_level2", &config->icon_wifi_level2, TYPE_STRING, {0}, "device.signal_wifi_2_bar"},
		{"icon_wifi_level3", &config->icon_wifi_level3, TYPE_STRING, {0}, "device.signal_wifi_3_bar"},
		{"icon_wifi_level4", &config->icon_wifi_level4, TYPE_STRING, {0}, "device.signal_wifi_4_bar"},
		{"icon_wifi_lock_level1", &config->icon_wifi_lock_level1, TYPE_STRING, {0}, "device.signal_wifi_1_bar_lock"},
		{"icon_wifi_lock_level2", &config->icon_wifi_lock_level2, TYPE_STRING, {0}, "device.signal_wifi_2_bar_lock"},
		{"icon_wifi_lock_level3", &config->icon_wifi_lock_level3, TYPE_STRING, {0}, "device.signal_wifi_3_bar_lock"},
		{"icon_wifi_lock_level4", &config->icon_wifi_lock_level4, TYPE_STRING, {0}, "device.signal_wifi_4_bar_lock"},
		{"icon_battery_20", &config->icon_battery_20, TYPE_STRING, {0}, "device.battery_20"},
		{"icon_battery_30", &config->icon_battery_30, TYPE_STRING, {0}, "device.battery_30"},
		{"icon_battery_50", &config->icon_battery_50, TYPE_STRING, {0}, "device.battery_50"},
		{"icon_battery_60", &config->icon_battery_60, TYPE_STRING, {0}, "device.battery_60"},
		{"icon_battery_80", &config->icon_battery_80, TYPE_STRING, {0}, "device.battery_80"},
		{"icon_battery_90", &config->icon_battery_90, TYPE_STRING, {0}, "device.battery_90"},
		{"icon_battery_100", &config->icon_battery_100, TYPE_STRING, {0}, "device.battery_full"},
		{"icon_battery_alert", &config->icon_battery_alert, TYPE_STRING, {0}, "device.battery_alert"},
		{"icon_battery_charging_20", &config->icon_battery_charging_20, TYPE_STRING, {0}, "device.battery_charging_20"},
		{"icon_battery_charging_30", &config->icon_battery_charging_30, TYPE_STRING, {0}, "device.battery_charging_30"},
		{"icon_battery_charging_50", &config->icon_battery_charging_50, TYPE_STRING, {0}, "device.battery_charging_50"},
		{"icon_battery_charging_60", &config->icon_battery_charging_60, TYPE_STRING, {0}, "device.battery_charging_60"},
		{"icon_battery_charging_80", &config->icon_battery_charging_80, TYPE_STRING, {0}, "device.battery_charging_80"},
		{"icon_battery_charging_90", &config->icon_battery_charging_90, TYPE_STRING, {0}, "device.battery_charging_90"},
		{"icon_battery_charging_100", &config->icon_battery_charging_100, TYPE_STRING, {0}, "device.battery_charging_full"},
		{"icon_battery_standard", &config->icon_battery_standard, TYPE_STRING, {0}, "device.battery_std"},
		{"icon_battery_unknown", &config->icon_battery_unknown, TYPE_STRING, {0}, "device.battery_unknown"},
		{"icon_bluetooth", &config->icon_bluetooth, TYPE_STRING, {0}, "device.bluetooth"},
		{"icon_bluetooth_connected", &config->icon_bluetooth_connected, TYPE_STRING, {0}, "device.bluetooth_connected"},
		{"icon_bluetooth_disabled", &config->icon_bluetooth_disabled, TYPE_STRING, {0}, "device.device/bluetooth_disabled"},
		{"icon_bluetooth_searching", &config->icon_bluetooth_searching, TYPE_STRING, {0}, "device.device/bluetooth_searching"},
		{"icon_cellular_level0", &config->icon_cellular_level0, TYPE_STRING, {0}, "device.signal_cellular_0_bar"},
		{"icon_cellular_level1", &config->icon_cellular_level1, TYPE_STRING, {0}, "device.signal_cellular_1_bar"},
		{"icon_cellular_level2", &config->icon_cellular_level2, TYPE_STRING, {0}, "device.signal_cellular_2_bar"},
		{"icon_cellular_level3", &config->icon_cellular_level3, TYPE_STRING, {0}, "device.signal_cellular_3_bar"},
		{"icon_cellular_level4", &config->icon_cellular_level4, TYPE_STRING, {0}, "device.signal_cellular_4_bar"},
		{"icon_cellular_no_internet0", &config->icon_cellular_no_internet0, TYPE_STRING, {0}, "device.signal_cellular_connected_no_internet_0_bar"},
		{"icon_cellular_no_internet1", &config->icon_cellular_no_internet1, TYPE_STRING, {0}, "device.signal_cellular_connected_no_internet_1_bar"},
		{"icon_cellular_no_internet2", &config->icon_cellular_no_internet2, TYPE_STRING, {0}, "device.signal_cellular_connected_no_internet_2_bar"},
		{"icon_cellular_no_internet3", &config->icon_cellular_no_internet3, TYPE_STRING, {0}, "device.signal_cellular_connected_no_internet_3_bar"},
		{"icon_cellular_no_internet4", &config->icon_cellular_no_internet4, TYPE_STRING, {0}, "device.signal_cellular_connected_no_internet_4_bar"},
		{"icon_cellular_off", &config->icon_cellular_off, TYPE_STRING, {0}, "device.signal_cellular_off"},
		{"icon_cellular_no_sim", &config->icon_cellular_no_sim, TYPE_STRING, {0}, "device.signal_cellular_no_sim"},
		{"icon_cellular_null", &config->icon_cellular_null, TYPE_STRING, {0}, "device.signal_cellular_null"},
		{"icon_cellular_alt", &config->icon_cellular_alt, TYPE_STRING, {0}, "device.signal_cellular_alt"},
		{"icon_sd_storage", &config->icon_sd_storage, TYPE_STRING, {0}, "device.sd_storage"},
		{"icon_settings", &config->icon_settings, TYPE_STRING, {0}, "action.settings"},
		{"icon_search", &config->icon_search, TYPE_STRING, {0}, "action.search"},
		{"icon_home", &config->icon_home, TYPE_STRING, {0}, "action.home"},
		{"icon_lock", &config->icon_lock, TYPE_STRING, {0}, "action.lock"},
		{"icon_lock_open", &config->icon_lock_open, TYPE_STRING, {0}, "action.lock_open"},
		{"icon_power", &config->icon_power, TYPE_STRING, {0}, "action.power_settings_new"},
		{"icon_backup", &config->icon_backup, TYPE_STRING, {0}, "action.backup"},
		{"icon_restore", &config->icon_restore, TYPE_STRING, {0}, "action.restore"},
		{"icon_sync", &config->icon_sync, TYPE_STRING, {0}, "notification.sync"},
		{"icon_sync_disabled", &config->icon_sync_disabled, TYPE_STRING, {0}, "notification.sync_disabled"},
		{"icon_cached", &config->icon_cached, TYPE_STRING, {0}, "action.cached"},
		{"icon_account", &config->icon_account, TYPE_STRING, {0}, "action.account_circle"},
		{"icon_visibility", &config->icon_visibility, TYPE_STRING, {0}, "action.visibility"},
		{"icon_visibility_off", &config->icon_visibility_off, TYPE_STRING, {0}, "action.visibility_off"},
		{"icon_favorite", &config->icon_favorite, TYPE_STRING, {0}, "action.favorite"},
		{"icon_favorite_border", &config->icon_favorite_border, TYPE_STRING, {0}, "action.favorite_border"},
		{"icon_error", &config->icon_error, TYPE_STRING, {0}, "alert.error"},
		{"icon_warning", &config->icon_warning, TYPE_STRING, {0}, "alert.warning"},
		{"icon_notification", &config->icon_notification, TYPE_STRING, {0}, "alert.notification_important"},
		{"icon_info", &config->icon_info, TYPE_STRING, {0}, "action.info"},
		{"icon_help", &config->icon_help, TYPE_STRING, {0}, "action.help"},
		{"icon_bug_report", &config->icon_bug_report, TYPE_STRING, {0}, "action.bug_report"},
		{"icon_alarm", &config->icon_alarm, TYPE_STRING, {0}, "action.alarm"},
		{"icon_alarm_on", &config->icon_alarm_on, TYPE_STRING, {0}, "action.alarm_on"},
		{"icon_alarm_off", &config->icon_alarm_off, TYPE_STRING, {0}, "action.alarm_off"},
		{"icon_schedule", &config->icon_schedule, TYPE_STRING, {0}, "action.schedule"},
		{"icon_today", &config->icon_today, TYPE_STRING, {0}, "action.today"},
		{"icon_event", &config->icon_event, TYPE_STRING, {0}, "action.event"},
		{"icon_history", &config->icon_history, TYPE_STRING, {0}, "action.history"},
		{"icon_play", &config->icon_play, TYPE_STRING, {0}, "av.play_arrow"},
		{"icon_pause", &config->icon_pause, TYPE_STRING, {0}, "av.pause"},
		{"icon_stop", &config->icon_stop, TYPE_STRING, {0}, "av.stop"},
		{"icon_next", &config->icon_next, TYPE_STRING, {0}, "av.skip_next"},
		{"icon_previous", &config->icon_previous, TYPE_STRING, {0}, "av.skip_previous"},
		{"icon_fast_forward", &config->icon_fast_forward, TYPE_STRING, {0}, "av.fast_forward"},
		{"icon_fast_rewind", &config->icon_fast_rewind, TYPE_STRING, {0}, "av.fast_rewind"},
		{"icon_volume_up", &config->icon_volume_up, TYPE_STRING, {0}, "av.volume_up"},
		{"icon_volume_down", &config->icon_volume_down, TYPE_STRING, {0}, "av.volume_down"},
		{"icon_volume_mute", &config->icon_volume_mute, TYPE_STRING, {0}, "av.volume_mute"},
		{"icon_volume_off", &config->icon_volume_off, TYPE_STRING, {0}, "av.volume_off"},
		{"icon_mic", &config->icon_mic, TYPE_STRING, {0}, "av.mic"},
		{"icon_mic_off", &config->icon_mic_off, TYPE_STRING, {0}, "av.mic_off"},
		{"icon_headset", &config->icon_headset, TYPE_STRING, {0}, "hardware.headset"},
		{"icon_speaker", &config->icon_speaker, TYPE_STRING, {0}, "hardware.speaker"},
		{"icon_camera", &config->icon_camera, TYPE_STRING, {0}, "image.camera"},
		{"icon_camera_alt", &config->icon_camera_alt, TYPE_STRING, {0}, "image.camera_alt"},
		{"icon_photo", &config->icon_photo, TYPE_STRING, {0}, "image.photo"},
		{"icon_photo_camera", &config->icon_photo_camera, TYPE_STRING, {0}, "image.photo_camera"},
		{"icon_flash_on", &config->icon_flash_on, TYPE_STRING, {0}, "image.flash_on"},
		{"icon_flash_off", &config->icon_flash_off, TYPE_STRING, {0}, "image.flash_off"},
		{"icon_videocam", &config->icon_videocam, TYPE_STRING, {0}, "av.videocam"},
		{"icon_videocam_off", &config->icon_videocam_off, TYPE_STRING, {0}, "av.videocam_off"},
		{"icon_phone", &config->icon_phone, TYPE_STRING, {0}, "communication.phone"},
		{"icon_call", &config->icon_call, TYPE_STRING, {0}, "communication.call"},
		{"icon_call_end", &config->icon_call_end, TYPE_STRING, {0}, "communication.call_end"},
		{"icon_contacts", &config->icon_contacts, TYPE_STRING, {0}, "communication.contacts"},
		{"icon_chat", &config->icon_chat, TYPE_STRING, {0}, "communication.chat"},
		{"icon_message", &config->icon_message, TYPE_STRING, {0}, "communication.message"},
		{"icon_email", &config->icon_email, TYPE_STRING, {0}, "communication.email"},
		{"icon_phone_locked", &config->icon_phone_locked, TYPE_STRING, {0}, "notification.phone_locked"},
		{"icon_phone_missed", &config->icon_phone_missed, TYPE_STRING, {0}, "notification.phone_missed"},
		{"icon_phonelink_ring", &config->icon_phonelink_ring, TYPE_STRING, {0}, "communication.phonelink_ring"},
		{"icon_add", &config->icon_add, TYPE_STRING, {0}, "content.add"},
		{"icon_remove", &config->icon_remove, TYPE_STRING, {0}, "content.remove"},
		{"icon_clear", &config->icon_clear, TYPE_STRING, {0}, "content.clear"},
		{"icon_create", &config->icon_create, TYPE_STRING, {0}, "content.create"},
		{"icon_save", &config->icon_save, TYPE_STRING, {0}, "content.save"},
		{"icon_send", &config->icon_send, TYPE_STRING, {0}, "content.send"},
		{"icon_paste", &config->icon_paste, TYPE_STRING, {0}, "content.content_paste"},
		{"icon_sort", &config->icon_sort, TYPE_STRING, {0}, "content.sort"},
		{"icon_filter", &config->icon_filter, TYPE_STRING, {0}, "content.filter_list"},
		{"icon_delete", &config->icon_delete, TYPE_STRING, {0}, "action.delete"},
		{"icon_place", &config->icon_place, TYPE_STRING, {0}, "maps.place"},
		{"icon_navigation", &config->icon_navigation, TYPE_STRING, {0}, "maps.navigation"},
		{"icon_my_location", &config->icon_my_location, TYPE_STRING, {0}, "maps.my_location"},
		{"icon_gps_fixed", &config->icon_gps_fixed, TYPE_STRING, {0}, "device.gps_fixed"},
		{"icon_gps_searching", &config->icon_gps_searching, TYPE_STRING, {0}, "device.gps_not_fixed"},
		{"icon_gps_location_searching", &config->icon_gps_location_searching, TYPE_STRING, {0}, "device.location_searching"},
		{"icon_gps_off", &config->icon_gps_off, TYPE_STRING, {0}, "device.gps_off"},
		{"icon_location_searching", &config->icon_location_searching, TYPE_STRING, {0}, "device.location_searching"},
		{"icon_location_disabled", &config->icon_location_disabled, TYPE_STRING, {0}, "device.location_disabled"},
		{"icon_brightness_low", &config->icon_brightness_low, TYPE_STRING, {0}, "device.brightness_low"},
		{"icon_brightness_medium", &config->icon_brightness_medium, TYPE_STRING, {0}, "device.brightness_medium"},
		{"icon_brightness_high", &config->icon_brightness_high, TYPE_STRING, {0}, "device.brightness_high"},
		{"icon_brightness_auto", &config->icon_brightness_auto, TYPE_STRING, {0}, "device.brightness_auto"},
		{"icon_network_cell", &config->icon_network_cell, TYPE_STRING, {0}, "device.network_cell"},
		{"icon_network_wifi", &config->icon_network_wifi, TYPE_STRING, {0}, "device.network_wifi"},
		{"icon_data_usage", &config->icon_data_usage, TYPE_STRING, {0}, "device.data_usage"},
		{"icon_nfc", &config->icon_nfc, TYPE_STRING, {0}, "device.nfc"},
		{"icon_sim_card", &config->icon_sim_card, TYPE_STRING, {0}, "hardware.sim_card"},
		{"icon_memory", &config->icon_memory, TYPE_STRING, {0}, "hardware.memory"},
		{"icon_keyboard", &config->icon_keyboard, TYPE_STRING, {0}, "hardware.keyboard"},
		{"icon_keyboard_up", &config->icon_keyboard_up, TYPE_STRING, {0}, "hardware.keyboard_arrow_up"},
		{"icon_keyboard_down", &config->icon_keyboard_down, TYPE_STRING, {0}, "hardware.keyboard_arrow_down"},
		{"icon_keyboard_left", &config->icon_keyboard_left, TYPE_STRING, {0}, "hardware.keyboard_arrow_left"},
		{"icon_keyboard_right", &config->icon_keyboard_right, TYPE_STRING, {0}, "hardware.keyboard_arrow_right"},
		{"icon_smartphone", &config->icon_smartphone, TYPE_STRING, {0}, "hardware.smartphone"},
		{"icon_checkbox_checked", &config->icon_checkbox_checked, TYPE_STRING, {0}, "toggle.check_box"},
		{"icon_checkbox_unchecked", &config->icon_checkbox_unchecked, TYPE_STRING, {0}, "toggle.check_box_outline_blank"},
		{"icon_radio_checked", &config->icon_radio_checked, TYPE_STRING, {0}, "toggle.radio_button_checked"},
		{"icon_radio_unchecked", &config->icon_radio_unchecked, TYPE_STRING, {0}, "toggle.radio_button_unchecked"},
		{"icon_toggle_on", &config->icon_toggle_on, TYPE_STRING, {0}, "toggle.toggle_on"},
		{"icon_toggle_off", &config->icon_toggle_off, TYPE_STRING, {0}, "toggle.toggle_off"},
		{0}
	};
	if (!config)
		return 1;
	
	t_conf_elem	*current = 0;
	int			i = 0;
	while ((current = &CONFIG_SCHEMA[i])->name)
	{
		// parse defaults
		int not_found = 0;
		char *value = get_setting("/config", current->name, &not_found);
		if (!value && !not_found) // error
			return 1;
		else if (not_found) // not found, use default
		{
			if (current->type == TYPE_STRING)
				ft_strlcpy((char *)current->config_ptr, current->default_value, ft_strlen(current->default_value) + 1);
			else if (current->type == TYPE_INT)
				*(int *)current->config_ptr = *(int *)current->default_value;
			i++;
			continue;
		}
		if (current->type == TYPE_STRING)
		{
			size_t	len = ft_strlen(value);
			if (len > STRING_SIZE - 1)
			{
				log("CONFIG: Invalid config entry, max string length: %i\n", LOG_ERROR, STRING_SIZE);
				free(value);
				return 1;
			}
			ft_strlcpy((char *)current->config_ptr, value, len + 1);
		}
		else if (current->type == TYPE_INT)
		{
			int v = ft_atoi(value);
			if (current->range.min <= v && v <= current->range.max)
				*(int *)current->config_ptr = v;
			else
			{
				log("CONFIG: Integer not in allowed range [%i, %i], got %i\n", LOG_ERROR, current->range.min, current->range.max, v);
				free(value);
				return 1;
			}
		}
		free(value);
		i++;
	}
	return 0;
}

char	*get_setting(const char *path, const char *key, int	*not_found)
{
	char	*line;
	int		fd;

	*not_found = 0;
	fd = open(path, O_READ);
	if (fd < 0)
		return 0;
	while ((line = get_next_line(fd)))
	{
		size_t	i;
		size_t	value_len;
		char	*output;

		if (line[0] == '#' || line[0] == '\r' || line[0] == '\n' || line[0] == ' ')
		{
			free(line);
			continue;
		}

		i = 0;
		while (line[i] && line[i] != ':')
			i++;

		if (line[i] != ':')
		{
			free(line);
			close(fd);
			return 0;
		}

		if (ft_strlen(key) == i && !ft_strncmp(line, key, i))
		{
			i++; // skip

			while (line[i] == ' ' || line[i] == '\t')
				i++;

			if (!line[i])
			{
				free(line);
				close(fd);
				return 0;
			}

			value_len = 0;
			while (line[i + value_len] && line[i + value_len] != '\n' && line[i + value_len] != '\r')
				value_len++;

			while (value_len > 0 && (line[i + value_len - 1] == ' ' || line[i + value_len - 1] == '\t'))
				value_len--;

			if (value_len == 0)
			{
				free(line);
				close(fd);
				*not_found = 1;
				return 0;
			}

			output = ft_calloc(value_len + 1, sizeof(char));
			if (!output)
			{
				free(line);
				close(fd);
				return 0;
			}

			ft_memcpy(output, line + i, value_len);
			output[value_len] = '\0';

			free(line);
			close(fd);
			return output;
		}
		free(line);
	}
	close(fd);
	return 0;
}

int	set_setting(const char *path, const char *key, const char *value)
{
	char	*line;
	int		fd;
	int		found = 0;
	char	**lines = NULL;
	int		line_count = 0;
	int		i;

	if (!path || !key || !value)
		return 1;

	fd = open(path, O_READ);
	if (fd < 0)
		return 1;

	while ((line = get_next_line(fd)))
	{
		line_count++;
		free(line);
	}
	close(fd);

	lines = (char **)ft_calloc(line_count + 2, sizeof(char *));
	if (!lines)
		return 1;

	fd = open(path, O_READ);
	if (fd < 0)
	{
		free(lines);
		return 1;
	}

	i = 0;
	while ((line = get_next_line(fd)) && i < line_count)
	{
		size_t key_len = 0;
		while (line[key_len] && line[key_len] != ':')
			key_len++;

		if (line[key_len] == ':' && ft_strlen(key) == key_len && !ft_strncmp(line, key, key_len))
		{
			size_t new_len = ft_strlen(key) + 2 + ft_strlen(value);
			char *new_line = (char *)ft_calloc(new_len + 1, sizeof(char));
			if (!new_line)
			{
				free(line);
				goto cleanup;
			}
			ft_strlcpy(new_line, key, new_len + 1);
			ft_strlcat(new_line, ": ", new_len + 1);
			ft_strlcat(new_line, value, new_len + 1);
			lines[i] = new_line;
			found = 1;
			free(line);
		}
		else
			lines[i] = line;
		i++;
	}
	close(fd);

	if (!found)
		goto cleanup;

	// Write all lines back to file
	fd = open(path, O_WRITE | O_TRUNC);
	if (fd < 0)
		goto cleanup;

	i = 0;
	while (lines[i])
	{
		size_t len = ft_strlen(lines[i]);
		if (write(fd, lines[i], len) != (int)len)
		{
			close(fd);
			goto cleanup;
		}
		// write newline if not already present
		if (len == 0 || (lines[i][len - 1] != '\n'))
		{
			if (write(fd, "\n", 1) != 1)
			{
				close(fd);
				goto cleanup;
			}
		}
		i++;
	}
	close(fd);

	// cleanup
	i = 0;
	while (lines[i])
	{
		free(lines[i]);
		i++;
	}
	free(lines);
	return 0;

cleanup:
	i = 0;
	while (lines[i])
	{
		free(lines[i]);
		i++;
	}
	free(lines);
	return 1;
}
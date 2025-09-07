/*
 * This file is part of the sigrok-cli project.
 *
 * Copyright (C) 2013 Bert Vermeulen <bert@biot.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <stdlib.h>
#include <glib.h>
#include "sigrok-cli.h"

struct sr_context *sr_ctx = NULL;
#ifdef HAVE_SRD
struct srd_session *srd_sess = NULL;
#endif

static void logger(const gchar *log_domain, GLogLevelFlags log_level,
		   const gchar *message, gpointer cb_data)
{
	(void)log_domain;
	(void)cb_data;

	/*
	 * All messages, warnings, errors etc. go to stderr (not stdout) in
	 * order to not mess up the CLI tool data output, e.g. VCD output.
	 */
	if (log_level & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING)
			|| opt_loglevel > SR_LOG_WARN) {
		fprintf(stderr, "%s\n", message);
		fflush(stderr);
	}

	if (log_level & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL))
		exit(1);

}

int select_channels(struct sr_dev_inst *sdi)
{
	struct sr_channel *ch;
	gboolean enabled;
	GSList *selected_channels, *l, *channels;

	channels = sr_dev_inst_channels_get(sdi);

	if (opt_channels) {
		if (!(selected_channels = parse_channelstring(sdi, opt_channels)))
			return SR_ERR;

		for (l = channels; l; l = l->next) {
			ch = l->data;
			enabled = (g_slist_find(selected_channels, ch) != NULL);
			if (sr_dev_channel_enable(ch, enabled) != SR_OK)
				return SR_ERR;
		}
		g_slist_free(selected_channels);
	}
#ifdef HAVE_SRD
	map_pd_channels(sdi);
#endif
	return SR_OK;
}

int maybe_config_get(struct sr_dev_driver *driver,
		const struct sr_dev_inst *sdi, struct sr_channel_group *cg,
		uint32_t key, GVariant **gvar)
{
	if (sr_dev_config_capabilities_list(sdi, cg, key) & SR_CONF_GET)
		return sr_config_get(driver, sdi, cg, key, gvar);

	return SR_ERR_NA;
}

int maybe_config_set(struct sr_dev_driver *driver,
		const struct sr_dev_inst *sdi, struct sr_channel_group *cg,
		uint32_t key, GVariant *gvar)
{
	(void)driver;

	if (sr_dev_config_capabilities_list(sdi, cg, key) & SR_CONF_SET)
		return sr_config_set(sdi, cg, key, gvar);

	return SR_ERR_NA;
}

int maybe_config_list(struct sr_dev_driver *driver,
		const struct sr_dev_inst *sdi, struct sr_channel_group *cg,
		uint32_t key, GVariant **gvar)
{
	if (sr_dev_config_capabilities_list(sdi, cg, key) & SR_CONF_LIST)
		return sr_config_list(driver, sdi, cg, key, gvar);

	return SR_ERR_NA;
}

static void get_option(struct sr_dev_inst *sdi,
	struct sr_channel_group *cg, const char *opt)
{
	struct sr_dev_driver *driver;
	const struct sr_key_info *ci;
	int ret;
	GVariant *gvar;
	const struct sr_key_info *srci, *srmqi, *srmqfi;
	uint32_t mq;
	uint64_t mask, mqflags;
	unsigned int j;
	char *s;

	driver = sr_dev_inst_driver_get(sdi);

	ci = sr_key_info_name_get(SR_KEY_CONFIG, opt);
	if (!ci)
		g_critical("Unknown option '%s'", opt);

	ret = maybe_config_get(driver, sdi, cg, ci->key, &gvar);
	if (ret != SR_OK)
		g_critical("Failed to get '%s': %s", opt, sr_strerror(ret));

	srci = sr_key_info_get(SR_KEY_CONFIG, ci->key);
	if (srci && srci->datatype == SR_T_MQ) {
		g_variant_get(gvar, "(ut)", &mq, &mqflags);
		if ((srmqi = sr_key_info_get(SR_KEY_MQ, mq)))
			printf("%s", srmqi->id);
		else
			printf("%d", mq);
		for (j = 0, mask = 1; j < 32; j++, mask <<= 1) {
			if (!(mqflags & mask))
				continue;
			if ((srmqfi = sr_key_info_get(SR_KEY_MQFLAGS, mqflags & mask)))
				printf("/%s", srmqfi->id);
			else
				printf("/%" PRIu64, mqflags & mask);
		}
		printf("\n");
	} else {
		s = g_variant_print(gvar, FALSE);
		printf("%s\n", s);
		g_free(s);
	}

	g_variant_unref(gvar);
}

static void get_options(void)
{
	GSList *devices;
	struct sr_dev_inst *sdi;
	size_t get_idx;
	char *get_text, *cg_name;
	GHashTable *args;
	GHashTableIter iter;
	gpointer key, value;
	struct sr_channel_group *cg;

	/* Lookup and open the device. */
	devices = device_scan();
	if (!devices) {
		g_critical("No devices found.");
		return;
	}
	sdi = devices->data;
	g_slist_free(devices);

	if (sr_dev_open(sdi) != SR_OK) {
		g_critical("Failed to open device.");
		return;
	}

	/* Set configuration values when -c was specified. */
	set_dev_options_array(sdi, opt_configs);

	/* Get configuration values which were specified by --get. */
	for (get_idx = 0; (get_text = opt_gets[get_idx]); get_idx++) {
		args = parse_generic_arg(get_text, FALSE, "channel_group");
		if (!args)
			continue;
		cg_name = g_hash_table_lookup(args, "sigrok_key");
		cg = lookup_channel_group(sdi, cg_name);
		g_hash_table_iter_init(&iter, args);
		while (g_hash_table_iter_next(&iter, &key, &value)) {
			if (g_ascii_strcasecmp(key, "sigrok_key") == 0)
				continue;
			get_option(sdi, cg, key);
		}
	}

	/* Close the device. */
	sr_dev_close(sdi);
}

static void set_options(void)
{
	struct sr_dev_inst *sdi;
	GSList *devices;

	if (!opt_configs) {
		g_critical("No setting specified.");
		return;
	}

	if (!(devices = device_scan())) {
		g_critical("No devices found.");
		return;
	}
	sdi = devices->data;
	g_slist_free(devices);

	if (sr_dev_open(sdi) != SR_OK) {
		g_critical("Failed to open device.");
		return;
	}

	set_dev_options_array(sdi, opt_configs);

	sr_dev_close(sdi);
}

int main(int argc, char **argv)
{
	g_log_set_default_handler(logger, NULL);

	if (parse_options(argc, argv)) {
		return 1;
	}

	/* Set the loglevel (amount of messages to output) for libsigrok. */
	if (sr_log_loglevel_set(opt_loglevel) != SR_OK)
		goto done;

	if (sr_init(&sr_ctx) != SR_OK)
		goto done;

#ifdef HAVE_SRD
	if (opt_pd_binary && !opt_pds) {
		g_critical("Option -B will not take effect in the absence of -P.");
		goto done;
	}

	/* Set the loglevel (amount of messages to output) for libsigrokdecode. */
	if (srd_log_loglevel_set(opt_loglevel) != SRD_OK)
		goto done;

	if (opt_pds) {
		if (srd_init(NULL) != SRD_OK)
			goto done;
		if (srd_session_new(&srd_sess) != SRD_OK) {
			g_critical("Failed to create new decode session.");
			goto done;
		}
		if (register_pds(opt_pds, opt_pd_annotations) != 0)
			goto done;

		/* Only one output type is ever shown. */
		if (opt_pd_binary) {
			if (setup_pd_binary(opt_pd_binary) != 0)
				goto done;
			if (setup_binary_stdout() != 0)
				goto done;
			if (srd_pd_output_callback_add(srd_sess, SRD_OUTPUT_BINARY,
					show_pd_binary, NULL) != SRD_OK)
				goto done;
		} else if (opt_pd_meta) {
			if (setup_pd_meta(opt_pd_meta) != 0)
				goto done;
			if (srd_pd_output_callback_add(srd_sess, SRD_OUTPUT_META,
					show_pd_meta, NULL) != SRD_OK)
				goto done;
		} else {
			if (opt_pd_annotations)
				if (setup_pd_annotations(opt_pd_annotations) != 0)
					goto done;
			if (srd_pd_output_callback_add(srd_sess, SRD_OUTPUT_ANN,
					show_pd_annotations, NULL) != SRD_OK)
				goto done;
		}
		show_pd_prepare();
	}
#endif

	if (opt_version)
		show_version();
	else if (opt_list_supported)
		show_supported();
	else if (opt_list_supported_wiki)
		show_supported_wiki();
	else if (opt_input_file && opt_show)
		load_input_file(TRUE);
	else if (opt_input_format && opt_show)
		show_input();
	else if (opt_output_format && opt_show)
		show_output();
	else if (opt_transform_module && opt_show)
		show_transform();
	else if (opt_scan_devs)
		show_dev_list();
#ifdef HAVE_SRD
	else if (opt_pds && opt_show)
		show_pd_detail();
#endif
	else if (opt_show)
		show_dev_detail();
	else if (opt_input_file)
		load_input_file(FALSE);
	else if (opt_gets)
		get_options();
	else if (opt_set)
		set_options();
	else if (opt_samples || opt_time || opt_frames || opt_continuous)
		run_session();
	else if (opt_list_serial)
		show_serial_ports();
	else
		show_help();

#ifdef HAVE_SRD
	if (opt_pds)
		show_pd_close();
	if (opt_pds)
		srd_exit();
#endif

done:
	if (sr_ctx)
		sr_exit(sr_ctx);

	return 0;
}

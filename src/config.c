/*
 * Dynamic Network Directory Service
 * Copyright (C) 2009-2014
 * Nicolas J. Bouliane <nib@dynvpn.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 3
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#if defined(__unix__)
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <libconfig.h>
#include <stdlib.h>

#include <logger.h>
#include "agent.h"

static struct dnc_cfg *dnc_cfg;

char *dnc_config_get_fullname(const char *file)
{
#ifdef _WIN32
	char fullname[256];
	snprintf(fullname, sizeof(fullname), "%s%s%s", getenv("AppData"), "\\dynvpn\\", file);
	return strdup(fullname);
#elif __APPLE__
	return strdup(file);
#else
	char fullname[256];
	snprintf(fullname, sizeof(fullname), "%s%s%s", getenv("HOME"), "/.dynvpn/", file);
	return strdup(fullname);
#endif
}

int dnc_config_toggle_auto_connect(int status)
{
	config_setting_t *root, *setting;
	config_t cfg;

	config_init(&cfg);
	root = config_root_setting(&cfg);

	/* Read the file. If there is an error, report it and exit. */
        if (!config_read_file(&cfg, dnc_cfg->dnc_conf)) {
                fprintf(stderr, "Can't open %s\n", dnc_cfg->dnc_conf);
		return -1;
        }

	setting = config_setting_get_member(root, "auto_connect");
	if (setting == NULL) {
                setting = config_setting_add(root, "auto_connect", CONFIG_TYPE_BOOL);
	}
	config_setting_set_bool(setting, status);

	config_write_file(&cfg, dnc_cfg->dnc_conf);
	config_setting_set_bool(setting, status);
	config_destroy(&cfg);

	return 0;
}

int dnc_config_init(struct dnc_cfg *_dnc_cfg)
{
	int ret = 0;
	uint8_t default_conf = 0;
	config_t cfg;
	config_init(&cfg);

	dnc_cfg = _dnc_cfg;

	jlog_init_cb(dnc_cfg->ev.on_log);
	jlog(L_NOTICE, "version: %s", DNCVERSION);

	dnc_cfg->dnc_conf = dnc_config_get_fullname("dynvpn.conf");
	dnc_cfg->ip_conf = dnc_config_get_fullname("dynvpn.ip");

	/* Read the file. If there is an error, use default configuration */
        if (config_read_file(&cfg, dnc_cfg->dnc_conf) == CONFIG_FALSE) {
		default_conf = 1;
        }

#if defined(__unix__) && !defined(__APPLE__)
	/* create ~/.dynvpn if it doesn't exist */
	char *path = dnc_config_get_fullname("");
	struct stat st;
	if (stat(path, &st) != 0) {
		ret = mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR);
		if (ret == -1) {
			free(path);
			return -1;
		}
	}
	free(path);
#endif
	/* create CONFPATH/dynvpn.conf if it doesn't exist */
	if (default_conf == 1) {
		if (config_write_file(&cfg, dnc_cfg->dnc_conf) == CONFIG_FALSE) {
			return -1;
		}
	}

	jlog(L_NOTICE, "conf: %s", dnc_cfg->dnc_conf);

	dnc_cfg->certificate = dnc_config_get_fullname("certificate.pem");
	dnc_cfg->privatekey = dnc_config_get_fullname("privatekey.pem");
	dnc_cfg->trusted_cert = dnc_config_get_fullname("trusted_cert.pem");

	if (config_lookup_string(&cfg, "log_file", &dnc_cfg->log_file)) {
		jlog_init_file(dnc_cfg->log_file);
	}

        if (default_conf || !config_lookup_string(&cfg, "server_address", &dnc_cfg->server_address)) {
		dnc_cfg->server_address = strdup("bhs1.dynvpn.com");
	}
	jlog(L_DEBUG, "server_address = \"%s\";", dnc_cfg->server_address);

        if (default_conf || !config_lookup_string(&cfg, "server_port", &dnc_cfg->server_port)) {
		dnc_cfg->server_port = strdup("9090");
	}
	jlog(L_DEBUG, "server_port = \"%s\";", dnc_cfg->server_port);

	if (default_conf || !config_lookup_bool(&cfg, "auto_connect", &dnc_cfg->auto_connect) ) {
		dnc_cfg->auto_connect = 0;
	}

	return 0;
}

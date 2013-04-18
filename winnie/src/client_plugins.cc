/*
winnie - an experimental window system

Copyright (C) 2013 Eleni Maria Stea

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <vector>
#include <dlfcn.h>
#include "client_plugins.h"

struct Plugin {
	void *so;

	bool (*init)();
	void (*cleanup)();
};

static char *strip_wspace(char *s);

static std::vector<Plugin> plugins;

bool init_client_plugins()
{
	FILE *fp = fopen("plugins.conf", "r");
	if(!fp) {
		fprintf(stderr, "failed to load plugin list: plugins.conf: %s\n", strerror(errno));
		return false;
	}

	plugins.clear();

	char buf[512];
	while(fgets(buf, sizeof buf, fp)) {
		char *line = strip_wspace(buf);

		if(!*line || *line == '#') {
			continue;
		}

		Plugin plugin;

		plugin.so = dlopen(line, RTLD_LAZY);
		if(!plugin.so) {
			fprintf(stderr, "failed to load plugin: %s: %s\n", line, dlerror());
			continue;
		}

		if(!(plugin.init = (bool (*)())dlsym(plugin.so, "init"))) {
			fprintf(stderr, "invalid plugin: %s: no init function\n", line);
			dlclose(plugin.so);
			continue;
		}
		plugin.cleanup = (void (*)())dlsym(plugin.so, "cleanup");

		if(!plugin.init()) {
			fprintf(stderr, "failed to initialize plugin: %s\n", line);
			dlclose(plugin.so);
			continue;
		}

		printf("loaded plugin: %s\n", line);
		plugins.push_back(plugin);
	}

	fclose(fp);
	return true;
}

void destroy_client_plugins()
{
	for(size_t i=0; i<plugins.size(); i++) {
		if(plugins[i].cleanup) {
			plugins[i].cleanup();
		}
		dlclose(plugins[i].so);
	}
	plugins.clear();
}

static char *strip_wspace(char *s)
{
	if(!s || !*s) {
		return s;
	}

	while(*s && isspace(*s)) s++;

	char *endp = s + strlen(s) - 1;
	while(*endp && isspace(*endp)) {
		*endp-- = 0;
	}
	return s;
}

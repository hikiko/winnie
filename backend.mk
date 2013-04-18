backend = SDL

ifeq ($(backend), SDL)
	def = -DWINNIE_SDL
else
	def = -DWINNIE_FBDEV
endif

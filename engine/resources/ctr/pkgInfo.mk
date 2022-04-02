#---------------------------------------------------------------------------------
# setup environment
#---------------------------------------------------------------------------------

#--- Path to bannertool ---
ifeq ($(strip $(TOOLDIR)),)
export TOOLDIR=$(DEVKITPRO)/tools/bin
endif

ifeq ($(OS),Windows_NT)
	MAKEROM    = makerom.exe
	BANNERTOOL = bannertool.exe
else
	MAKEROM    = $(TOOLDIR)/makerom
	BANNERTOOL = $(TOOLDIR)/bannertool
endif

#---------------------------------------------------------------------------------
# options for package generation
#---------------------------------------------------------------------------------

APP_TITLE         := OpenBOR
APP_DESCRIPTION   := OpenBOR port for 3DS
APP_AUTHOR        := http://www.chronocrash.com/

APP_PRODUCT_CODE  := CTR-P-OPENBOR
APP_UNIQUE_ID     := 0xB043D
APP_VERSION_MAJOR := 0
APP_VERSION_MINOR := 0
APP_VERSION_MICRO := 5

APP_ICON          := $(CURDIR)/resources/ctr/OpenBOR_Icon_48x48.png

BANNER_IMAGE_FILE := $(CURDIR)/resources/ctr/OpenBOR_Logo_256x128.png
BANNER_AUDIO_FILE := $(CURDIR)/resources/ctr/audio_silent.wav

# Homebrew Boot animation by PabloMK7
APP_LOGO          := $(CURDIR)/resources/ctr/hb_logo.bin
RSF               := $(CURDIR)/resources/ctr/template.rsf
#ROMFS             :=	$(CURDIR)/resources/ctr/romfs

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

ifeq ($(BUILD_CTR), 2)
CFLAGS += -DCTR_VERSION=\"$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_MICRO)-ctr\"
else
CFLAGS += -DCTR_VERSION=\"$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_MICRO)-ctr-dirty\"
endif

BANNER_IMAGE_ARG            := -i $(BANNER_IMAGE_FILE)
BANNER_AUDIO_ARG            := -a $(BANNER_AUDIO_FILE)

COMMON_MAKEROM_PARAMS       := -rsf $(RSF) -target t -exefslogo -elf $(TARGET) -icon $(TARGET_FINAL).smdh \
		-banner $(TARGET_FINAL).bnr -DAPP_TITLE="$(APP_TITLE)" -DAPP_PRODUCT_CODE="$(APP_PRODUCT_CODE)" \
		-DAPP_UNIQUE_ID="$(APP_UNIQUE_ID)" -DAPP_SYSTEM_MODE="80MB" -DAPP_SYSTEM_MODE_EXT="124MB" \
		-major "$(APP_VERSION_MAJOR)" -minor "$(APP_VERSION_MINOR)" -micro "$(APP_VERSION_MICRO)"

ifneq ($(APP_LOGO),)
	APP_LOGO_ID              = Homebrew
	COMMON_MAKEROM_PARAMS   += -DAPP_LOGO_ID="$(APP_LOGO_ID)" -logo $(APP_LOGO)
else
	APP_LOGO_ID              = Nintendo
	COMMON_MAKEROM_PARAMS   += -DAPP_LOGO_ID="$(APP_LOGO_ID)"
endif

export _3DSXFLAGS           += --smdh=$(TARGET_FINAL).smdh

ifneq ($(ROMFS),)
	CFLAGS                  += -DCTR_ROMFS
	export _3DSXFLAGS       += --romfs=$(ROMFS)
	APP_ROMFS               := $(TOPDIR)/$(ROMFS)
	COMMON_MAKEROM_PARAMS   += -DAPP_ROMFS="$(APP_ROMFS)"
endif

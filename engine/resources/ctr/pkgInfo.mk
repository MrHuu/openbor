#---------------------------------------------------------------------------------
# setup environment
#---------------------------------------------------------------------------------

#--- Path to DEVKITPRO ---
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPRO")
endif

#--- Path to DEVKITARM ---
ifeq ($(strip $(DEVKITARM)),)
export DEVKITARM=$(DEVKITPRO)/devkitARM
endif

#--- Path to PORTLIBS ---
ifeq ($(strip $(PORTLIBS)),)
export PORTLIBS=$(DEVKITPRO)/portlibs/3ds
endif

#--- Path to bannertool ---
ifeq ($(strip $(TOOLDIR)),)
export TOOLDIR=$(DEVKITPRO)/tools/bin
endif

ifeq ($(OS),Windows_NT)
	MAKEROM		=	makerom.exe
	BANNERTOOL	=	bannertool.exe
else
	MAKEROM		=	$(TOOLDIR)/makerom
	BANNERTOOL	=	$(TOOLDIR)/bannertool
endif

#---------------------------------------------------------------------------------
# options for .cia generation
#---------------------------------------------------------------------------------

APP_TITLE		:= OpenBOR
APP_DESCRIPTION	:= Port of OpenBOR
APP_AUTHOR		:= http://www.chronocrash.com/

APP_PRODUCT_CODE	:=	CTR-P-OPENBOR
APP_UNIQUE_ID		:=	0xB043D
APP_VERSION_MAJOR	:=	0
APP_VERSION_MINOR	:=	0
APP_VERSION_MICRO	:=	3

APP_ICON	:=	$(CURDIR)/resources/ctr/OpenBOR_Icon_48x48.png

BANNER_IMAGE_FILE	:=	$(CURDIR)/resources/ctr/OpenBOR_Logo_256x128.png
BANNER_AUDIO_FILE	:=	$(CURDIR)/resources/ctr/audio_silent.wav

# Homebrew Boot animation by PabloMK7
APP_LOGO	:=	$(CURDIR)/resources/ctr/hb_logo.bin

RSF		:=	$(CURDIR)/resources/ctr/template.rsf
#ROMFS	:=	$(CURDIR)/resources/ctr/romfs

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

ifeq ($(ROMFS),)
# CFLAGS		+=	$(INCLUDE) -DARM11 -D_3DS -DDATAPATH=\"sdmc:/3ds/OpenBOR/\"
else
# CFLAGS		+=	$(INCLUDE) -DARM11 -D_3DS -D_3DS_ROMFS -DDATAPATH=\"romfs:/\"
endif

export _3DSXFLAGS		+=	--smdh=$(CURDIR)/$(TARGET).smdh

ifneq ($(ROMFS),)
	export _3DSXFLAGS	+=	--romfs=$(CURDIR)/$(ROMFS)
endif

BANNER_IMAGE_ARG			:=	-i $(BANNER_IMAGE_FILE)
BANNER_AUDIO_ARG			:=	-a $(BANNER_AUDIO_FILE)

COMMON_MAKEROM_PARAMS		:= -rsf $(RSF) -target t -exefslogo -elf $(TARGET).elf -icon icon.icn \
-banner banner.bnr -DAPP_TITLE="$(APP_TITLE)" -DAPP_PRODUCT_CODE="$(APP_PRODUCT_CODE)" \
-DAPP_UNIQUE_ID="$(APP_UNIQUE_ID)" -DAPP_SYSTEM_MODE="80MB" -DAPP_SYSTEM_MODE_EXT="124MB" \
-major "$(APP_VERSION_MAJOR)" -minor "$(APP_VERSION_MINOR)" -micro "$(APP_VERSION_MICRO)"

ifneq ($(APP_LOGO),)
	APP_LOGO_ID				=	Homebrew
	COMMON_MAKEROM_PARAMS	+=	-DAPP_LOGO_ID="$(APP_LOGO_ID)" -logo $(APP_LOGO)
else
	APP_LOGO_ID				=	Nintendo
	COMMON_MAKEROM_PARAMS	+=	-DAPP_LOGO_ID="$(APP_LOGO_ID)"
endif

ifneq ($(ROMFS),)
	APP_ROMFS				:=	$(TOPDIR)/$(ROMFS)
	COMMON_MAKEROM_PARAMS	+=	-DAPP_ROMFS="$(APP_ROMFS)" 
endif

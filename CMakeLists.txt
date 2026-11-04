# PSn00bSDK example CMake script
# (C) 2021 spicyjpeg - MPL licensed

cmake_minimum_required(VERSION 3.21)

project(
	castlemanya
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "CastleManyA my custom Castlevania game"
	HOMEPAGE_URL "http://lameguy64.net/?page=psn00bsdk"
)

psn00bsdk_add_executable(
	castlemanya GPREL
	main.c
	engine/game.c
	engine/input.c
	gameplay/demo/demo_game.c
	gameplay/test/test_area.c
	graphics/renderer.c
	ui/main_menu.c
	ui/settings.c
	helpers/collisions.c
	helpers/utils.c
)

psn00bsdk_add_cd_image(
	castlemanya_iso      # Target name
	castlemanya # Output file name (= castlemanya.bin + castlemanya.cue)
	iso.xml  # Path to config file
	DEPENDS castlemanya system.cnf
)

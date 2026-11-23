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
	engine/input.c
	engine/cd_loader.c
	engine/loader.c
	gameplay/many.c
	graphics/animation.c
	graphics/door.c
	graphics/graphic.c
	helpers/collisions.c
	helpers/level/level_generator.c
	helpers/level/level_manager.c
	helpers/utils.c
	ui/menu.c
	ui/gameplay.c
	world/level01.c
	world/level02.c
	world/level06.c
)

psn00bsdk_add_cd_image(
	castlemanya_iso      # Target name
	castlemanya # Output file name (= castlemanya.bin + castlemanya.cue)
	iso.xml  # Path to config file
	DEPENDS castlemanya system.cnf
)

psn00bsdk_target_incbin(castlemanya PRIVATE loader_tim assets/images/Many-loading.tim)
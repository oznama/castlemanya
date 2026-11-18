@echo off
title Genera PSX Game bin
cls

echo ===================================
echo   Generando build...
echo ===================================
echo.

IF EXIST "./build" (
	RMDIR /S /Q "./build"
	echo Limpiando carpeta build existente.
	echo.
)

echo Integrando archivos de compilacion...
cmake --preset default

echo.

echo Compilando en carpeta build...
cmake --build ./build

echo.
echo Build generado correctamente.
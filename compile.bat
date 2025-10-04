@echo off
echo Compilando proyecto con sistema de carga optimizado...

gcc -Iinclude -Wall -O2 -std=c99 src/*.c -Llib -lglfw3 -lopengl32 -lglu32 -lgdi32 -lwinmm -o PROYECTOTERROR.exe

if %ERRORLEVEL% EQU 0 (
    echo Compilacion exitosa! Ejecutando PROYECTOTERROR.exe...
    PROYECTOTERROR.exe
) else (
    echo Error en la compilacion. Codigo de error: %ERRORLEVEL%
)

pause

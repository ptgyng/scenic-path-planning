@echo off
REM ============================================================
REM  build.bat — 一键编译景区路径规划系统 (Windows)
REM ============================================================

setlocal
if "%1"=="cl" goto msvc
if "%1"=="utf8" goto gcc_utf8

:gcc_default
echo === 用 GCC (MinGW) 编译 ===
gcc -O2 -o scenic.exe graph.c travels.c menu.c main.c
if errorlevel 1 (
    echo 编译失败！请确认已安装 GCC (MinGW 或 MinGW-w64)
    goto end
)
echo 编译成功: scenic.exe
goto end

:gcc_utf8
echo === 用 GCC 编译, UTF-8 输出 ===
gcc -O2 -fexec-charset=UTF-8 -o scenic.exe graph.c travels.c menu.c main.c
if errorlevel 1 (
    echo 编译失败！
    goto end
)
echo 编译成功: scenic.exe
goto end

:msvc
echo === 用 MSVC (cl.exe) 编译 ===
cl /O2 /Fe:scenic.exe graph.c travels.c menu.c main.c
if errorlevel 1 (
    echo 编译失败！请确认已在 Visual Studio 开发人员命令提示符中运行
    goto end
)
echo 编译成功: scenic.exe

:end
endlocal

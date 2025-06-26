@echo off

title Flashing MasterDemo
mode con:cols=100 lines=50
set PlatformName=STM32H750-Discovery

echo.
echo *******************************************************************************
echo *
echo * Flashing Embedded Wizard Master Demo for target %PlatformName%
echo *
echo *******************************************************************************
echo.

call ..\Application\FlashDownload\FlashDownload.cmd BootLoader-%PlatformName%.hex separateConsole
call ..\Application\FlashDownload\FlashDownload.cmd EmbeddedWizard-%PlatformName%.hex separateConsole

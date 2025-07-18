# Getting started with STM MCUs: STM32H750 Discovery

<img src="https://doc.embedded-wizard.de/images/9.30/stm32h750-discovery-1.png" width=50% height=50%>
<p><em>STM32H750 Discovery board (STM32H750B-DK)</em></p>

The following article explains all necessary steps to create an Embedded Wizard UI application suitable for the STM32H750 Discovery board (STM32H750B-DK) from STMicroelectronics.

Please follow these instructions carefully and step by step in order to ensure that you will get everything up and running on your target. In case you are not familiar with Embedded Wizard, please read first the chapter on basic concepts and the Quick Tour tutorial to understand the principles of Embedded Wizard and the GUI development workflow.

## 1. Prerequisites

First, make sure you have all of the following items:

### 1.1 Hardware components

- **STM32H750 Discovery board** from STMicroelectronics (STM32H750B-DK)
- **USB cable** to connect the board with your PC

### 1.2 Software components

- **Embedded Wizard Studio Free or Embedded Wizard Studio Pro**
  > If you want to use the Free edition of Embedded Wizard Studio please register on the Embedded Wizard website and download the software. As a customer, please visit the Embedded Wizard Download Center (login/password required) and download Embedded Wizard Studio Pro.

- **Embedded Wizard Build Environment for STM32H750 Discovery**
  > To evaluate Embedded Wizard on the mentioned target, you can download the suitable Build Environment from the provided link. As a customer, please visit the Embedded Wizard Download Center (login/password required) and download the latest version of the Build Environment and your licensed Platform Package libraries or source codes.

- **STM32CubeProgrammer** from STMicroelectronics
- **Optional: STM32CubeIDE** from STMicroelectronics
- **Optional: IAR Embedded Workbench**
- **Optional: Keil MDK-ARM**

## 2. Installing Tools and Software

**Step 1:** Install the latest version of Embedded Wizard Studio.

**Step 2:** Install the STM32CubeProgrammer utility. Test the connection from PC to Discovery board and the proper installation of the USB drivers: Connect the Discovery board with your PC via USB (make sure to use the ST-LINK USB connector) and start the previously installed STM32CubeProgrammer. Select the green **Connect** button and verify that the connection could be established successfully. Finally, close the STM32CubeProgrammer utility.

**Step 3:** Unpack the provided Embedded Wizard Build Environment for STM32H750 Discovery to your local file system (e.g. `C:\STM32\STM32H750-Discovery`). Please make sure to use a short working folder path because STM32CubeIDE (to be more precise: Eclipse) has restrictions regarding the file path length.

**Step 4:** Take a text editor and open the file `FlashDownload.cfg` that you will find in the subdirectory `\Application\FlashDownload` of the Build Environment. At the beginning of the file, the following environment variable has to be set to the installation path of the STM32CubeProgrammer utility:
- `Stm32CubeProgrammerPath` - Absolute path of your installed STM32CubeProgrammer utility (Step 2).

**Step 5:** Make the same changes within the file `StartGccBuildEnvironment_Bootloader.bat` that you will find in the root directory of the Build Environment.

## 3. Embedded Wizard GUI Demos

If you just want to run our GUI demos on your STM32H750 Discovery board without building the different examples, you can simply flash the binary file of the Embedded Wizard Master Demo.

The Embedded Wizard Master Demo combines a variety of examples within one huge demo application. It can be used for presentations and showcases. Each demo application can be activated from a common main menu. To return back from a demo application to the main menu, a small home button appears on top of every demo. Additionally, the Master Demo contains an auto-demo mode that presents one demo application after the other. The auto-demo starts automatically and stops as soon as the user touches the screen.

In order to flash the binary file to your target, please follow these steps:

**Step 1:** Connect your development board with your PC via USB (make sure to use the ST-LINK USB connector).

**Step 2:** Execute the batch file `FlashMasterDemo.bat`: A console window will appear and the flash tool will download the binary file of the Master Demo.

<img src="https://doc.embedded-wizard.de/images/9.30/stm32h750-discovery-8.png" width=50% height=50%>
<p><em>'Master Demo' running on STM32H750 Discovery board.</em></p>

## 4. Exploring the Build Environment

The provided Embedded Wizard Build Environment for STM32H750 Discovery contains everything you need to create, compile, link and flash an Embedded Wizard UI application for the STM32H750 target. After unpacking, you will find the following subdirectories and files:

### Root Directory Files
- `StartGccBuildEnvironment.bat` - Script file to start a windows command line to build your GUI applications for the target.
- `StartGccBuildEnvironment_Bootloader.bat` - Script file to start a windows command line to build the necessary bootloader for the target.

### Application Folder (`\Application`)
This folder contains ready-to-use projects to compile and link an Embedded Wizard generated UI application. They are used for all provided examples and they can be used to build your own UI applications.

- **`\FlashDownload`** - Contains a script to load the created binaries into the flash of your target by using the STM32CubeProgrammer tool. Don't forget to set the path of your installed STM32CubeProgrammer utility within the file `FlashDownload.cfg`.
- **`\GeneratedCode`** - This folder is used to receive the generated code from an Embedded Wizard UI project. All template projects are building the UI application out of this folder. You can create your own UI project and generate the code into the subdirectory `\GeneratedCode` without the need to adapt the project.
- **`\Project`** - Contains the prepared projects for GCC (make), IAR Embedded Workbench, Keil MDK-ARM and STM32CubeIDE.
- **`\Source`** - Contains the files `main.c` and `ewmain.c`. There you will find the initialization of the system and the main loop to drive an Embedded Wizard GUI application. The file `ewconfig.h` contains general configuration settings for the target system, like memory ranges and display parameter and configuration settings for the Embedded Wizard Graphics Engine and Runtime Environment. Additionally, this folder contains a configuration file for FreeRTOS and the device driver C/H files used for the DeviceIntegration example.

### Bootloader Folder (`\Bootloader`)
This folder contains ready-to-use projects to compile and link the necessary bootloader in order to support code execution from external memory (QSPI flash).

- **`\Project`** - Contains the prepared projects for GCC (make), IAR Embedded Workbench and Keil MDK-ARM.
- **`\Source`** - Contains the source code of the bootloader to support code execution from external memory (QSPI flash). There you will find the necessary initialization of the system to start the GUI application located in the QSPI flash.

### Examples Folder (`\Examples\<ScreenSize>`)
This folder contains a set of demo applications prepared for a dedicated screen size (480x272 pixel). Each example is stored in a separate folder containing the entire Embedded Wizard UI project. Every project contains the necessary profile settings for the STM32H750 target. The following samples are provided:

- **HelloWorld** - A very simple project that is useful as starting point and to verify that the entire toolchain, your installation and your board is properly working.
- **ColorFormats** - This project demonstrates that every UI application can be generated for different color formats: RGBA8888, RGB888, RGBA4444, RGB565, Index8 and LumA44.
- **ScreenOrientation** - This demo shows, that the orientation of the UI application is independent from the physical orientation of the display.
- **DeviceIntegration** - This example shows the integration of devices into a UI application and addresses typical questions: How to start a certain action on the target? How to get data from a device?
- **GraphicsAccelerator** - This application demonstrates the graphics performance of the DMA2D hardware graphics accelerator. Sets of basic drawing operations are executed permanently and continuously, while the user can switch on/off the hardware accelerator.
- **BezierClock** - The sample application BezierClock implements a fancy digital clock and timer application with animated digits. The application uses vector graphics to render dynamically the different digits for clock and timer. The change from one digit to another is handled by moving the vector points to get a smooth transition animation.
- **BrickGame** - The sample application BrickGame implements a classic "paddle and ball" game. In the game, a couple of brick rows are arranged in the upper part of the screen. A ball travels across the screen, bouncing off the top and side walls of the screen. When a brick is hit, the ball bounces away and the brick is destroyed. The player has a movable paddle to bounce the ball upward, keeping it in play.
- **ClimateCabinet** - The ClimateCabinet demo shows the implementation of a control panel for a climatic exposure test cabinet. The user can define a heating time, a nominal temperature and humidity, a dwell time and the final cooling time.
- **Dashboard** - The sample application Dashboard shows the implementation of a car dashboard. The application demonstrates the usage of vector graphics created from svg path data and how transitions between two dashboard variants and further UI components can be achieved.
- **PaperCutter** - This demo shows the implementation of a paper cutting machine, where the user can define the pagination and format of the paper as well as the cutting speed and the amount of papers. The application contains many rect effects and fade-in/fade-out effects to change dynamically the layout of the screen.
- **PulseOximeter** - The sample application PulseOximeter shows the implementation of a medical device for monitoring a person's pulse frequency and peripheral oxygen saturation. The application demonstrates the usage of vector graphics within graphs and circular gauges.
- **SmartThermostat** - The SmartThermostat demo application shows the implementation of a fancy, rotatable temperature controller to adjust and display the nominal and actual temperature.
- **WashingMachine** - This demo shows the implementation of a washing machine with a couple of fancy scrollable list widgets to choose the washing program and parameters. The speciality of this sample application is the magnification effect of the centered list items and the soft fade-in/fade-out effects.
- **WaveformGenerator** - This WaveformGenerator demo application combines waveforms with different amplitudes and frequencies. The implementation shows the usage of vector graphics to draw a curve based on a list of coordinates.

### Master Demo Folder (`\MasterDemo`)
This folder contains the binary file of the Embedded Wizard Master Demo application and a script file to flash the demo on your target. The Master Demo combines a variety of examples within one huge demo application. It can be used for presentations and showcases.

### Platform Package Folder (`\PlatformPackage`)
This folder contains the necessary source codes and/or libraries of the STM32 Platform Package: Several Graphics Engines for the different color formats (RGBA8888, RGB888, RGBA4444, RGB565, Index8 and LumA44) and the Runtime Environment (in the subdirectory `\RTE`).

### Target Specific Folder (`\TargetSpecific`)
This folder contains all configuration files and platform specific source codes. The different `ew_bsp_xxx` files implement the bridge between the Embedded Wizard UI application and the underlying board support package (STM32 hardware drivers) in order to access the display, the graphics accelerator, the serial interface and the clock.

### Third Party Folder (`\ThirdParty`)
This folder contains third-party source codes and tools:
- **`\gcc-arm-none-eabi`** - Contains a subset of the ARM GNU toolchain to compile the examples.
- **`\Make`** - Contains a `make` tool to build the entire GUI application via command line.
- **`\STM32Cube_FW_H7`** - Contains the necessary subset of the STM32CubeH7 embedded software for STM32H7 series used for the Embedded Wizard UI applications (HAL, BSP, drivers, FreeRTOS).

## 5. Code Execution from External Memory (Bootloader)

Due to the fact that the STM32H750 MCU provides only a small internal flash memory, the entire GUI application has to be launched from the external QSPI flash memory. For that purpose, a small bootloader is necessary in order to initialize the system and to start the code execution from QSPI flash. For more details, please have a look into the application note AN5188.

The following steps are necessary to build and flash the bootloader using the GCC ARM embedded toolchain:

**Step 1:** Navigate to the top level of the Build Environment.

**Step 2:** Open `StartGccBuildEnvironment_Bootloader.bat` - as a result, a windows command line window should open. In case there are error messages, please edit the file and double-check the path settings.

**Step 3:** Now start compiling, linking and flashing:

```bash
make
make install
```

If everything works as expected, the bootloader should be built and flashed to the STM32H750 target. Now the code is executed from external memory (QSPI flash). You can now build and download any GUI application into the QSPI flash memory.

> **Note:** Flashing the bootloader is only necessary one time.

## 6. Creating the UI Examples

For the first bring up of your system, we recommend to use the example 'HelloWorld':

<img src="https://doc.embedded-wizard.de/images/9.30/stm32h750-discovery-3.png" width=50% height=50%>
<p><em>Example 'HelloWorld' within Embedded Wizard Studio.</em></p>

The following steps are necessary to generate the source code of this sample application:

**Step 1:** Navigate to the directory `\Examples\<ScreenSize>\HelloWorld`.

**Step 2:** Open the project file `HelloWorld.ewp` with your previously installed Embedded Wizard Studio. The entire project is well documented inline. You can run the UI application within the Prototyper by pressing `Ctrl+F5`.

**Step 3:** To start the code generator, select the menu items **Build/Build this profile** - or simply press `F8`. Embedded Wizard Studio generates now the sources files of the example project into the directory `\Application\GeneratedCode`.

## 7. Compiling, Linking and Flashing

The following steps are necessary to build and flash the Embedded Wizard UI sample application using the GCC ARM embedded toolchain:

**Step 1:** Navigate to the top level of the Build Environment.

**Step 2:** Open `StartGccBuildEnvironment.bat` - as a result, a windows command line window should open. In case there are error messages, please edit the file and double-check the path settings.

**Step 3:** Now start compiling, linking and flashing:

```bash
make
make install
```

If everything works as expected, the application should be built and flashed to the STM32H750 target.

<img src="https://doc.embedded-wizard.de/images/9.30/stm32h750-discovery-4.png" width=50% height=50%>
<p><em>Example 'HelloWorld' running on STM32H750 Discovery board.</em></p>

All other examples can be created in the same way: Just open the desired example with Embedded Wizard Studio, generate code and rebuild the whole application using simply:

```bash
make install
```

## 8. Creating your own UI Applications

In order to create your own UI project suitable for the STM32H750 target, you can create a new project and select the STM32H750 Discovery project template:

<img src="https://doc.embedded-wizard.de/images/12.00/stm32h750-discovery-5.png" width=80% height=80%>

As a result you get a new Embedded Wizard project, that contains the necessary Profile attributes suitable for the STM32H750 Discovery board:

<img src="https://doc.embedded-wizard.de/images/9.30/stm32h750-discovery-6.png" width=30% height=30%>

The following profile settings are important for your target:

**Step 1:** The attribute `PlatformPackage` should refer to the STM32 Platform Package.

**Step 2:** The attribute `ScreenSize` should correspond to the display size of the STM32H750 Discovery board.

**Step 3:** The attributes `ModeOfBitmapResources` and `ModeOfStringConstants` can be set to `DirectAccess` in case that the resources should be taken directly from flash memory. By default these attributes are set to `Compressed`.

**Step 4:** The attribute `OutputDirectory` should refer to the `\Application\GeneratedCode` directory within your Build Environment. By using this template, it will be very easy to build the UI project for your target.

**Step 5:** The attribute `CleanOutputDirectories` should be set to `true` to ensure that unused source code within the output directory `\Application\GeneratedCode` will be deleted.

**Step 6:** The attribute `PostProcess` should refer to:
- `\Application\Project\EWARM\EWARM_ew_post_process.cmd` if you are working with IAR Embedded Workbench
- `\Application\Project\MDK-ARM\MDK-ARM_ew_post_process.cmd` if you are working with Keil MDK-ARM
- `\Application\Project\STM32CubeIDE\STM32H750-Discovery\STM32CubeIDE_ew_post_process.cmd` if you are working with STM32CubeIDE
- Leave it blank in case of the GCC ARM embedded toolchain

Now you can use the template project in the same manner as it was used for the provided examples to compile, link and flash the binary. After generating code, please follow these steps, in order to build your own UI application:

**Step 1:** Start the batch file `StartGccBuildEnvironment.bat`. Again, a windows command line window should open.

**Step 2:** Start compiling, linking and flashing:

```bash
make install
```

Most of the project settings are taken directly out of the generated code, like the color format or the screen orientation. Only a few additional settings can be configured directly within the `Makefile`, like the usage of an external flash memory or the usage of the FreeRTOS operating system. All other settings can be made directly within the file `ewconfig.h`, which contains general configuration settings for the target system.

## 9. Console Output

In order to receive error messages or to display simple debug or trace messages from your Embedded Wizard UI application, a serial terminal like 'Putty' or 'TeraTerm' should be used.

**Step 1:** As soon as you connect your STM32H750 target with the PC via USB, a `STMicroelectronics STLink Virtual COM Port (COMx)` appears within your system device list. Open the device manager to get the number of the installed COM port.

**Step 2:** Now you can open your terminal application and connect it via COMx with the following settings: **115200-8-N-1**

<img src="https://doc.embedded-wizard.de/images/13.00/stm32h750-discovery-7.png" width=80% height=80%>

This terminal connection can be used for all trace statements from your Embedded Wizard UI applications or for all debug messages from your C code.

## 10. Using IAR Embedded Workbench

If you want to use the IAR Embedded Workbench instead of the GCC ARM embedded toolchain, please follow these instructions:

> **Tip:** By default, the external QSPI flash of the STM32 board is used to store string constants, bitmap resources and font resources. If the flash downloader integrated in IAR Embedded Workbench doesn't support the external flash, the examples can alternatively be flashed using the STM32CubeProgrammer. For more convenience the STM32CubeProgrammer can be integrated to IAR Embedded Workbench as an external tool.
> 
> Command and argument are as follows:
> - Command: `$PROJ_DIR$\..\..\FlashDownload\FlashDownload.cmd`
> - Arguments: `$TARGET_PATH$ separateConsole`

The subdirectory `\Application\Project\EWARM` contains a template project that is commonly used for all provided Embedded Wizard examples. All Embedded Wizard examples will store the generated code within the common `\Application\GeneratedCode` folder.

The generated code of an Embedded Wizard example is imported automatically to the IAR Embedded Workbench project using the Project Connection mechanism.

To establish this automatic project import a post process has to be added to the Profile settings within Embedded Wizard Studio:

**Step 1:** Open the desired Embedded Wizard example project.

**Step 2:** Select the Profile and set the attribute `PostProcess` to the file `..\..\..\Application\Project\EWARM\EWARM_ew_post_process.cmd`.

After the Embedded Wizard code generation the installed post process will generate a `ewfiles.ipcf` file, that controls the import to the IAR Embedded Workbench project.

After returning to IAR Embedded Workbench, the latest generated code and the suitable Embedded Wizard Platform Package will be imported to the IAR Embedded Workbench project (depending on the color format and the screen orientation selected in the Embedded Wizard Profile).

If the color format or the screen orientation was changed, please do a complete rebuild of the IAR Embedded Workbench project.

## 11. Using Keil MDK-ARM

If you want to use the Keil MDK-ARM toolchain instead of the GCC ARM embedded toolchain, please follow these instructions:

The subdirectory `\Application\Project\MDK-ARM` contains a template project that is commonly used for all provided Embedded Wizard examples. All Embedded Wizard examples will store the generated code within the common `\Application\GeneratedCode` folder.

The generated code of an Embedded Wizard example is imported automatically to the Keil MDK-ARM project using the CMSIS PACK mechanism.

The following steps are needed to establish this automatic project import:

**Step 1:** Install `Tara.Embedded_Wizard_Launcher.x.x.x.pack` by double clicking. You will find the file within the subdirectory `\Application\Project\MDK-ARM`.

**Step 2:** Open the desired Embedded Wizard example project.

**Step 3:** Select the Profile and set the attribute `PostProcess` to the file `..\..\..\Application\Project\MDK-ARM\MDK-ARM_ew_post_process.cmd`.

After the Embedded Wizard code generation the installed post process will generate a `ewfiles.gpdsc` file, that controls the Keil MDK-ARM project import.

In Keil MDK-ARM a dialog appears: "For the current project new generated code is available for import". After confirmation, the latest generated code and the suitable Embedded Wizard Platform Package will be imported to the Keil MDK-ARM project (depending on the color format and the screen orientation selected in the Embedded Wizard Profile).

If the color format or the screen orientation was changed, please do a complete rebuild of the Keil MDK-ARM project.

## 12. Using STM32CubeIDE

If you want to use the STM32CubeIDE toolchain instead of the GCC ARM embedded toolchain, please follow these instructions:

The subdirectory `\Application\Project\STM32CubeIDE` contains a template project that is commonly used for all provided Embedded Wizard examples. All Embedded Wizard examples will store the generated code within the common `\Application\GeneratedCode` folder.

The following steps are needed to establish this automatic project import:

**Step 1:** Open the desired Embedded Wizard example project.

**Step 2:** Select the Profile and set the attribute `PostProcess` to the file `..\..\..\Application\Project\STM32CubeIDE\STM32H750-Discovery\STM32CubeIDE_ew_post_process.cmd`.

After the Embedded Wizard code generation the installed post process will adapt the `.cproject` XML file. All necessary libraries and include paths (depending on the color format and screen rotation) will be set automatically.

The given STM32CubeIDE example under `\Application\STM32CubeIDE` contains a workspace which has all adaptions for an Embedded Wizard project. For using this within STM32CubeIDE do following steps:

**Step 1:** Open STM32CubeIDE and select the directory `\Application\Project\STM32CubeIDE` as workspace directory.

**Step 2:** To import the C project, select the menu item **File/Import** and choose **General** - **Existing Projects into Workspace** and press **Next**.

**Step 3:** Choose **Select root directory** - **Browse** and select the directory `\Application\Project\STM32CubeIDE\STM32H750-Discovery`.

**Step 4:** Press **Finish**.

**Step 5:** To compile the project press the **Build** button (hammer symbol) within the toolbar.

**Step 6:** Press the **Run** button within the toolbar to start the application on the target.

If the color format or the screen orientation was changed, please do a clean in STM32CubeIDE.

## 13. Custom Specific Hardware

In order to bring-up an Embedded Wizard generated UI application on your STM32 custom hardware, you can use the provided Embedded Wizard Build Environment for STM32H750 Discovery as a template. The file `ewconfig.h` contains target specific settings that have to be adjusted to your particular hardware. Furthermore, the subdirectory `\TargetSpecific` contains all the platform specific source codes. Assuming that your custom hardware is similar to the STM32H750 Discovery board, the following adaptations or configurations are typically necessary:

**Step 1:** **System clock** (`ew_bsp_system.c`) - The first and the important step is to configure the system and peripheral clock. Depending on your hardware you can use the internal or external clock as source. Please take care that your LTDC and USART are connected to the selected clock source and configured correctly. The Embedded Wizard UI application runs independent from the chosen system frequency, however, with a slow system clock, all components need more time for their tasks (e.g. display refresh).

**Step 2:** **SDRAM** (`ew_bsp_system.c`) - The SDRAM configuration has to be adapted to your particular SDRAM. Please use a memory test to ensure that writing and reading works properly. If the start address and/or the size of the SDRAM has changed, please adapt the settings for the framebuffer and the memory pool within the file `ewconfig.h`.

**Step 3:** **MPU** (`ew_bsp_system.c`) - Please take care to configure the MPU properly in order to avoid unexpected speculative read accesses outside of valid memory regions. For more details see AN4838 and AN4861 chapter 4.6.2.

**Step 4:** **USART** (`ew_bsp_console.c`) - Typically, the USART configuration just requires a new pinout according your hardware layout. The usage of the serial connection is highly recommended in order to get status and debug messages during runtime.

**Step 5:** **LTDC** (`ew_bsp_display.c`) - The LTDC is an integrate display controller which allows you to connect many different display types to a STM32 device. Here you have to adjust the interface to your display. It is important to adjust your pinout, polarity, timings, color format and layer settings according your dedicated display. For more details concerning LTDC settings please have a look to AN4861. In case you are using a MIPI-DSI display, please use the Build Environment for STM32H747-Discovery as template.

**Step 6:** **Touch** (`ew_bsp_touch.c`) - If your application requires touch support, you can integrate a given touch driver provided by the touch controller manufacturer or write your own. As a result the current touch position should be returned.

As soon as these steps are done, you can create your own GUI application or use one of the provided examples. If the size of your display is different compared to the display of the STM32H750 Discovery board (480x272 pixel), please adapt the attribute `ScreenSize` of the UI project and the size of the framebuffer within the file `ewconfig.h` accordingly.

In case you want to change the operating system, please adapt the all functions within the file `ew_bsp_os.c` to your particular operating system. Furthermore, adapt the makefile/projectfile accordingly.

## 14. Release Notes

The following section contains the version history of the Build Environment (including Graphics Engine and Runtime Environment) for STM32H750 Discovery. These release notes describe only the platform specific aspects - for all general improvements and enhancements please see the Embedded Wizard release notes.

### Version 14.00.00.00

**Features:**
- Using Graphics Engine (GFX) and Runtime Environment (RTE) V14.00
- Using STM32CubeProgrammer V2.17.0
- Using STM32CubeH7 Firmware Package V1.11.0
- Using Arm GNU Toolchain V11.2-2022.02
- Using IAR Embedded Workbench 9.60.2, IAR C/C++ Compiler V9.60.2.399/W64 for ARM
- Using Keil MDK-ARM Professional Version 5.41.0.0, ARM Compiler 6.22 (armclang)
- Using STM32CubeIDE V1.16.1

**Known Issues:**
- Using STM32CubeIDE V1.16.1 will generate a couple of linker warnings and errors (e.g. "_close is not implemented and will always fail") - which can be ignored.

---

## Support

For additional support and documentation, please visit:
- [Embedded Wizard Website](https://www.embedded-wizard.de)
- [STMicroelectronics Support](https://www.st.com/content/st_com/en/support/support-home.html)

## License

Please refer to the license terms provided with the Embedded Wizard Build Environment and STMicroelectronics software packages.
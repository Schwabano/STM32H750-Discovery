/*******************************************************************************
*
* E M B E D D E D   W I Z A R D   P R O J E C T
*
*                                                Copyright (c) TARA Systems GmbH
*                                    written by Paul Banach and Manfred Schweyer
*
********************************************************************************
*
* This software is delivered "as is" and shows the usage of other software
* components. It is provided as an example software which is intended to be
* modified and extended according to particular requirements.
*
* TARA Systems hereby disclaims all warranties and conditions with regard to the
* software, including all implied warranties and conditions of merchantability
* and non-infringement of any third party IPR or other rights which may result
* from the use or the inability to use the software.
*
********************************************************************************
*
* DESCRIPTION:
*   This file is part of the interface (glue layer) between an Embedded Wizard
*   generated UI application and the board support package (BSP) of a dedicated
*   target.
*   Please note: The implementation of this module is partially based on
*   examples that are provided within the STM32 cube firmware. In case you want
*   to adapt this module to your custom specific hardware, please adapt the
*   hardware initialization code according your needs or integrate the generated
*   initialization code created by using the tool CubeMX.
*   This template is responsible to initialize the display hardware of the board
*   and to provide the necessary access to update the display content.
*   The color format of the framebuffer has to correspond to the color format
*   of the Graphics Engine.
*
*   Important: This file is intended to be used as a template. Please adapt the
*   implementation according your particular hardware.
*
*******************************************************************************/

#include "ewconfig.h"
#include "stm32h7xx_hal.h"
#include "stm32h750b_discovery.h"

#include <string.h>

#include "ewrte.h"

#include "ew_bsp_display.h"
#include "ew_bsp_clock.h"
#include "ew_bsp_os.h"

#if EW_USE_OPERATING_SYSTEM == 1

  static XSemaphoreHandle     LcdUpdateSemaphore = 0;

#endif

#define LAYER_INDEX           0

LTDC_HandleTypeDef            LtdcHandle;
LTDC_LayerCfgTypeDef          LayerConfig;

#if EW_USE_DOUBLE_BUFFER == 1

  static volatile uint32_t    CurrentFramebuffer = 0;
  static volatile uint32_t    PendingFramebuffer = 0;

#else

  static volatile int         CurrentLine = -1;
  static volatile int         PendingLine = -1;

#endif


/**
  * @brief LTDC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
  GPIO_InitTypeDef gpio_init_structure;

 /* Enable the LTDC and DMA2D clocks */
  __HAL_RCC_LTDC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  /*** LTDC Pins configuration ***/
  /* GPIOI configuration */
  gpio_init_structure.Pin       = GPIO_PIN_0 |GPIO_PIN_1 |GPIO_PIN_9 |GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

  /* GPIOJ configuration */
  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                                  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

  /* GPIOK configuration */
  gpio_init_structure.Pin       = GPIO_PIN_2 | GPIO_PIN_3 | \
                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 ;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOK, &gpio_init_structure);

  /* GPIOH configuration */
  gpio_init_structure.Pin       =  GPIO_PIN_9 | GPIO_PIN_1;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

  gpio_init_structure.Pin       = GPIO_PIN_7;     /* LCD_DISP pin has to be manually controlled */
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);
    /* Assert display enable LCD_DISP pin */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);

  /* Set LTDC Interrupt priority */
  HAL_NVIC_SetPriority(LTDC_IRQn, 8, 0);

  /* Enable LTDC Interrupt */
  HAL_NVIC_EnableIRQ(LTDC_IRQn);
}

/**
  * @brief LTDC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef *hltdc)
{
  /*##-1- Reset peripherals ##################################################*/
  /* Enable LTDC reset state */
  __HAL_RCC_LTDC_FORCE_RESET();

  /* Release LTDC from reset state */
  __HAL_RCC_LTDC_RELEASE_RESET();
}

void LTDC_IRQHandler( void )
{
  HAL_LTDC_IRQHandler( &LtdcHandle );
}


void HAL_LTDC_LineEventCallback( LTDC_HandleTypeDef* aLtdc )
{
  #if EW_USE_DOUBLE_BUFFER == 1

    register uint32_t pendingBuffer = PendingFramebuffer;
    if ( CurrentFramebuffer != pendingBuffer )
    {
      /* change address within layer configuration structure */
      LayerConfig.FBStartAdress = pendingBuffer;

      /* change layer configuration */
      if ( HAL_LTDC_ConfigLayer( &LtdcHandle, &LayerConfig, LAYER_INDEX ) != HAL_OK )
        EwPrint( "HAL_LTDC_LineEventCallback: Could not change layer configuration!\n" );

      #if EW_USE_OPERATING_SYSTEM == 1

        EwBspOsSemaphoreRelease( LcdUpdateSemaphore );

      #else

        /* save new address */
        CurrentFramebuffer = pendingBuffer;

      #endif
    }

    /* program next V-sync */
    HAL_LTDC_ProgramLineEvent( aLtdc, 0 );

  #else

    /* update current line number */
    CurrentLine = PendingLine;

    #if EW_USE_OPERATING_SYSTEM == 1

      EwBspOsSemaphoreRelease( LcdUpdateSemaphore );

    #endif

  #endif
}


/* Helper function to sync on a certain line number updated by the display.
   This function is necessary when the system is used in single framebuffer mode. */
#if EW_USE_DOUBLE_BUFFER == 0
static void SyncOnLine( int aLine )
{
  /* add the vsync area and the vertical back porch */
  aLine = aLine + LtdcHandle.Init.AccumulatedVBP + 1;

  /* ensure that requested line is still inside active area or vertical front porch */
  if ( aLine > LtdcHandle.Init.TotalHeigh + 1 )
    aLine = 0;

  CurrentLine = -1;
  PendingLine = aLine;

  CPU_LOAD_SET_IDLE();

  /* program line number of next field */
  HAL_LTDC_ProgramLineEvent( &LtdcHandle, aLine );

  #if EW_USE_OPERATING_SYSTEM == 1

    EwBspOsSemaphoreWait( LcdUpdateSemaphore, 1000 );

  #else

    while( aLine != CurrentLine )
      ;

  #endif

  CPU_LOAD_SET_ACTIVE();
}
#endif


/*******************************************************************************
* FUNCTION:
*   EwBspDisplayInit
*
* DESCRIPTION:
*   The function EwBspDisplayInit initializes the display hardware and returns
*   the display parameter.
*
* ARGUMENTS:
*   aGuiWidth,
*   aGuiHeight   - Size of the GUI in pixel.
*   aDisplayInfo - Display info data structure.
*
* RETURN VALUE:
*   Returns 1 if successful, 0 otherwise.
*
*******************************************************************************/
int EwBspDisplayInit( int aGuiWidth, int aGuiHeight, XDisplayInfo* aDisplayInfo )
{
  EW_UNUSED_ARG( aGuiWidth );
  EW_UNUSED_ARG( aGuiHeight );

  /* check and clean display info structure */
  if ( !aDisplayInfo )
    return 0;
  memset( aDisplayInfo, 0, sizeof( XDisplayInfo ));

  /* initialize given framebuffer */
  EwZero((void*)EW_FRAME_BUFFER_ADDR, EW_FRAME_BUFFER_WIDTH * EW_FRAME_BUFFER_HEIGHT * EW_FRAME_BUFFER_DEPTH );

  #if EW_USE_OPERATING_SYSTEM == 1

    /* create the LCD update semaphore */
    LcdUpdateSemaphore = EwBspOsSemaphoreCreate( 1, 1 );

  #endif

  /* Polarity configuration */
  /* Initialize the horizontal synchronization polarity as active low */
  LtdcHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  /* Initialize the vertical synchronization polarity as active low */
  LtdcHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  /* Initialize the data enable polarity as active low */
  LtdcHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  /* Initialize the pixel clock polarity as input pixel clock */
  LtdcHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

// for some reason the display content is shifted.... ??? msy
#define SHIFT 10

  /* Timing configuration for RK043FN48H 480x272 LCD */
  /* Horizontal synchronization width = Hsync - 1 */
  LtdcHandle.Init.HorizontalSync = 40 - SHIFT;
  /* Vertical synchronization height = Vsync - 1 */
  LtdcHandle.Init.VerticalSync = 9;
  /* Accumulated horizontal back porch = Hsync + HBP - 1 */
  LtdcHandle.Init.AccumulatedHBP = 53 - SHIFT;
  /* Accumulated vertical back porch = Vsync + VBP - 1 */
  LtdcHandle.Init.AccumulatedVBP = 11;
  /* Accumulated active width = Hsync + HBP + Active Width - 1 */
  LtdcHandle.Init.AccumulatedActiveW = 533 - SHIFT;
  /* Accumulated active height = Vsync + VBP + Active Height - 1 */
  LtdcHandle.Init.AccumulatedActiveH = 283;
  /* Total width = Hsync + HBP + Active Width + HFP - 1 */
  LtdcHandle.Init.TotalWidth = 565 - SHIFT;
  /* Total height = Vsync + VBP + Active Height + VFP - 1 */
  LtdcHandle.Init.TotalHeigh = 285;

  /* Configure R,G,B component values for LCD background color */
  LtdcHandle.Init.Backcolor.Blue  = 0;
  LtdcHandle.Init.Backcolor.Green = 0;
  LtdcHandle.Init.Backcolor.Red   = 0;

  LtdcHandle.Instance = LTDC;

  /* Layer1 Configuration */
  LayerConfig.WindowX0 = 0;
  LayerConfig.WindowX1 = EW_FRAME_BUFFER_WIDTH;
  LayerConfig.WindowY0 = 0;
  LayerConfig.WindowY1 = EW_FRAME_BUFFER_HEIGHT;

  /* Pixel Format configuration: translate framebuffer color format into LTDC mode */
  #if ( EW_FRAME_BUFFER_COLOR_FORMAT == EW_FRAME_BUFFER_COLOR_FORMAT_RGBA8888 )
    LayerConfig.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  #elif ( EW_FRAME_BUFFER_COLOR_FORMAT == EW_FRAME_BUFFER_COLOR_FORMAT_RGB888 )
    LayerConfig.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
  #elif ( EW_FRAME_BUFFER_COLOR_FORMAT == EW_FRAME_BUFFER_COLOR_FORMAT_RGBA4444 )
    LayerConfig.PixelFormat = LTDC_PIXEL_FORMAT_ARGB4444;
  #elif ( EW_FRAME_BUFFER_COLOR_FORMAT == EW_FRAME_BUFFER_COLOR_FORMAT_RGB565 )
    LayerConfig.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  #elif ( EW_FRAME_BUFFER_COLOR_FORMAT == EW_FRAME_BUFFER_COLOR_FORMAT_Index8 )
    LayerConfig.PixelFormat = LTDC_PIXEL_FORMAT_L8;
  #elif ( EW_FRAME_BUFFER_COLOR_FORMAT == EW_FRAME_BUFFER_COLOR_FORMAT_LumA44 )
    LayerConfig.PixelFormat = LTDC_PIXEL_FORMAT_AL44;
  #endif

  /* Start Address will be set by Graphics Engine */
  LayerConfig.FBStartAdress = (uint32_t)EW_FRAME_BUFFER_ADDR;

  /* Alpha constant (255 totally opaque) */
  LayerConfig.Alpha = 255;

  /* Default Color configuration (configure A,R,G,B component values) */
  LayerConfig.Alpha0          = 0;
  LayerConfig.Backcolor.Blue  = 0;
  LayerConfig.Backcolor.Green = 0;
  LayerConfig.Backcolor.Red   = 0;

  /* Configure blending factors */
  LayerConfig.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  LayerConfig.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

  /* Configure the number of lines and number of pixels per line */
  LayerConfig.ImageWidth  = EW_FRAME_BUFFER_WIDTH;
  LayerConfig.ImageHeight = EW_FRAME_BUFFER_HEIGHT;

  /* Configure the LTDC */
  if ( HAL_LTDC_Init( &LtdcHandle ) != HAL_OK )
    EwPrint( "EwBspDisplayInit: Could not configure LTDC!\n" );

  /* Configure the Layer */
  if ( HAL_LTDC_ConfigLayer( &LtdcHandle, &LayerConfig, LAYER_INDEX ) != HAL_OK )
    EwPrint( "EwBspDisplayInit: Could not configure layer!\n" );

  #if EW_USE_OPERATING_SYSTEM == 1

    /* initially take the LcdUpdate token for the first LCD update */
    EwBspOsSemaphoreWait( LcdUpdateSemaphore, 1000 );

  #endif

  #if EW_USE_DOUBLE_BUFFER == 1

    /* Configure the vsync */
    if ( HAL_LTDC_ProgramLineEvent( &LtdcHandle, 0 ) != HAL_OK )
      EwPrint( "EwBspDisplayInit: Could not configure interrupt for vsync!\n" );

  #endif

  /* return the current display configuration */
  aDisplayInfo->FrameBuffer    = (void*)EW_FRAME_BUFFER_ADDR;
  aDisplayInfo->DoubleBuffer   = (void*)EW_DOUBLE_BUFFER_ADDR;
  aDisplayInfo->BufferWidth    = EW_FRAME_BUFFER_WIDTH;
  aDisplayInfo->BufferHeight   = EW_FRAME_BUFFER_HEIGHT;
  aDisplayInfo->DisplayWidth   = EW_DISPLAY_WIDTH;
  aDisplayInfo->DisplayHeight  = EW_DISPLAY_HEIGHT;

  #if EW_USE_DOUBLE_BUFFER == 1
    aDisplayInfo->UpdateMode   = EW_BSP_DISPLAY_UPDATE_NORMAL;
  #else
    aDisplayInfo->UpdateMode   = EW_BSP_DISPLAY_UPDATE_PARTIAL;
  #endif

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBspDisplayDone
*
* DESCRIPTION:
*   The function EwBspDisplayDone deinitializes the display hardware.
*
* ARGUMENTS:
*   aDisplayInfo - Display info data structure.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspDisplayDone( XDisplayInfo* aDisplayInfo )
{
  EW_UNUSED_ARG( aDisplayInfo );

  #if EW_USE_OPERATING_SYSTEM == 1

    /* destroy the LCD semaphore */
    EwBspOsSemaphoreDestroy( LcdUpdateSemaphore );

  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwBspDisplayGetUpdateArea
*
* DESCRIPTION:
*   The function EwBspDisplayGetUpdateArea returns the next update area
*   depending on the selected display mode:
*   In case of a synchroneous single-buffer, the function has to return the
*   the rectangular areas that correspond to the horizontal stripes (fields)
*   of the framebuffer.
*   In case of a scratch-pad buffer, the function has to return the subareas
*   that fit into the provided update rectangle.
*   During each display update, this function is called until it returns 0.
*
* ARGUMENTS:
*   aUpdateRect - Rectangular area which should be updated (redrawn).
*
* RETURN VALUE:
*   Returns 1 if a further update area can be provided, 0 otherwise.
*
*******************************************************************************/
int EwBspDisplayGetUpdateArea( XRect* aUpdateRect )
{
  #if EW_USE_DOUBLE_BUFFER == 0

    static int field = 0;

    /* check if last field is already reached */
    if ( field >= EW_NUMBER_OF_FIELDS )
    {
      field = 0;
      return 0;
    }

    /* determine rectangular area of current field */
    #if EW_SURFACE_ROTATION == 0
      *aUpdateRect = EwNewRect( 0, field * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS,
        EW_FRAME_BUFFER_WIDTH, ( field + 1 ) * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS );
    #endif

    #if EW_SURFACE_ROTATION == 90
      *aUpdateRect = EwNewRect( field * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS, 0,
        ( field + 1 ) * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS, EW_FRAME_BUFFER_WIDTH );
    #endif

    #if EW_SURFACE_ROTATION == 180
      *aUpdateRect = EwNewRect( 0, EW_FRAME_BUFFER_HEIGHT - ( field + 1 ) * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS,
        EW_FRAME_BUFFER_WIDTH, EW_FRAME_BUFFER_HEIGHT - field * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS );
    #endif

    #if EW_SURFACE_ROTATION == 270
      *aUpdateRect = EwNewRect( EW_FRAME_BUFFER_HEIGHT - ( field + 1 ) * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS,
        0, EW_FRAME_BUFFER_HEIGHT - field * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS, EW_FRAME_BUFFER_WIDTH );
    #endif

    /* next field */
    field++;

    /* sync on start line of next field to ensure save drawing operation */
    SyncOnLine( field * EW_FRAME_BUFFER_HEIGHT / EW_NUMBER_OF_FIELDS );

    return 1;

  #endif

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwBspDisplayWaitForCompletion
*
* DESCRIPTION:
*   The function EwBspDisplayWaitForCompletion is called from the Graphics Engine
*   to ensure that all pending activities of the display system are completed, so
*   that the rendering of the next frame can start.
*   In case of a double-buffering system, the function has to wait until the
*   V-sync has occured and the pending buffer is used by the display controller.
*   In case of an external display controller, the function has to wait until
*   the transfer (update) of the graphics data has been completed and there are
*   no pending buffers.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspDisplayWaitForCompletion( void )
{
  #if EW_USE_DOUBLE_BUFFER == 1

    register uint32_t pendingBuffer = PendingFramebuffer;
    if ( CurrentFramebuffer == pendingBuffer )
      return;

    CPU_LOAD_SET_IDLE();

    #if EW_USE_OPERATING_SYSTEM == 1

      /* wait until pending framebuffer is used as current framebuffer and
         use CPU time for other tasks */
      EwBspOsSemaphoreWait( LcdUpdateSemaphore, 1000 );
      CurrentFramebuffer = pendingBuffer;

    #else

      /* wait until pending framebuffer is used as current framebuffer */
      while( CurrentFramebuffer != pendingBuffer )
        ;

    #endif

    CPU_LOAD_SET_ACTIVE();

  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwBspDisplayCommitBuffer
*
* DESCRIPTION:
*   The function EwBspDisplayCommitBuffer is called from the Graphics Engine
*   when the rendering of a certain buffer has been completed.
*   The type of buffer depends on the selected framebuffer concept.
*   If the display is running in a double-buffering mode, the function is called
*   after each buffer update in order to change the currently active framebuffer
*   address. Changing the framebuffer address should be synchronized with V-sync.
*   If the system is using an external graphics controller, this function is
*   responsible to start the transfer of the framebuffer content.
*
* ARGUMENTS:
*   aAddress - Address of the framebuffer to be shown on the display.
*   aX,
*   aY       - Origin of the area which has been updated by the Graphics Engine.
*   aWidth,
*   aHeight  - Size of the area which has been updated by the Graphics Engine.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspDisplayCommitBuffer( void* aAddress, int aX, int aY, int aWidth, int aHeight )
{
  #if EW_USE_DOUBLE_BUFFER == 1

    /* set pending framebuffer address to be used on next V-sync */
    PendingFramebuffer = (uint32_t)aAddress;

  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwBspDisplaySetClut
*
* DESCRIPTION:
*   The function EwBspDisplaySetClut is called from the Graphics Engine
*   in order to update the hardware CLUT of the current framebuffer.
*   The function is only called when the color format of the framebuffer is
*   Index8 or LumA44.
*
* ARGUMENTS:
*   aClut - Pointer to a color lookup table with 256 entries.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspDisplaySetClut( unsigned long* aClut )
{
  if ( HAL_LTDC_ConfigCLUT( &LtdcHandle, (uint32_t*)aClut, 256, LAYER_INDEX ) != HAL_OK )
    EwPrint( "EwBspDisplaySetClut: Could not configure layer CLUT!\n" );
  if ( HAL_LTDC_EnableCLUT( &LtdcHandle, LAYER_INDEX ) != HAL_OK )
    EwPrint( "EwBspDisplaySetClut: Could not enable layer CLUT!\n" );
}


/* msy */

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
*   This template is responsible to initialize the touch driver of the display
*   hardware and to receive the touch events for the UI application.
*   Important: This file is intended to be used as a template. Please adapt the
*   implementation according your particular hardware.
*
*******************************************************************************/

#include "ewconfig.h"
#include "string.h"
#include "stm32h7xx_hal.h"
#include "stm32h750b_discovery.h"
#include "stm32h750b_discovery_bus.h"
#include "ft5336.h"

#include "ewrte.h"
#include "ewgfxdriver.h"
#include "ewextgfx.h"

#include "ew_bsp_clock.h"
#include "ew_bsp_display.h"
#include "ew_bsp_touch.h"

#define TS_I2C_ADDRESS                  0x70U
#define NO_OF_FINGERS                   FT5336_MAX_NB_TOUCH
#define DELTA_TOUCH                     16
#define DELTA_TIME                      500

/* additional touch flag to indicate idle state */
#define EW_BSP_TOUCH_IDLE               0

/* additional touch flag to indicate hold state */
#define EW_BSP_TOUCH_HOLD               4

/* structure to store internal touch information for one finger */
typedef struct
{
  int           XPos;      /* horizontal position in pixel */
  int           YPos;      /* vertical position in pixel */
  unsigned long Ticks;     /* time of recent touch event */
  unsigned char TouchId;   /* constant touch ID provided by touch controller */
  unsigned char State;     /* current state within a touch cycle */
} XTouchData;


static int           TouchAreaMinX  =  0; /* raw touch value of the leftmost position on the touch screen */
static int           TouchAreaMinY  =  0; /* raw touch value of the topmost position on the touch screen */
static int           TouchAreaMaxX  =  0; /* raw touch value of the rightmost position on the touch screen */
static int           TouchAreaMaxY  =  0; /* raw touch value of the bottommost position on the touch screen */
static int           DisplayWidth   =  0; /* width of the display */
static int           DisplayHeight  =  0; /* height of the display */
static int           IsInitalized   =  0;

static XTouchEvent   TouchEvent[ NO_OF_FINGERS ];
static XTouchData    TouchData[ NO_OF_FINGERS ];
static FT5336_Object_t        FT5336Obj;


/*******************************************************************************
* FUNCTION:
*   EwBspTouchInit
*
* DESCRIPTION:
*   Initalizes the touch driver interface. The provided display information is
*   used to configure the touch driver interface so that a proper mapping of
*   touch coordinates to GUI coordinates can be done.
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
int EwBspTouchInit( int aGuiWidth, int aGuiHeight, XDisplayInfo* aDisplayInfo )
{
  FT5336_IO_t IOCtx;
  uint32_t    id = 0;

  EW_UNUSED_ARG( aGuiWidth );
  EW_UNUSED_ARG( aGuiHeight );

  /* clear all touch state variables */
  memset( TouchData, 0, sizeof( TouchData ));
  memset( TouchEvent, 0, sizeof( TouchEvent ));

  /* check display info structure */
  if ( !aDisplayInfo )
  {
    EwPrint( "EwBspTouchInit: Invalid DisplayInfo!\n" );
    return 0;
  }

  /* take physical size of display from provided display info structure */
  DisplayWidth   = aDisplayInfo->DisplayWidth;
  DisplayHeight  = aDisplayInfo->DisplayHeight;

  /* take touch calibration values */
  TouchAreaMinX = EW_TOUCH_AREA_MIN_X;
  TouchAreaMinY = EW_TOUCH_AREA_MIN_Y;
  TouchAreaMaxX = EW_TOUCH_AREA_MAX_X;
  TouchAreaMaxY = EW_TOUCH_AREA_MAX_Y;

  /* check touch calibration and configuration to avoid division by zero */
  if (( TouchAreaMaxX == TouchAreaMinX ) || ( TouchAreaMaxY == TouchAreaMinY ))
  {
    EwPrint( "EwBspTouchInit: Invalid touch area!\n" );
    return 0;
  }

  /* Initialize the touch panel */
  IOCtx.Init        = BSP_I2C4_Init;
  IOCtx.DeInit      = BSP_I2C4_DeInit;
  IOCtx.ReadReg     = BSP_I2C4_ReadReg;
  IOCtx.WriteReg    = BSP_I2C4_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;
  IOCtx.Address     = TS_I2C_ADDRESS;

  if ( FT5336_RegisterBusIO ( &FT5336Obj, &IOCtx) != FT5336_OK || 
       FT5336_ReadID( &FT5336Obj, &id ) != FT5336_OK ||
       id != FT5336_ID ||
       FT5336_Init( &FT5336Obj ) != FT5336_OK )
  {
    EwPrint( "EwBspTouchInit: Cannot initialize touch panel!\n" );
    return 0;
  }

  #ifdef EW_PRINT_TOUCH_DATA

    EwPrint( "\n" );
    EwPrint( "EwBspTouchInit: Using TouchArea %d, %d - %d, %d\n", TouchAreaMinX, TouchAreaMinY, TouchAreaMaxX, TouchAreaMaxY );

  #endif

  IsInitalized = 1;
  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBspTouchDone
*
* DESCRIPTION:
*   Terminates the touch driver.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspTouchDone( void )
{
  IsInitalized = 0;
}


/*******************************************************************************
* FUNCTION:
*   EwBspTouchGetEvents
*
* DESCRIPTION:
*   The function EwBspTouchGetEvents reads the current touch positions from the
*   touch driver and returns the current touch position and touch status of the
*   different fingers. The returned number of touch events indicates the number
*   of XTouchEvent that contain position and status information.
*   The orientation of the touch positions is adjusted to match GUI coordinates.
*   If the hardware supports only single touch, the finger number is always 0.
*
* ARGUMENTS:
*   aTouchEvent - Pointer to return array of XTouchEvent.
*
* RETURN VALUE:
*   Returns the number of detected touch events, otherwise 0.
*
*******************************************************************************/
int EwBspTouchGetEvents( XTouchEvent** aTouchEvent )
{
  FT5336_MultiTouch_State_t      touchPadState;
  int           touchX;
  int           touchY;
  int           x, y;
  int           t;
  int           f;
  unsigned long ticks;
  int           noOfEvents = 0;
  int           finger;
  char          identified[ NO_OF_FINGERS ];
  XTouchData*   touch;
  int32_t       ret;


  if ( !IsInitalized )
    return 0;

  /* access touch driver to receive current touch status and position */
  CPU_LOAD_SET_IDLE();
  ret = FT5336_GetMultiTouchState( &FT5336Obj, &touchPadState );

  CPU_LOAD_SET_ACTIVE();

  /* all fingers have the state unidentified */
  memset( identified, 0, sizeof( identified ));

  /* get current time in ms */
  ticks = EwGetTicks();

  /* iterate through all touch events from the hardware */
  if ( ret == FT5336_OK )
    for ( t = 0; t < touchPadState.TouchDetected; t++ )
  {
    #ifdef EW_PRINT_TOUCH_DATA

      EwPrint( "Raw touch data: id %d, x %d, y %d\n", touchPadState.TouchId[ t ], touchPadState.TouchX[ t ], touchPadState.TouchY[ t ] );

    #endif

    /* apply swapping of raw touch coordinates if required */
    #if ( EW_TOUCH_SWAP_XY )

      touchX = touchPadState.TouchY[ t ];
      touchY = touchPadState.TouchX[ t ];

    #else

      touchX = touchPadState.TouchX[ t ];
      touchY = touchPadState.TouchY[ t ];

    #endif

    /* convert raw touch coordinates into display coordinates */
    touchX = (( touchX - TouchAreaMinX ) * DisplayWidth ) / ( TouchAreaMaxX - TouchAreaMinX );
    touchY = (( touchY - TouchAreaMinY ) * DisplayHeight ) / ( TouchAreaMaxY - TouchAreaMinY );

    /* check for valid display coordinates */
    if (( touchX < 0 ) || ( touchX > DisplayWidth ) ||
        ( touchY < 0 ) || ( touchY > DisplayHeight ))
      continue;

    /* convert display coordinates into GUI coordinates */
    #if ( EW_SURFACE_ROTATION == 90 )

      x = touchY;
      y = DisplayWidth - touchX;

    #elif ( EW_SURFACE_ROTATION == 270 )

      x = DisplayHeight - touchY;
      y = touchX;

    #elif ( EW_SURFACE_ROTATION == 180 )

      x = DisplayWidth - touchX;
      y = DisplayHeight - touchY;

    #else

      x = touchX;
      y = touchY;

    #endif

    #ifdef EW_PRINT_TOUCH_DATA

      EwPrint( "GUI coordinates: x %d, y %d\n", x, y );

    #endif

    /* Important note: The touch driver does not provde down/up status information - the current
       phase within the touch cycle has to be determined by the software */
    /* iterate through all fingers to find a finger that matches with the provided touch event */
    for ( finger = -1, f = 0; f < NO_OF_FINGERS; f++ )
    {
      touch = &TouchData[ f ];

      /* check if the finger is already active */
      if (( touch->State != EW_BSP_TOUCH_IDLE ) && ( touch->TouchId == touchPadState.TouchId[ t ]))
      {
        finger = f;
        break;
      }

      /* check if the finger was used within the recent time span and if the touch position is in the vicinity */
      if (( touch->State == EW_BSP_TOUCH_IDLE ) && ( ticks < touch->Ticks + DELTA_TIME )
        && ( x > touch->XPos - DELTA_TOUCH ) && ( x < touch->XPos + DELTA_TOUCH )
        && ( y > touch->YPos - DELTA_TOUCH ) && ( y < touch->YPos + DELTA_TOUCH ))
        finger = f;

      /* otherwise take the first free finger */
      if (( touch->State == EW_BSP_TOUCH_IDLE ) && ( finger == -1 ))
        finger = f;
    }

    /* determine the state within a touch cycle and assign the touch parameter to the found finger */
    if ( finger >= 0 )
    {
      touch = &TouchData[ finger ];
      identified[ finger ] = 1;

      /* check for start of touch cycle */
      if ( touch->State == EW_BSP_TOUCH_IDLE )
        touch->State = EW_BSP_TOUCH_DOWN;
      else
      {
        /* check if the finger has moved */
        if (( touch->XPos != x ) || ( touch->YPos != y ))
          touch->State = EW_BSP_TOUCH_MOVE;
        else
          touch->State = EW_BSP_TOUCH_HOLD;
      }

      /* store current touch parameter */
      touch->XPos    = x;
      touch->YPos    = y;
      touch->TouchId = touchPadState.TouchId[ t ];
      touch->Ticks   = ticks;
    }
  }

  /* prepare sequence of touch events suitable for Embedded Wizard GUI application */
  for ( f = 0; f < NO_OF_FINGERS; f++ )
  {
    touch = &TouchData[ f ];

    /* begin of a touch cycle */
    if ( identified[ f ] && ( touch->State == EW_BSP_TOUCH_DOWN ))
      TouchEvent[ noOfEvents ].State = EW_BSP_TOUCH_DOWN;

    /* move within a touch cycle */
    else if ( identified[ f ] && ( touch->State == EW_BSP_TOUCH_MOVE ))
      TouchEvent[ noOfEvents ].State = EW_BSP_TOUCH_MOVE;

    /* end of a touch cycle */
    else if ( !identified[ f ] && ( touch->State != EW_BSP_TOUCH_IDLE ))
    {
      TouchEvent[ noOfEvents ].State = EW_BSP_TOUCH_UP;
      touch->State = EW_BSP_TOUCH_IDLE;
    }
    else
      continue;

    TouchEvent[ noOfEvents ].XPos   = touch->XPos;
    TouchEvent[ noOfEvents ].YPos   = touch->YPos;
    TouchEvent[ noOfEvents ].Finger = f;

    #ifdef EW_PRINT_TOUCH_EVENTS

      EwPrint( "Touch event for finger %d with state %d ( %4d, %4d )\n", f, TouchEvent[ noOfEvents ].State, TouchEvent[ noOfEvents ].XPos, TouchEvent[ noOfEvents ].YPos );

    #endif

    noOfEvents++;
  }

  /* return the prepared touch events and the number of prepared touch events */
  if ( aTouchEvent )
    *aTouchEvent = TouchEvent;

  return noOfEvents;
}


/* msy, mli */

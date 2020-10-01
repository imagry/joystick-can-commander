/**
 * @file joystick.c
 * @brief Joystick Interface Source
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>

#include "joystick.h"

/**
 * @brief Button press debounce delay. [microseconds]
 *
 */
#define BUTTON_PRESSED_DELAY (5000)

/**
 * @brief Invalid \ref joystick_device_s.controller value
 *
 */
#define JOYSTICK_DEVICE_CONTROLLER_INVALID ( NULL )

/**
 * @brief Joystick Identifier Data
 *
 */
#define JOYSTICK_ID_DATA_SIZE ( 16 )

/**
 * @brief Joystick Description String
 *
 */
#define JOYSTICK_ID_STRING_SIZE ( 64 )

typedef struct
{
    unsigned char data[ JOYSTICK_ID_DATA_SIZE ];
    char ascii_string[ JOYSTICK_ID_STRING_SIZE ];

} joystick_guid_s;

typedef struct
{
    void *controller;

    void *haptic;

    joystick_guid_s* guid;

} joystick_device_data_s;


static joystick_guid_s joystick_guid;
static joystick_device_data_s joystick_data = {
    .controller = NULL,
    .haptic = NULL,
    .guid = &joystick_guid
    };

static joystick_device_data_s* joystick = NULL;

static int joystick_init_subsystem( )
{
    int ret = 0;

    if ( joystick == NULL )
    {
        int init_result = SDL_Init( SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC );

        ret = 0;

        if ( init_result < 0 )
        {
            printf( "OSCC_ERROR: SDL_Init - %s\n", SDL_GetError() );
            ret = -1;
        }
    }
    return ret;
}

static int joystick_get_guid_at_index( unsigned long device_index )
{
    int ret = 0;

    if ( joystick != NULL )
    {
        ret = 0;

        const SDL_JoystickGUID m_guid =
            SDL_JoystickGetDeviceGUID( (int) device_index );

        memcpy( joystick_guid.data, m_guid.data, sizeof( m_guid.data ) );

        memset( joystick_guid.ascii_string, 0,
                sizeof( joystick_guid.ascii_string ) );

        SDL_JoystickGetGUIDString( m_guid,
                                   joystick_guid.ascii_string,
                                   sizeof( joystick_guid.ascii_string ) );
    }
    return ret;
}

static int joystick_get_num_devices( )
{
    int num_joysticks = 0;

    if ( joystick != NULL )
    {
        num_joysticks = SDL_NumJoysticks();

        if ( num_joysticks < 0 )
        {
            printf( "OSCC_ERROR: SDL_NumJoysticks - %s\n", SDL_GetError() );
            num_joysticks = -1;
        }
    }
    return ( num_joysticks );
}

int joystick_init( )
{
    int ret = 0;

    ret = joystick_init_subsystem();

    if ( ret == -1 )
    {
        printf("init subsystem error\n");
    }
    else
    {
        joystick = &joystick_data;
        joystick->controller = JOYSTICK_DEVICE_CONTROLLER_INVALID;

        const int num_joysticks = joystick_get_num_devices();

        if ( num_joysticks > 0 )
        {
            unsigned long device_index = 0;

            ret = joystick_get_guid_at_index( device_index );

            if ( ret == 0 )
            {
//                printf( "Found %d devices -- connecting to device at system index %lu - GUID: %s\n",
//                        num_joysticks,
//                        device_index,
//                        joystick_guid.ascii_string );
                char mappings [][1024] = {
                        "050000004c050000cc09000000810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
                        "030000004c050000c405000011010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
                        "050000004c050000c405000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
                        "030000004c050000cc09000011010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
                        "050000004c050000cc09000000010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
                        "030000004c050000a00b000011010000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,",
                        "030000004c050000c405000011810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
                        "050000004c050000c405000000810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
                        "030000004c050000cc09000011810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
                        "050000004c050000cc09000000810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,",
                        "030000004c050000a00b000011810000,PS4 Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b10,leftshoulder:b4,leftstick:b11,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b12,righttrigger:a5,rightx:a3,righty:a4,start:b9,x:b3,y:b2,"
                };
		int i;
                for (i = 0; i< sizeof(mappings); i++) {
//                    printf("Trying to configure mapping %s\n", mappings[i]);
                    int res = SDL_GameControllerAddMapping(mappings[i]);

                    if (res == -1)
                    {
                        printf("Failed to configure new mapping\n");
                        ret = -1;
                    }
                    else
                    {
//                        printf("Mapping configured successfully. Trying to connect controller\n");
                        ret = joystick_open( device_index );

                        if (ret == 0)
                        {
                            printf("Joystick connected successfully\n");
                            break;
                        }
                    }
                }


            }
        }
        else
        {
            printf( "No joystick/devices available on the host\n" );
            ret = -1;
        }
    }

    return ret;

}

int joystick_open( unsigned long device_index )
{
    int ret = -1;

    if ( joystick != NULL )
    {
        joystick->controller =
            (void*) SDL_GameControllerOpen( (int) device_index );

        if ( joystick->controller != JOYSTICK_DEVICE_CONTROLLER_INVALID )
        {
            ret = 0;

            const SDL_JoystickGUID m_guid =
                SDL_JoystickGetGUID(
                    SDL_GameControllerGetJoystick( joystick->controller ) );

            memcpy( joystick_guid.data, m_guid.data, sizeof( m_guid.data ) );

            memset( joystick_guid.ascii_string,
                    0,
                    sizeof( joystick_guid.ascii_string ) );

            SDL_JoystickGetGUIDString( m_guid,
                                       joystick_guid.ascii_string,
                                       sizeof( joystick_guid.ascii_string ) );

            joystick->haptic =
                (void*) SDL_HapticOpenFromJoystick(
                    SDL_GameControllerGetJoystick( joystick->controller ));

            if ( SDL_HapticRumbleInit( joystick->haptic ) != 0 )
            {
                SDL_HapticClose( joystick->haptic );
            }
        }
    }
    return ret;
}

void joystick_close( )
{
    if ( joystick != NULL )
    {
        if ( joystick->controller != JOYSTICK_DEVICE_CONTROLLER_INVALID )
        {
            if ( SDL_GameControllerGetAttached( joystick->controller ) ==            SDL_TRUE )
            {
                if ( joystick->haptic )
                {
                    SDL_HapticClose( joystick->haptic );
                }
                SDL_GameControllerClose( joystick->controller );
            }

            joystick->controller = JOYSTICK_DEVICE_CONTROLLER_INVALID;
        }
        joystick = NULL;
    }
    // Release the joystick subsystem
    SDL_Quit();
}

int joystick_update( )
{
    int ret = -1;

    if ( joystick != NULL )
    {
        if ( joystick->controller != JOYSTICK_DEVICE_CONTROLLER_INVALID )
        {
            SDL_GameControllerUpdate();

            if ( SDL_GameControllerGetAttached( joystick->controller ) ==            SDL_FALSE )
            {
                printf("SDL_GameControllerGetAttached - device not attached\n");
            }
            else
            {
                ret = 0;
            }
        }
    }
    return ret;
}

int joystick_get_axis( unsigned long axis_index, int * const position )
{
    int ret = -1;

    if ( ( joystick  != NULL ) && ( position != NULL ) )
    {
        ret = 0;

        const Sint16 pos = SDL_GameControllerGetAxis( joystick->controller,
                                                      axis_index );


        ( *position ) = (int) pos;
    }

    return ret;
}

int joystick_get_button( unsigned long button_index,
                         unsigned int * const button_state )
{
    int ret = -1;

    if ( ( joystick  != NULL ) && ( button_state != NULL ) )
    {
        ret = 0;

        const Uint8 m_state = SDL_GameControllerGetButton( joystick->controller,
                                                           button_index );

        if ( m_state == 1 )
        {
            ( *button_state ) = JOYSTICK_BUTTON_STATE_PRESSED;

            if ( joystick->haptic )
            {
                SDL_HapticRumblePlay( joystick->haptic, 1.0f, 100 );
            }

            ( void ) usleep( BUTTON_PRESSED_DELAY );
        }
        else
        {
            ( *button_state ) = JOYSTICK_BUTTON_STATE_NOT_PRESSED;
        }
    }

    return ret;
}

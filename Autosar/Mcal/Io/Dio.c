/**
 * @file    Dio.c
 * @brief   **Specification of Dio Driver**
 *
 * This specification specifies the functionality, API and the configuration of the AUTOSAR Basic
 * Software module DIO Driver. This specification is applicable to drivers only for on chip DIO
 * pins and ports. The DIO Driver provides services for reading and writing to/from.
 *   - DIO Channels (Pins)
 *   - DIO Ports
 *   - DIO Channel Groups The behavior of those services is synchronous.
 *
 * This module works on pins and ports which are configured by the PORT driver for this purpose.
 * For this reason, there is no configuration and initialization of this port structure in the DIO Driver
 */
#include "Std_Types.h"
#include "Registers.h"
#include "Dio.h"
#include "Bfx.h"


/**
 * @brief  **Returns the value of the specified DIO channel**
 *
 * The function returns the logic pin state of the port pin specified by the parameter
 * #DioChannelType
 *
 * @param  ChannelId ID of DIO channel, LSBs represent the pin while the 2MSBs the Port
 *
 * @retval STD_HIGH The physical level of the corresponding Pin is STD_HIGH
 *         STD_LOW The physical level of the corresponding Pin is STD_LOW
 *
 * @code
 *    Dio_LevelType Level = Dio_ReadChannel( DIO_RED_LED );
 * @endcode
 */
Dio_LevelType Dio_ReadChannel( Dio_ChannelType ChannelId )
{
    Dio_PortType Port               = ChannelId;
    uint8 Pin                       = ChannelId;
    Dio_RegisterType *DiosBaseAddrs = DIOA;

    Bfx_ClrBitMask_u8u8( &Pin, 0xf0u );
    Bfx_ShiftBitRt_u8u8( &Port, 4u );

    return Bfx_GetBit_u32u8_u8( DiosBaseAddrs[ Port ].IDR, Pin );
}


/**
 * @brief  **Service to set a level of a channel**
 *
 * The function set or reset the specified pin #DioChannelType
 *
 * @param  ChannelId ID of DIO channel, LSBs represent the pin while the 2MSBs the Port
 * @param  Level Value to be written STD_LOW or STD_HIGH
 *
 * @code
 *      Dio_WriteChannel( DIO_RED_LED, STD_LOW );
 * @endcode
 */
void Dio_WriteChannel( Dio_ChannelType ChannelId, Dio_LevelType Level )
{
    Dio_PortType Port               = ChannelId;
    uint8 Pin                       = ChannelId;
    Dio_RegisterType *DiosBaseAddrs = DIOA;

    Bfx_ClrBitMask_u8u8( &Pin, 0xf0u );
    Bfx_ShiftBitRt_u8u8( &Port, 4u );

    Bfx_PutBit_u32u8u8( (uint32 *)&DiosBaseAddrs[ Port ].ODR, Pin, Level );
}


/**
 * @brief  **Returns the level of all channels of that port**
 *
 * Read an entire 16 bit micrcontroller port from PORTA (0) to PortF (5)
 *
 * @param  PortId Port to read
 *
 * @retval Dio_PortLevelType Level of all channels of that port
 *
 * @code
 *      Dio_PortLevel Value = Dio_ReadPort( DIO_PORT_PA );
 * @endcode
 */
Dio_PortLevelType Dio_ReadPort( Dio_PortType PortId )
{
    Dio_RegisterType *DiosBaseAddrs = DIOA;

    return DiosBaseAddrs[ PortId ].IDR;
}


/**
 * @brief  **Service to set a value of the port**
 *
 * Read an entire 16 bit micrcontroller port from PORTA (0) to PortF (5)
 *
 * @param  PortId ID of DIO channel
 * @param  Level Value to be written
 *
 * @code
 *      Dio_WritePort( DIO_PORT_A, 0xAA55 );
 * @endcode
 */
void Dio_WritePort( Dio_PortType PortId, Dio_PortLevelType Level )
{
    Dio_RegisterType *DiosBaseAddrs = DIOA;

    DiosBaseAddrs[ PortId ].ODR = Level;
}


/**
 * @brief  **This Service reads a subset of the adjoining bits of a port**
 *
 * Read a specific set of adjoining bits of the port selected, the pointer to structure @ChannelGroupIdPtr
 * contains the port, bits to read and the offset from where the group of bits starts
 *
 * @param  ChannelGroupIdPtr Pointer to ChannelGroup
 *
 * @retval Dio_PortLevelType Level of a subset of the adjoining bits of a port
 *
 * @code
 *      Dio_ChannelGroupType LedBar =
 *      {
 *          .Mask = 0x00ffu,
 *          .Offset = 2u,
 *          .Port = DIO_PORT_LEDS
 *      }
 *
 *      Dio_PortLevelType Leds = Dio_ReadChannelGroup( &LedBar );
 * @endcode
 */
Dio_PortLevelType Dio_ReadChannelGroup( const Dio_ChannelGroupType *ChannelGroupIdPtr )
{
    Dio_RegisterType *DiosBaseAddrs = DIOA;
    Dio_PortLevelType GroupValue    = DiosBaseAddrs[ ChannelGroupIdPtr->Port ].IDR;

    Bfx_ClrBitMask_u32u32( &GroupValue, ~ChannelGroupIdPtr->Mask );
    Bfx_ShiftBitRt_u32u8( &GroupValue, ChannelGroupIdPtr->Offset );

    return GroupValue;
}


/**
 * @brief  **Service to set a subset of the adjoining bits of a port to a specified level**
 *
 * Write a value to a specific set of adjoining bits of the port selected, the pointer to structure
 * @ChannelGroupIdPtr contains the port, bits to write and the offset from where the group of bits starts
 *
 * @param  ChannelGroupIdPtr Pointer to ChannelGroup
 * @param  Level Value to be written
 *
 * @code
 *      Dio_ChannelGroupType LedBar =
 *      {
 *          .Mask = 0x00ffu,
 *          .Offset = 2u,
 *          .Port = DIO_PORT_LEDS
 *      }
 *
 *      Dio_ReadChannelGroup( &LedBar, 0x55 );
 * @endcode
 */
void Dio_WriteChannelGroup( const Dio_ChannelGroupType *ChannelGroupIdPtr, Dio_PortLevelType Level )
{
    Dio_RegisterType *DiosBaseAddrs = DIOA;

    Bfx_ClrBitMask_u32u32( (uint32 *)&DiosBaseAddrs[ ChannelGroupIdPtr->Port ].ODR, ChannelGroupIdPtr->Mask );
    Bfx_ShiftBitLt_u32u8( &Level, ChannelGroupIdPtr->Offset );
    Bfx_SetBitMask_u32u32( (uint32 *)&DiosBaseAddrs[ ChannelGroupIdPtr->Port ].ODR, Level );
}


/**
 * @brief  Service to flip (change from 1 to 0 or from 0 to 1) the level of a channel and return
 *
 * The function toggle the logic pin state of the port pin specified by the parameter
 * @DioChannelType
 *
 * @param  ChannelId ID of DIO channel, LSBs represent the pin while the 2MSBs the Port
 *
 * @retval STD_HIGH The physical level of the corresponding Pin is STD_HIGH
 *         STD_LOW The physical level of the corresponding Pin is STD_LOW
 *
 * @code
 *      Dio_LevelType Led = Dio_FlipChannel( DIO_RED_LED )
 * @endcode
 */
Dio_LevelType Dio_FlipChannel( Dio_ChannelType ChannelId )
{
    Dio_PortType Port               = ChannelId;
    uint8 Pin                       = ChannelId;
    Dio_RegisterType *DiosBaseAddrs = DIOA;

    Bfx_ClrBitMask_u8u8( &Pin, 0xf0u );
    Bfx_ShiftBitRt_u8u8( &Port, 4u );

    Bfx_ToggleBitMask_u32u32( (uint32 *)&DiosBaseAddrs[ Port ].ODR, ( 1u << Pin ) );

    return Bfx_GetBit_u32u8_u8( DiosBaseAddrs[ Port ].IDR, Pin );
}


/**
 * @brief  Service to set the value of a given port with required mask.
 *
 * The function shall set the specified value for the channels in the specified port if the
 * corresponding bit in Mask is '1'.
 *
 * @param  PortId ID of DIO port
 * @param  Level Value to be written
 * @param  Mask Channels to be masked in the port
 *
 * @code
 *      Dio_MaskedWritePort( DIO_PORT_LEDS, 0xffu, 0x55 )
 * @endcode
 */
void Dio_MaskedWritePort( Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask )
{
    Dio_RegisterType *DiosBaseAddrs = DIOA;

    Bfx_PutBitsMask_u32u32u32( (uint32 *)&DiosBaseAddrs[ PortId ].ODR, Level, Mask );
}

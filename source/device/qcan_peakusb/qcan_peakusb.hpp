//============================================================================//
// File:          qcan_peakusb.hpp                                            //
// Description:   CAN plugin for PEAK USB device                              //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53842 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//============================================================================//

#ifndef QCAN_PEAK_USB_H_
#define QCAN_PEAK_USB_H_

#include <QObject>
#include <QtPlugin>
#include <QLibrary>
#include <QCanInterface>

#ifdef Q_OS_WIN32
#include <windows.h>
#ifdef DRV_CALLBACK_TYPE
#define DRV_CALLBACK_TYPE WINAPI
#endif
#else
#define DRV_CALLBACK_TYPE
#endif

#ifndef __APPLE__
#include "PCANBasic.h"
#define  QCAN_SUPPORT_CAN_FD     1
#define  QCAN_PEAKLIB            "PCANBasic.dll"
#else
#include "PCBUSB.h"
#define  QCAN_SUPPORT_CAN_FD     0
#define  QCAN_PEAKLIB            "libPCBUSB.0.6.dylib"
#endif

//----------------------------------------------------------------------------//
// QCanPeakUsb                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
class QCanPeakUsb : public QCanInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.microcontrol.Qt.qcan.QCanInterface" FILE "plugin.json")
    Q_INTERFACES(QCanInterface)

private:

   //----------------------------------------------------------------
   // Type definitions of PCANBasic.dll taken from PCANBasic.h
   // (Last Change 24.04.2015)
   //
   typedef TPCANStatus (*CAN_Initialize_tf)     (TPCANHandle uwChannelV, TPCANBaudrate uwBtr0Btr1V, TPCANType ubHwTypeV, DWORD ulIOPortV, WORD uwInterruptV);
   #if QCAN_SUPPORT_CAN_FD > 0
   typedef TPCANStatus (*CAN_InitializeFD_tf)   (TPCANHandle uwChannelV, TPCANBitrateFD pszBitrateFDV);
   #endif
   typedef TPCANStatus (*CAN_Uninitialize_tf)   (TPCANHandle uwChannelV);
   typedef TPCANStatus (*CAN_Reset_tf)          (TPCANHandle uwChannelV);
   typedef TPCANStatus (*CAN_GetStatus_tf)      (TPCANHandle uwChannelV);
   typedef TPCANStatus (*CAN_Read_tf)           (TPCANHandle uwChannelV, TPCANMsg *ptsMessageBufferV, TPCANTimestamp *tsTimestampBufferV);
   #if QCAN_SUPPORT_CAN_FD > 0
   typedef TPCANStatus (*CAN_ReadFD_tf)         (TPCANHandle uwChannelV, TPCANMsgFD *ptsMessageBufferV, TPCANTimestampFD *puqTimestampBufferV);
   #endif
   typedef TPCANStatus (*CAN_Write_tf)          (TPCANHandle uwChannelV, TPCANMsg *ptsMessageBufferV);
   #if QCAN_SUPPORT_CAN_FD > 0
   typedef TPCANStatus (*CAN_WriteFD_tf)        (TPCANHandle uwChannelV, TPCANMsgFD *ptsMessageBufferV);
   #endif
   typedef TPCANStatus (*CAN_FilterMessages_tf) (TPCANHandle uwChannelV, DWORD ulFromIDV, DWORD ulToIDV, TPCANMode ubModeV);
   typedef TPCANStatus (*CAN_GetValue_tf)       (TPCANHandle uwChannelV, TPCANParameter ubParameterV, void *pvdBufferV, DWORD ulBufferLengthV);
   typedef TPCANStatus (*CAN_SetValue_tf)       (TPCANHandle uwChannelV, TPCANParameter ubParameterV, void *pvdBufferV, DWORD ulBufferLengthV);
   typedef TPCANStatus (*CAN_GetErrorText_tf)   (TPCANStatus ulErrorV, WORD uwLanguageV, LPSTR pszBufferV);


   CAN_Initialize_tf      pfnCAN_InitializeP;
   #if QCAN_SUPPORT_CAN_FD > 0
   CAN_InitializeFD_tf    pfnCAN_InitializeFDP;
   #endif
   CAN_Uninitialize_tf    pfnCAN_UninitializeP;
   CAN_Reset_tf           pfnCAN_ResetP;
   CAN_GetStatus_tf       pfnCAN_GetStatusP;
   CAN_Read_tf            pfnCAN_ReadP;
   #if QCAN_SUPPORT_CAN_FD > 0
   CAN_ReadFD_tf          pfnCAN_ReadFDP;
   #endif
   CAN_Write_tf           pfnCAN_WriteP;
   #if QCAN_SUPPORT_CAN_FD > 0
   CAN_WriteFD_tf         pfnCAN_WriteFDP;
   #endif
   CAN_FilterMessages_tf  pfnCAN_FilterMessagesP;
   CAN_GetValue_tf        pfnCAN_GetValueP;
   CAN_SetValue_tf        pfnCAN_SetValueP;
   CAN_GetErrorText_tf    pfnCAN_GetErrorTextP;

   /*!
    * \brief clCanLibP
    */
   QLibrary clCanLibP;


   /*!
    * \brief uwCanChannelP
    */
   uint16_t uwCanChannelP = PCAN_USBBUS1;

   bool     btLibFuncLoadP = false;

   /*!
    * \brief clStatisticP
    */
   QCanStatistic_ts clStatisticP;

public:
   QString echo(const QString &message) Q_DECL_OVERRIDE;

   InterfaceError_e  connect(uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   void              disconnect(uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   QIcon             icon(uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;
   QString           name(uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   InterfaceError_e  read( QCanFrame &clFrameR,
                           uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   InterfaceError_e  setBitrate( int32_t slBitrateV,
                                 int32_t slBrsClockV,
                                 uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   InterfaceError_e  setMode( const QCan::CAN_Mode_e teModeV,
                              uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   QCan::CAN_State_e state(uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   InterfaceError_e  statistic(QCanStatistic_ts &clStatisticR,
                               uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   uint32_t          supportedFeatures(uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

   InterfaceError_e  write(const QCanFrame &clFrameR,
                           uint8_t ubChannelV = 0) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void errorOccurred(int32_t slCanBusErrorV);
};

#endif /*QCAN_PEAK_USB_H_*/

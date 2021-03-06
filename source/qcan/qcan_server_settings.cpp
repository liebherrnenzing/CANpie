//====================================================================================================================//
// File:          qcan_server_settings.cpp                                                                            //
// Description:   QCAN classes - CAN server                                                                           //
//                                                                                                                    //
// Copyright (C) MicroControl GmbH & Co. KG                                                                           //
// 53844 Troisdorf - Germany                                                                                          //
// www.microcontrol.net                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the   //
// following conditions are met:                                                                                      //
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions, the following   //
//    disclaimer and the referenced file 'LICENSE'.                                                                   //
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       //
//    following disclaimer in the documentation and/or other materials provided with the distribution.                //
// 3. Neither the name of MicroControl nor the names of its contributors may be used to endorse or promote products   //
//    derived from this software without specific prior written permission.                                           //
//                                                                                                                    //
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance     //
// with the License. You may obtain a copy of the License at                                                          //
//                                                                                                                    //
//    http://www.apache.org/licenses/LICENSE-2.0                                                                      //
//                                                                                                                    //
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   //
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for  //
// the specific language governing permissions and limitations under the License.                                     //
//                                                                                                                    //
//====================================================================================================================//


/*--------------------------------------------------------------------------------------------------------------------*\
** Include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#include <QtCore/QDebug>
#include <QtCore/QDateTime>

#include "qcan_server_memory.hpp"
#include "qcan_server_settings.hpp"


/*--------------------------------------------------------------------------------------------------------------------*\
** Definitions                                                                                                        **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/

#define  MAXIMUM_SERVER_TIME_DIFFERENCE      ((int64_t) 2000)



//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::QCanServerSettings()                                                                           //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::QCanServerSettings()
{
   pclSettingsP = new QSharedMemory(QString(QCAN_MEMORY_KEY));
   pclSettingsP->attach(QSharedMemory::ReadOnly);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::~QCanServerSettings()                                                                          //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::~QCanServerSettings()
{
   pclSettingsP->detach();
   delete (pclSettingsP);
}


//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::checkState()                                                                                   //
// return the current state of the QCan server                                                                        //
//--------------------------------------------------------------------------------------------------------------------//
QCanServerSettings::State_e QCanServerSettings::state(void)
{
   State_e  teServerStateT = eSTATE_UNKNOWN;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      if ( (QDateTime::currentMSecsSinceEpoch() - (ptsSettingsT->tsServer.sqDateTimeActual)) > MAXIMUM_SERVER_TIME_DIFFERENCE)
      {
         teServerStateT = eSTATE_CRASHED;
      }
      else
      {
         teServerStateT = eSTATE_ACTIVE;
      }
   }
   else
   {
      teServerStateT = eSTATE_INACTIVE;
   }

   return (teServerStateT);
}





//--------------------------------------------------------------------------------------------------------------------//
// networkCount()                                                                                                     //
// return number of supported CAN networks                                                                            //
//--------------------------------------------------------------------------------------------------------------------//
int QCanServerSettings::networkCount(void)
{
   int32_t  slCountT = 0;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      slCountT = ptsSettingsT->tsServer.slNetworkCount;
   }

   return (slCountT);
}

/*
//--------------------------------------------------------------------------------------------------------------------//
// networkDataBitrate()                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkDataBitrate(const CAN_Channel_e teChannelV)
{
   QString  clDatBitRateT;

   if ( (teChannelV > eCAN_CHANNEL_NONE) && (teChannelV <= QCAN_NETWORK_MAX))
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      if (ptsSettingsT->atsNetwork[teChannelV - 1].slDatBitRate == eCAN_BITRATE_NONE)
      {
         clDatBitRateT = "None";
      }
      else
      {
         if (ptsSettingsT->atsNetwork[teChannelV - 1].slDatBitRate < 1000000)
         {
            //------------------------------------------------
            // print values < 1000000 in kBit/s
            //
            clDatBitRateT = QString("%1 kBit/s").arg(ptsSettingsT->atsNetwork[teChannelV - 1].slDatBitRate / 1000);
         }
         else
         {
            //------------------------------------------------
            // print values >= 1000000 in MBit/s
            //
            clDatBitRateT = QString("%1 MBit/s").arg(ptsSettingsT->atsNetwork[teChannelV - 1].slDatBitRate / 1000000);
         }
      }

   }


   return (clDatBitRateT);
}


//--------------------------------------------------------------------------------------------------------------------//
// networkName()                                                                                                      //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkName(const CAN_Channel_e teChannelV)
{
   QString clResultT;

   if ( (teChannelV > eCAN_CHANNEL_NONE) && (teChannelV <= QCAN_NETWORK_MAX))
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      //-------------------------------------------------------------------------------------------
      // calculate the size of the interface name, a name
      //
      int slSizeT = strlen(ptsSettingsT->atsNetwork[teChannelV - 1].szInterfaceName);
      if (slSizeT > QCAN_IF_NAME_LENGTH)
      {
         slSizeT = QCAN_IF_NAME_LENGTH;
      }

      clResultT = QString::fromLatin1(&(ptsSettingsT->atsNetwork[teChannelV - 1].szInterfaceName[0]), slSizeT);
   }

   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// networkNominalBitrate()                                                                                            //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::networkNominalBitrate(const CAN_Channel_e teChannelV)
{
   QString  clNomBitRateT;

   if ( (teChannelV > eCAN_CHANNEL_NONE) && (teChannelV <= QCAN_NETWORK_MAX))
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();

      if (ptsSettingsT->atsNetwork[teChannelV - 1].slNomBitRate == eCAN_BITRATE_NONE)
      {
         clNomBitRateT = "None";
      }
      else
      {
         //------------------------------------------------
         // print values < 1000000 in kBit/s
         //
         clNomBitRateT = QString("%1 kBit/s").arg(ptsSettingsT->atsNetwork[teChannelV - 1].slNomBitRate / 1000);
      }
   }

   return (clNomBitRateT);
}
*/

//--------------------------------------------------------------------------------------------------------------------//
// QCanServerSettings::state()                                                                                        //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
QString QCanServerSettings::stateString(void)
{
   QString  clResultT;

   switch (state())
   {
      case eSTATE_CRASHED:
         clResultT = QObject::tr("is crashed");
         break;

      case eSTATE_INACTIVE:
         clResultT = QObject::tr("is not active");
         break;

      case eSTATE_ACTIVE:
         clResultT = QObject::tr("is active");
         break;

      default:
         clResultT = QObject::tr("state is unknown");
         break;

   }

   return (clResultT);
}


//--------------------------------------------------------------------------------------------------------------------//
// versionBuild()                                                                                                     //
// return build version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int QCanServerSettings::versionBuild(void)
{
   int32_t  slVersionT = -1;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      slVersionT = ptsSettingsT->tsServer.slVersionBuild;
   }

   return (slVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// versionMajor()                                                                                                     //
// return major version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int QCanServerSettings::versionMajor(void)
{
   int32_t  slVersionT = -1;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      slVersionT = ptsSettingsT->tsServer.slVersionMajor;
   }

   return (slVersionT);
}


//--------------------------------------------------------------------------------------------------------------------//
// versionMinor()                                                                                                     //
// return minor version                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//
int32_t QCanServerSettings::versionMinor(void)
{
   int32_t  slVersionT = -1;

   if (pclSettingsP->isAttached())
   {
      ServerSettings_ts * ptsSettingsT = (ServerSettings_ts *) pclSettingsP->data();
      slVersionT = ptsSettingsT->tsServer.slVersionMinor;
   }

   return (slVersionT);
}


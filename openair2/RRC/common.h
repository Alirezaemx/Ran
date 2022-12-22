/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#ifndef __OPENAIR_RRC_COMMON_H___
#define __OPENAIR_RRC_COMMON_H___
static const uint16_t T300[8] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
static const uint16_t T310[8] = {0, 50, 100, 200, 500, 1000, 2000};
static const uint16_t N310[8] = {1, 2, 3, 4, 6, 8, 10, 20};
static const uint16_t N311[8] = {1, 2, 3, 4, 6, 8, 10, 20};
static const uint32_t T304[8] = {50, 100, 150, 200, 500, 1000, 2000, UINT32_MAX};
// TimeToTrigger enum mapping table (36.331 TimeToTrigger IE)
static const uint32_t timeToTrigger_ms[16] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};

/* 36.133 Section 9.1.4 RSRP Measurement Report Mapping, Table: 9.1.4-1 */
static const float RSRP_meas_mapping[98] = {-140, -139, -138, -137, -136, -135, -134, -133, -132, -131, -130, -129, -128, -127, -126, -125, -124, -123, -122, -121, -120, -119, -118, -117, -116,
                                            -115, -114, -113, -112, -111, -110, -109, -108, -107, -106, -105, -104, -103, -102, -101, -100, -99,  -98,  -97,  -96,  -95,  -94,  -93,  -92,  -91,
                                            -90,  -89,  -88,  -87,  -86,  -85,  -84,  -83,  -82,  -81,  -80,  -79,  -78,  -77,  -76,  -75,  -74,  -73,  -72,  -71,  -70,  -69,  -68,  -67,  -66,
                                            -65,  -64,  -63,  -62,  -61,  -60,  -59,  -58,  -57,  -56,  -55,  -54,  -53,  -52,  -51,  -50,  -49,  -48,  -47,  -46,  -45,  -44,  -43};

static const float RSRQ_meas_mapping[35] = {-19, -18.5, -18, -17.5, -17, -16.5, -16, -15.5, -15, -14.5, -14, -13.5, -13, -12.5, -12, -11.5, -11, -10.5,
                                            -10, -9.5,  -9,  -8.5,  -8,  -7.5,  -7,  -6.5,  -6,  -5.5,  -5,  -4.5,  -4,  -3.5,  -3,  -2.5,  -2};

#endif

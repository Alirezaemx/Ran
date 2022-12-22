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

/*! \file extern.h
* \brief mac externs
* \author  Navid Nikaein and Raymond Knopp
* \date 2010 - 2014
* \version 1.0
* \email navid.nikaein@eurecom.fr
* @ingroup _mac

*/

#ifndef __MAC_EXTERN_H__
#define __MAC_EXTERN_H__

//#include "PHY/defs_common.h"
#include "mac.h"
#include "RRC/LTE/rrc_defs.h"

extern UE_RRC_INST *UE_rrc_inst;
extern UE_MAC_INST *UE_mac_inst;

extern eNB_ULSCH_INFO eNB_ulsch_info[NUMBER_OF_eNB_MAX][MAX_NUM_CCs][MAX_MOBILES_PER_ENB];	// eNBxUE = 8x8
extern eNB_DLSCH_INFO eNB_dlsch_info[NUMBER_OF_eNB_MAX][MAX_NUM_CCs][MAX_MOBILES_PER_ENB];	// eNBxUE = 8x8

extern int NB_UE_INST;

extern const int cqi_to_mcs[16];
extern uint8_t rb_table[34];
extern rb_id_t mbms_rab_id;


#if defined(PRE_SCD_THREAD)
extern uint16_t pre_nb_rbs_required[2][MAX_NUM_CCs][NUMBER_OF_UE_MAX];
extern uint8_t dlsch_ue_select_tbl_in_use;
extern uint8_t new_dlsch_ue_select_tbl_in_use;
extern bool pre_scd_activeUE[NUMBER_OF_UE_MAX];
extern eNB_UE_STATS pre_scd_eNB_UE_stats[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
#endif

extern mac_rlc_am_muilist_t rlc_am_mui;
extern SCHEDULER_MODES global_scheduler_mode;

#include "common/ran_context.h"
extern RAN_CONTEXT_t RC;
extern rb_id_t mbms_rab_id;

static const uint32_t BSR_TABLE[BSR_TABLE_SIZE] = {0,     10,    12,    14,    17,    19,    22,    26,    31,    36,    42,    49,    57,     67,     78,     91,
                                                   105,   125,   146,   171,   200,   234,   274,   321,   376,   440,   515,   603,   706,    826,    967,    1132,
                                                   1326,  1552,  1817,  2127,  2490,  2915,  3413,  3995,  4677,  5467,  6411,  7505,  8787,   10287,  12043,  14099,
                                                   16507, 19325, 22624, 26487, 31009, 36304, 42502, 49759, 58255, 68201, 79846, 93479, 109439, 128125, 150000, 300000};

// extended bsr table--currently not used
static const uint32_t Extended_BSR_TABLE[BSR_TABLE_SIZE] = {0,      10,     13,     16,     19,     23,     29,     35,     43,     53,      65,      80,      98,      120,     147,     181,
                                                            223,    274,    337,    414,    509,    625,    769,    945,    1162,   1429,    1757,    2161,    2657,    3267,    4017,    4940,
                                                            6074,   7469,   9185,   11294,  13888,  17077,  20999,  25822,  31752,  39045,   48012,   59039,   72598,   89272,   109774,  134986,
                                                            165989, 204111, 250990, 308634, 379519, 466683, 573866, 705666, 867737, 1067031, 1312097, 1613447, 1984009, 2439678, 3000000, 6000000};

/*
 * If the CQI is low, then scheduler will use a higher aggregation level and lower aggregation level otherwise
 * this is also dependent to transmission mode, where an offset could be defined
 */
// the follwoing three tables are calibrated for TXMODE 1 and 2
static const uint8_t cqi2fmt0_agg[MAX_SUPPORTED_BW][CQI_VALUE_RANGE] = {
    {3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // 1.4_DCI0_CRC_Size= 37 bits
    //{3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0}, // 5_DCI0_CRC_SIZE = 41
    {3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, // 5_DCI0_CRC_SIZE = 41
    {3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0}, // 10_DCI0_CRC_SIZE = 43
    {3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0} // 20_DCI0_CRC_SIZE = 44
};

static const uint8_t cqi2fmt1x_agg[MAX_SUPPORTED_BW][CQI_VALUE_RANGE] = {
    {3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // 1.4_DCI0_CRC_Size < 38 bits
    {3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, // 5_DCI0_CRC_SIZE  < 43
    {3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0}, // 10_DCI0_CRC_SIZE  < 47
    {3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0} // 20_DCI0_CRC_SIZE  < 55
};

static const uint8_t cqi2fmt2x_agg[MAX_SUPPORTED_BW][CQI_VALUE_RANGE] = {
    {3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // 1.4_DCI0_CRC_Size= 47 bits
    {3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, // 5_DCI0_CRC_SIZE = 55
    {3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0}, // 10_DCI0_CRC_SIZE = 59
    {3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0} // 20_DCI0_CRC_SIZE = 64
};

#endif //DEF_H

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

/*! \file phy_procedures_lte_eNB.c
* \brief Implementation of common utilities for eNB/UE procedures from 36.213 LTE specifications
* \author R. Knopp, F. Kaltenberger
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr
* \note
* \warning
*/

#include "sched_nr.h"


/// LUT for the number of symbols in the coreset indexed by coreset index (4 MSB rmsi_pdcch_config)
uint8_t nr_coreset_nsymb_pdcch_type_0_b40Mhz[16] = {2,2,2,2,2,3,3,3,3,3,1,1,1,2,2,2}; // below 40Mhz bw
uint8_t nr_coreset_nsymb_pdcch_type_0_a40Mhz[10] = {2,2,3,3,1,1,2,2,3,3}; // above 40Mhz bw
/// LUT for the number of RBs in the coreset indexed by coreset index
uint8_t nr_coreset_rb_offset_pdcch_type_0_b40Mhz[16] = {0,1,2,3,4,0,1,2,3,4,12,14,16,12,14,16};
uint8_t nr_coreset_rb_offset_pdcch_type_0_a40Mhz[10] = {0,4,0,4,0,28,0,28,0,28};
/// LUT for monitoring occasions param O indexed by ss index (4 LSB rmsi_pdcch_config)
uint8_t nr_ss_param_O_type_0_mux1_FR1[16] = {0,0,2,2,5,5,7,7,0,5,0,0,2,2,5,5};
uint8_t nr_ss_param_O_type_0_mux1_FR2[14] = {0,0,2.5,2.5,5,5,0,2.5,5,7.5,7.5,7.5,0,5};
/// LUT for number of SS sets per slot indexed by ss index
uint8_t nr_ss_sets_per_slot_type_0_FR1[16] = {1,2,1,2,1,2,1,2,1,1,1,1,1,1,1,1};
uint8_t nr_ss_sets_per_slot_type_0_FR2[14] = {1,2,1,2,1,2,2,2,2,1,2,2,1,1};
/// LUT for monitoring occasions param M indexed by ss index
uint8_t nr_ss_param_M_type_0_mux1_FR1[16] = {1,0.5,1,0.5,1,0.5,1,0.5,2,2,1,1,1,1,1,1};
uint8_t nr_ss_param_M_type_0_mux1_FR2[14] = {1,0.5,1,0.5,1,0.5,0.5,0.5,0.5,1,0.5,0.5,2,2};
/// LUT for SS first symbol index indexed by ss index
uint8_t nr_ss_first_symb_idx_type_0_mux1_FR1[8] = {0,0,1,2,1,2,1,2};



nr_subframe_t nr_subframe_select(nfapi_nr_config_request_t *cfg,unsigned char subframe)
{
  if (cfg->subframe_config.duplex_mode.value == FDD)
    return(SF_DL);
}


void nr_get_pdcch_vars_from_mib(nr_pdcch_vars_t *pdcch_vars,
                          nr_scs_e scs_common,
                          nr_scs_e pdcch_scs,
                          nr_frequency_range_e freq_range,
                          uint8_t rmsi_pdcch_config,
                          uint16_t N_RB)
{
  nr_pdcch_coreset_params_t *coreset_params = &pdcch_vars->coreset_params;
  nr_pdcch_ss_params_t *ss_params = &pdcch_vars->ss_params;
  uint8_t ss_idx = rmsi_pdcch_config&0xf;
  uint8_t cset_idx = (rmsi_pdcch_config>>4)&0xf;

  /// Coreset params
  switch(scs_common) {

    case kHz15:
      break;

    case kHz30:

      if (N_RB < 106) { // Minimum 40Mhz bandwidth not satisfied
        switch(pdcch_scs) {
          case kHz15:
            break;

          case kHz30:
            coreset_params->mux_pattern = nr_ssb_and_cset_mux_pattern_type_1;
            coreset_params->n_rb = (cset_idx < 10)? 24 : 48;
            coreset_params->n_symb = nr_coreset_nsymb_pdcch_type_0_b40Mhz[cset_idx];
            coreset_params->rb_offset =  nr_coreset_rb_offset_pdcch_type_0_b40Mhz[cset_idx];
            break;

          default:
            AssertFatal(1==0,"Invalid scs_common/pdcch_scs combination %d/%d \n", scs_common, pdcch_scs);
        }
      }

      else {
        AssertFatal(ss_idx<10 ,"Invalid scs_common/pdcch_scs combination %d/%d \n", scs_common, pdcch_scs);
        switch(pdcch_scs) {
          case kHz15:
            break;

          case kHz30:
            coreset_params->mux_pattern = nr_ssb_and_cset_mux_pattern_type_1;
            coreset_params->n_rb = (cset_idx < 4)? 24 : 48;
            coreset_params->n_symb = nr_coreset_nsymb_pdcch_type_0_b40Mhz[cset_idx];
            coreset_params->rb_offset =  nr_coreset_rb_offset_pdcch_type_0_b40Mhz[cset_idx];
            break;

          default:
            AssertFatal(1==0,"Invalid scs_common/pdcch_scs combination %d/%d \n", scs_common, pdcch_scs);
        }
      }

    case kHz60:
      break;

    case kHz120:
      break;

  default:
    AssertFatal(1==0,"Invalid common subcarrier spacing %d\n", scs_common);

  }

  /// Search space params
  switch(coreset_params->mux_pattern) {

    case nr_ssb_and_cset_mux_pattern_type_1:
      if (freq_range == nr_FR1) {
        ss_params->param_O = nr_ss_param_O_type_0_mux1_FR1[ss_idx];
        ss_params->nb_ss_sets_per_slot = nr_ss_sets_per_slot_type_0_FR1[ss_idx];
        ss_params->param_M = nr_ss_param_M_type_0_mux1_FR1[ss_idx];
        ss_params->first_symbol_idx = (ss_idx < 8)? ( (ss_idx&1)? coreset_params->n_symb : 0 ) : nr_ss_first_symb_idx_type_0_mux1_FR1[ss_idx - 8];
      }

      else {
        AssertFatal(ss_idx<14 ,"Invalid search space index for multiplexing type 1 and FR2 %d\n", ss_idx);
        ss_params->param_O = nr_ss_param_O_type_0_mux1_FR2[ss_idx];
        ss_params->nb_ss_sets_per_slot = nr_ss_sets_per_slot_type_0_FR2[ss_idx];
        ss_params->param_M = nr_ss_param_M_type_0_mux1_FR2[ss_idx];
        ss_params->first_symbol_idx = (ss_idx < 12)? ( (ss_idx&1)? 7 : 0 ) : 0;
      }

      break;

    case nr_ssb_and_cset_mux_pattern_type_2:
      break;

    case nr_ssb_and_cset_mux_pattern_type_3:
      break;

    default:
      AssertFatal(1==0, "Invalid SSB and coreset multiplexing pattern %d\n", coreset_params->mux_pattern);
  }
}

void nr_get_pdcch_type_0_monitoring_period(nr_pdcch_vars_t *pdcch_vars,
                                           uint8_t mu,
                                           uint8_t nb_slots_per_frame,
                                           uint8_t ssb_idx)
{
  uint8_t O = pdcch_vars->ss_params.param_O, M = pdcch_vars->ss_params.param_M;

  if (pdcch_vars->coreset_params.mux_pattern == nr_ssb_and_cset_mux_pattern_type_1) {
    pdcch_vars->nb_slots = 2;
    pdcch_vars->sfn_mod2 = ((uint8_t)(floor( (O*pow(2, mu) + floor(ssb_idx*M)) / nb_slots_per_frame )) & 1)? 1 : 0;
    pdcch_vars->first_slot = (uint8_t)(O*pow(2, mu) + floor(ssb_idx*M)) % nb_slots_per_frame;
  }
  else { //nr_ssb_and_cset_mux_pattern_type_2, nr_ssb_and_cset_mux_pattern_type_3
    pdcch_vars->nb_slots = 1;
  }

}

void nr_get_pdcch_vars_from_config(nr_pdcch_vars_t* pdcch_vars,
                                   nfapi_nr_coreset_t* coreset,
                                   nfapi_nr_search_space_t* search_space) {

  nr_pdcch_coreset_params_t cset_params = pdcch_vars->coreset_params;
  nr_pdcch_ss_params_t ss_params = pdcch_vars->ss_params;
  
}

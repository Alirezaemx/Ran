/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.0  (the "License"); you may not use this file
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

/*! \file lte-enb.c
 * \brief Top-level threads for eNodeB
 * \author R. Knopp, F. Kaltenberger, Navid Nikaein
 * \date 2012
 * \version 0.1
 * \company Eurecom
 * \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr, navid.nikaein@eurecom.fr
 * \note
 * \warning
 */

#define _GNU_SOURCE
#include <pthread.h>

#include "time_utils.h"

#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all

#include "rt_wrapper.h"

#include "assertions.h"


#include "PHY/types.h"

#include "PHY/defs.h"
#undef MALLOC //there are two conflicting definitions, so we better make sure we don't use it at all
//#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all

#include "../../ARCH/COMMON/common_lib.h"

//#undef FRAME_LENGTH_COMPLEX_SAMPLES //there are two conflicting definitions, so we better make sure we don't use it at all

#include "PHY/LTE_TRANSPORT/if4_tools.h"
#include "PHY/LTE_TRANSPORT/if5_tools.h"

#include "PHY/extern.h"
#include "SCHED/extern.h"
#include "LAYER2/MAC/extern.h"

#include "../../SIMU/USER/init_lte.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/extern.h"
#include "LAYER2/MAC/proto.h"
#include "RRC/LITE/extern.h"
#include "PHY_INTERFACE/extern.h"
#include "PHY_INTERFACE/defs.h"
#ifdef SMBV
#include "PHY/TOOLS/smbv.h"
unsigned short config_frames[4] = {2,9,11,13};
#endif
#include "UTIL/LOG/log_extern.h"
#include "UTIL/OTG/otg_tx.h"
#include "UTIL/OTG/otg_externs.h"
#include "UTIL/MATH/oml.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/OPT/opt.h"
#include "enb_config.h"
//#include "PHY/TOOLS/time_meas.h"

#ifndef OPENAIR2
#include "UTIL/OTG/otg_extern.h"
#endif

#if defined(ENABLE_ITTI)
# if defined(ENABLE_USE_MME)
#   include "s1ap_eNB.h"
#ifdef PDCP_USE_NETLINK
#   include "SIMULATION/ETH_TRANSPORT/proto.h"
#endif
# endif
#endif

#include "T.h"

//#define DEBUG_THREADS 1

//#define USRP_DEBUG 1
struct timing_info_t {
  //unsigned int frame, hw_slot, last_slot, next_slot;
  RTIME time_min, time_max, time_avg, time_last, time_now;
  //unsigned int mbox0, mbox1, mbox2, mbox_target;
  unsigned int n_samples;
} timing_info;

// Fix per CC openair rf/if device update
// extern openair0_device openair0;


#if defined(ENABLE_ITTI)
extern volatile int             start_eNB;
extern volatile int             start_UE;
#endif
extern volatile int                    oai_exit;

extern openair0_config_t openair0_cfg[MAX_CARDS];

extern int transmission_mode;

extern int oaisim_flag;

//pthread_t                       main_eNB_thread;

time_stats_t softmodem_stats_mt; // main thread
time_stats_t softmodem_stats_hw; //  hw acquisition
time_stats_t softmodem_stats_rxtx_sf; // total tx time
time_stats_t softmodem_stats_rx_sf; // total rx time

/* mutex, cond and variable to serialize phy proc TX calls
 * (this mechanism may be relaxed in the future for better
 * performances)
 */
static struct {
  pthread_mutex_t  mutex_phy_proc_tx;
  pthread_cond_t   cond_phy_proc_tx;
  volatile uint8_t phy_proc_CC_id;
} sync_phy_proc;

extern double cpuf;

void exit_fun(const char* s);

void init_eNB(int,int);
void stop_eNB(int nb_inst);


void wakeup_prach_eNB(PHY_VARS_eNB *eNB,RU_t *ru,int frame,int subframe);
#ifdef Rel14
void wakeup_prach_eNB_br(PHY_VARS_eNB *eNB,RU_t *ru,int frame,int subframe);
#endif

extern void oai_subframe_ind(eNB_rxtx_proc_t *proc);
extern uint8_t nfapi_pnf;

static inline int rxtx(PHY_VARS_eNB *eNB,eNB_rxtx_proc_t *proc, char *thread_name) {

  start_meas(&softmodem_stats_rxtx_sf);

  // *******************************************************************

  // ****************************************
  // Common RX procedures subframe n

  T(T_ENB_PHY_DL_TICK, T_INT(eNB->Mod_id), T_INT(proc->frame_tx), T_INT(proc->subframe_tx));

  // if this is IF5 or 3GPP_eNB
  if (eNB && eNB->RU_list && eNB->RU_list[0] && eNB->RU_list[0]->function < NGFI_RAU_IF4p5) {
    //LOG_D(PHY,"%s:%s() %u/%u Before wakeup_prach_eNB() proc->instance_cnt_rxtx:%d\n", thread_name, __FUNCTION__, proc->frame_tx, proc->subframe_tx, proc->instance_cnt_rxtx);
    wakeup_prach_eNB(eNB,NULL,proc->frame_rx,proc->subframe_rx);
    //LOG_D(PHY,"%s:%s() %u/%u Before wakeup_prach_eNB_br() proc->instance_cnt_rxtx:%d\n", thread_name, __FUNCTION__, proc->frame_tx, proc->subframe_tx, proc->instance_cnt_rxtx);
#ifdef Rel14
    wakeup_prach_eNB_br(eNB,NULL,proc->frame_rx,proc->subframe_rx);
    //LOG_D(PHY,"%s:%s() %u/%u proc->instance_cnt_rxtx:%d\n", thread_name, __FUNCTION__, proc->frame_tx, proc->subframe_tx, proc->instance_cnt_rxtx);
#endif
  }

  LOG_D(PHY, "SFN/SF proc:%d/%d rx_ind:%d/%d [num_pdus:%d]\n", NFAPI_SFNSF2SFN(eNB->UL_INFO.rx_ind.sfn_sf), NFAPI_SFNSF2SF(eNB->UL_INFO.rx_ind.sfn_sf), proc->frame_rx, proc->subframe_rx, eNB->UL_INFO.rx_ind.rx_indication_body.number_of_pdus);

  if (eNB->UL_INFO.rx_ind.sfn_sf == (proc->frame_rx<<4|proc->subframe_rx) && eNB->UL_INFO.rx_ind.rx_indication_body.number_of_pdus>0)
  {
    // Fix me here, these should be locked
    for (int i=0; i<eNB->UL_INFO.rx_ind.rx_indication_body.number_of_pdus; i++)
    {
      LOG_E(PHY, "SFN/SF:%d/%d eNB->UL_INFO.rx_ind.number_of_pdus:%d Resetting!\n", proc->frame_rx, proc->subframe_rx, eNB->UL_INFO.rx_ind.rx_indication_body.number_of_pdus);
      LOG_E(PHY, "NOT ZERO ING RX INDs\n\n\n\n\n\n\n");
      //eNB->UL_INFO.rx_ind[proc->subframe_rx&1].number_of_pdus  = 0;
    }
  }

  if (eNB->UL_INFO.crc_ind.sfn_sf == (proc->frame_rx<<4|proc->subframe_rx))
  {
    for (int i=0; i<eNB->UL_INFO.crc_ind.crc_indication_body.number_of_crcs; i++)
    {
      LOG_E(PHY, "SFN/SF:%d/%d eNB->UL_INFO.crc_ind.number_of_crcs:%d Resetting!\n", proc->frame_rx, proc->subframe_rx, eNB->UL_INFO.crc_ind.crc_indication_body.number_of_crcs);
      LOG_E(PHY, "NOT ZERO ING CRCs\n\n\n\n\n\n\n");
      //eNB->UL_INFO.crc_ind.number_of_crcs = 0;
    }
  }

  if (nfapi_pnf)
  {
    oai_subframe_ind(proc); // PNF ---> P7:subframe_ind --> VNF 
    //LOG_E(PHY, "Returned from oai_subframe_ind()\n");
    LOG_D(PHY, "UL_info[rx_ind:%d number_of_harqs:%d number_of_crcs:%d number_of_cqis:%d number_of_preambles:%d]\n", eNB->UL_INFO.rx_ind.rx_indication_body.number_of_pdus, eNB->UL_INFO.harq_ind.number_of_harqs, eNB->UL_INFO.crc_ind.crc_indication_body.number_of_crcs, eNB->UL_INFO.cqi_ind.number_of_cqis, eNB->UL_INFO.rach_ind.number_of_preambles);
  }

  // UE-specific RX processing for subframe n
  phy_procedures_eNB_uespec_RX(eNB, proc, no_relay );

  pthread_mutex_lock(&eNB->UL_INFO_mutex);

  eNB->UL_INFO.frame     = proc->frame_rx;
  eNB->UL_INFO.subframe  = proc->subframe_rx;
  eNB->UL_INFO.module_id = eNB->Mod_id;
  eNB->UL_INFO.CC_id     = eNB->CC_id;

  eNB->if_inst->UL_indication(&eNB->UL_INFO);

  pthread_mutex_unlock(&eNB->UL_INFO_mutex);

  // *****************************************
  // TX processing for subframe n+4
  // run PHY TX procedures the one after the other for all CCs to avoid race conditions
  // (may be relaxed in the future for performance reasons)
  // *****************************************
  //if (wait_CCs(proc)<0) return(-1);
  
  if (oai_exit) return(-1);
  
  phy_procedures_eNB_TX(eNB, proc, no_relay, NULL, 1);

  stop_meas( &softmodem_stats_rxtx_sf );
  
  return(0);
}


/*!
 * \brief The RX UE-specific and TX thread of eNB.
 * \param param is a \ref eNB_proc_t structure which contains the info what to process.
 * \returns a pointer to an int. The storage is not on the heap and must not be freed.
 */

static void* eNB_thread_rxtx( void* param ) {

  static int eNB_thread_rxtx_status;

  eNB_rxtx_proc_t *proc = (eNB_rxtx_proc_t*)param;
  PHY_VARS_eNB *eNB = RC.eNB[0][proc->CC_id];

  char thread_name[100];

  //LOG_D(PHY,"%s()\n", __FUNCTION__);

  // set default return value
  eNB_thread_rxtx_status = 0;


  sprintf(thread_name,"RXn_TXnp4_%d",&eNB->proc.proc_rxtx[0] == proc ? 0 : 1);
  thread_top_init(thread_name,1,850000L,1000000L,2000000L);

  //LOG_D(PHY,"%s() thread_name:%s\n", __FUNCTION__, thread_name);

  while (!oai_exit) {
    VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME( VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_RXTX0+(proc->subframe_rx&1), 0 );

    LOG_D(PHY,"%s:%s() %u/%u About to wait on proc->instance_cnt_rxtx:%d\n", thread_name, __FUNCTION__, proc->frame_tx, proc->subframe_tx, proc->instance_cnt_rxtx);
    if (wait_on_condition(&proc->mutex_rxtx,&proc->cond_rxtx,&proc->instance_cnt_rxtx,thread_name)<0) break;

    LOG_D(PHY,"%s:%s() %u/%u - WOKEN on proc->instance_cnt_rxtx proc->instance_cnt_rxtx:%d \n", thread_name, __FUNCTION__, proc->frame_tx, proc->subframe_tx, proc->instance_cnt_rxtx);

    VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME( VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_RXTX0+(proc->subframe_rx&1), 1 );

    if (oai_exit) break;

    LOG_D(PHY,"%s:%s() %u/%u About to rxtx()\n", thread_name, __FUNCTION__, proc->frame_tx, proc->subframe_tx);
    if (eNB->CC_id==0)
      if (rxtx(eNB,proc,thread_name) < 0) break;

    LOG_D(PHY,"%s:%s() %u/%u DONE rxtx()\n", thread_name, __FUNCTION__, proc->frame_tx, proc->subframe_tx);

    if (release_thread(&proc->mutex_rxtx,&proc->instance_cnt_rxtx,thread_name)<0) break;

  } // while !oai_exit

  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME( VCD_SIGNAL_DUMPER_FUNCTIONS_eNB_PROC_RXTX0+(proc->subframe_rx&1), 0 );

  printf( "Exiting eNB thread RXn_TXnp4\n");

  eNB_thread_rxtx_status = 0;
  return &eNB_thread_rxtx_status;
}


#if 0 //defined(ENABLE_ITTI) && defined(ENABLE_USE_MME)
// Wait for eNB application initialization to be complete (eNB registration to MME)
static void wait_system_ready (char *message, volatile int *start_flag) {
  
  static char *indicator[] = {".    ", "..   ", "...  ", ".... ", ".....",
			      " ....", "  ...", "   ..", "    .", "     "};
  int i = 0;
  
  while ((!oai_exit) && (*start_flag == 0)) {
    LOG_N(EMU, message, indicator[i]);
    fflush(stdout);
    i = (i + 1) % (sizeof(indicator) / sizeof(indicator[0]));
    usleep(200000);
  }
  
  LOG_D(EMU,"\n");
}
#endif





void eNB_top(PHY_VARS_eNB *eNB, int frame_rx, int subframe_rx, char *string)
{
  eNB_proc_t *proc           = &eNB->proc;
  eNB_rxtx_proc_t *proc_rxtx = &proc->proc_rxtx[0];

  proc->frame_rx    = frame_rx;
  proc->subframe_rx = subframe_rx;

  if (!oai_exit) {
    //LOG_D(PHY,"eNB_top in %p (proc %p, CC_id %d), frame %d, subframe %d, instance_cnt_prach %d\n",
	  //(void*)pthread_self(), proc, eNB->CC_id, proc->frame_rx,proc->subframe_rx,proc->instance_cnt_prach);

    T(T_ENB_MASTER_TICK, T_INT(0), T_INT(proc->frame_rx), T_INT(proc->subframe_rx));

    proc_rxtx->subframe_rx = proc->subframe_rx;
    proc_rxtx->frame_rx    = proc->frame_rx;
    proc_rxtx->subframe_tx = (proc->subframe_rx+4)%10;
    proc_rxtx->frame_tx    = (proc->subframe_rx>5) ? (1+proc->frame_rx)&1023 : proc->frame_rx;
    proc->frame_tx         = proc_rxtx->frame_tx;
    proc_rxtx->timestamp_tx = proc->timestamp_tx;

    if (rxtx(eNB,proc_rxtx,string) < 0) LOG_E(PHY,"eNB %d CC_id %d failed during execution\n",eNB->Mod_id,eNB->CC_id);
    LOG_D(PHY,"eNB_top out %p (proc %p, CC_id %d), frame %d, subframe %d, instance_cnt_prach %d\n",
	  (void*)pthread_self(), proc, eNB->CC_id, proc->frame_rx,proc->subframe_rx,proc->instance_cnt_prach);
  }
}


int wakeup_rxtx(PHY_VARS_eNB *eNB,RU_t *ru) {

  eNB_proc_t *proc=&eNB->proc;

  eNB_rxtx_proc_t *proc_rxtx=&proc->proc_rxtx[proc->frame_rx&1];

  LTE_DL_FRAME_PARMS *fp = &eNB->frame_parms;

  int i;
  struct timespec wait;
  
  LOG_D(PHY,"About to wake RUs\n");

  pthread_mutex_lock(&proc->mutex_RU);
  LOG_D(PHY,"eNB->num_RU:%d\n", eNB->num_RU);
  for (i=0;i<eNB->num_RU;i++) {
    LOG_D(PHY,"eNB->RU_list[%d]:%p\n",i,eNB->RU_list[i]);
    if (ru == eNB->RU_list[i]) {
      LOG_D(PHY,"proc->RU_mask:%02x\n", proc->RU_mask);
      if ((proc->RU_mask&(1<<i)) > 0)
	LOG_E(PHY,"eNB %d frame %d, subframe %d : previous information from RU %d (num_RU %d,mask %x) has not been served yet!\n",
	      eNB->Mod_id,proc->frame_rx,proc->subframe_rx,ru->idx,eNB->num_RU,proc->RU_mask);
      proc->RU_mask |= (1<<i);
    }
  }
  if (proc->RU_mask != (1<<eNB->num_RU)-1) {  // not all RUs have provided their information so return
    LOG_E(PHY,"Not all RUs have provided their info\n");
    pthread_mutex_unlock(&proc->mutex_RU);
    return(0);
  }
  else { // all RUs have provided their information so continue on and wakeup eNB processing
    proc->RU_mask = 0;
    pthread_mutex_unlock(&proc->mutex_RU);
  }




  wait.tv_sec=0;
  wait.tv_nsec=5000000L;

#if 0
  /* accept some delay in processing - up to 5ms */
  for (i = 0; i < 10 && proc_rxtx->instance_cnt_rxtx == 0; i++) {
    LOG_W( PHY,"[eNB] Frame %d Subframe %d, eNB RXn-TXnp4 thread busy!! (cnt_rxtx %i)\n", proc_rxtx->frame_tx, proc_rxtx->subframe_tx, proc_rxtx->instance_cnt_rxtx);
    usleep(500);
  }
  if (proc_rxtx->instance_cnt_rxtx == 0) {
    //exit_fun( "TX thread busy" ); - DJP - this is commented out just whilst I work out what has gone wrong
    return(-1);
  }
#endif

  // wake up TX for subframe n+4
  // lock the TX mutex and make sure the thread is ready
  if (pthread_mutex_timedlock(&proc_rxtx->mutex_rxtx,&wait) != 0) {
    LOG_E( PHY, "[eNB] ERROR pthread_mutex_lock for eNB RXTX thread %d (IC %d)\n", proc_rxtx->subframe_rx&1,proc_rxtx->instance_cnt_rxtx );
    exit_fun( "error locking mutex_rxtx" );
    return(-1);
  }
  
  ++proc_rxtx->instance_cnt_rxtx;
  LOG_D(PHY,"%s() %u/%u Just incremented proc->instance_cnt_rxtx:%d\n", __FUNCTION__, proc_rxtx->frame_tx, proc_rxtx->subframe_tx, proc_rxtx->instance_cnt_rxtx);
  
  // We have just received and processed the common part of a subframe, say n. 
  // TS_rx is the last received timestamp (start of 1st slot), TS_tx is the desired 
  // transmitted timestamp of the next TX slot (first).
  // The last (TS_rx mod samples_per_frame) was n*samples_per_tti, 
  // we want to generate subframe (n+4), so TS_tx = TX_rx+4*samples_per_tti,
  // and proc->subframe_tx = proc->subframe_rx+4
  proc_rxtx->timestamp_tx = proc->timestamp_rx + (4*fp->samples_per_tti);
  proc_rxtx->frame_rx     = proc->frame_rx;
  proc_rxtx->subframe_rx  = proc->subframe_rx;
  proc_rxtx->frame_tx     = (proc_rxtx->subframe_rx > 5) ? (proc_rxtx->frame_rx+1)&1023 : proc_rxtx->frame_rx;
  proc_rxtx->subframe_tx  = (proc_rxtx->subframe_rx + 4)%10;
  
  LOG_D(PHY,"Signal &proc_rxtx->cond_rxtx\n");

  // the thread can now be woken up
  if (pthread_cond_signal(&proc_rxtx->cond_rxtx) != 0) {
    LOG_E( PHY, "[eNB] ERROR pthread_cond_signal for eNB RXn-TXnp4 thread\n");
    exit_fun( "ERROR pthread_cond_signal" );
    return(-1);
  }
  
  pthread_mutex_unlock( &proc_rxtx->mutex_rxtx );

  return(0);
}

void wakeup_prach_eNB(PHY_VARS_eNB *eNB,RU_t *ru,int frame,int subframe) {

  eNB_proc_t *proc = &eNB->proc;
  LTE_DL_FRAME_PARMS *fp=&eNB->frame_parms;
  int i;

  if (ru!=NULL) {
    pthread_mutex_lock(&proc->mutex_RU_PRACH);
    for (i=0;i<eNB->num_RU;i++) {
      if (ru == eNB->RU_list[i]) {
	LOG_D(PHY,"frame %d, subframe %d: RU %d for eNB %d signals PRACH (mask %x, num_RU %d)\n",frame,subframe,i,eNB->Mod_id,proc->RU_mask_prach,eNB->num_RU);
	if ((proc->RU_mask_prach&(1<<i)) > 0)
	  LOG_E(PHY,"eNB %d frame %d, subframe %d : previous information (PRACH) from RU %d (num_RU %d, mask %x) has not been served yet!\n",
		eNB->Mod_id,frame,subframe,ru->idx,eNB->num_RU,proc->RU_mask_prach);
	proc->RU_mask_prach |= (1<<i);
      }
    }
    if (proc->RU_mask_prach != (1<<eNB->num_RU)-1) {  // not all RUs have provided their information so return
      pthread_mutex_unlock(&proc->mutex_RU_PRACH);
      return;
    }
    else { // all RUs have provided their information so continue on and wakeup eNB processing
      proc->RU_mask_prach = 0;
      pthread_mutex_unlock(&proc->mutex_RU_PRACH);
    }
  }
    
  // check if we have to detect PRACH first
  if (is_prach_subframe(fp,frame,subframe)>0) { 
    LOG_D(PHY,"Triggering prach processing, frame %d, subframe %d\n",frame,subframe);
    if (proc->instance_cnt_prach == 0) {
      LOG_W(PHY,"[eNB] Frame %d Subframe %d, dropping PRACH\n", frame,subframe);
      return;
    }
    
    // wake up thread for PRACH RX
    if (pthread_mutex_lock(&proc->mutex_prach) != 0) {
      LOG_E( PHY, "[eNB] ERROR pthread_mutex_lock for eNB PRACH thread %d (IC %d)\n", proc->thread_index, proc->instance_cnt_prach);
      exit_fun( "error locking mutex_prach" );
      return;
    }
    
    ++proc->instance_cnt_prach;
    // set timing for prach thread
    proc->frame_prach = frame;
    proc->subframe_prach = subframe;
    
    // the thread can now be woken up
    if (pthread_cond_signal(&proc->cond_prach) != 0) {
      LOG_E( PHY, "[eNB] ERROR pthread_cond_signal for eNB PRACH thread %d\n", proc->thread_index);
      exit_fun( "ERROR pthread_cond_signal" );
      return;
    }
    
    pthread_mutex_unlock( &proc->mutex_prach );
  }

}

#ifdef Rel14
void wakeup_prach_eNB_br(PHY_VARS_eNB *eNB,RU_t *ru,int frame,int subframe) {

  eNB_proc_t *proc = &eNB->proc;
  LTE_DL_FRAME_PARMS *fp=&eNB->frame_parms;
  int i;

  if (ru!=NULL) {
    pthread_mutex_lock(&proc->mutex_RU_PRACH_br);
    for (i=0;i<eNB->num_RU;i++) {
      if (ru == eNB->RU_list[i]) {
	LOG_D(PHY,"frame %d, subframe %d: RU %d for eNB %d signals PRACH BR (mask %x, num_RU %d)\n",frame,subframe,i,eNB->Mod_id,proc->RU_mask_prach_br,eNB->num_RU);
	if ((proc->RU_mask_prach_br&(1<<i)) > 0)
	  LOG_E(PHY,"eNB %d frame %d, subframe %d : previous information (PRACH BR) from RU %d (num_RU %d, mask %x) has not been served yet!\n",
		eNB->Mod_id,frame,subframe,ru->idx,eNB->num_RU,proc->RU_mask_prach_br);
	proc->RU_mask_prach_br |= (1<<i);
      }
    }
    if (proc->RU_mask_prach_br != (1<<eNB->num_RU)-1) {  // not all RUs have provided their information so return
      pthread_mutex_unlock(&proc->mutex_RU_PRACH_br);
      return;
    }
    else { // all RUs have provided their information so continue on and wakeup eNB processing
      proc->RU_mask_prach_br = 0;
      pthread_mutex_unlock(&proc->mutex_RU_PRACH_br);
    }
  }
    
  // check if we have to detect PRACH first
  if (is_prach_subframe(fp,frame,subframe)>0) { 
    LOG_D(PHY,"Triggering prach br processing, frame %d, subframe %d\n",frame,subframe);
    if (proc->instance_cnt_prach_br == 0) {
      LOG_W(PHY,"[eNB] Frame %d Subframe %d, dropping PRACH BR\n", frame,subframe);
      return;
    }
    
    // wake up thread for PRACH RX
    if (pthread_mutex_lock(&proc->mutex_prach_br) != 0) {
      LOG_E( PHY, "[eNB] ERROR pthread_mutex_lock for eNB PRACH thread %d (IC %d)\n", proc->thread_index, proc->instance_cnt_prach_br);
      exit_fun( "error locking mutex_prach" );
      return;
    }
    
    ++proc->instance_cnt_prach_br;
    // set timing for prach thread
    proc->frame_prach_br = frame;
    proc->subframe_prach_br = subframe;
    
    // the thread can now be woken up
    if (pthread_cond_signal(&proc->cond_prach_br) != 0) {
      LOG_E( PHY, "[eNB] ERROR pthread_cond_signal for eNB PRACH BR thread %d\n", proc->thread_index);
      exit_fun( "ERROR pthread_cond_signal" );
      return;
    }
    
    pthread_mutex_unlock( &proc->mutex_prach_br );
  }

}
#endif

/*!
 * \brief The prach receive thread of eNB.
 * \param param is a \ref eNB_proc_t structure which contains the info what to process.
 * \returns a pointer to an int. The storage is not on the heap and must not be freed.
 */
static void* eNB_thread_prach( void* param ) {
  static int eNB_thread_prach_status;


  PHY_VARS_eNB *eNB= (PHY_VARS_eNB *)param;
  eNB_proc_t *proc = &eNB->proc;

  // set default return value
  eNB_thread_prach_status = 0;

  thread_top_init("eNB_thread_prach",1,500000L,1000000L,20000000L);


  while (!oai_exit) {
    
    if (oai_exit) break;

    
    if (wait_on_condition(&proc->mutex_prach,&proc->cond_prach,&proc->instance_cnt_prach,"eNB_prach_thread") < 0) break;

    LOG_D(PHY,"Running eNB prach procedures\n");
    prach_procedures(eNB
#ifdef Rel14
		     ,0
#endif
		     );
    
    if (release_thread(&proc->mutex_prach,&proc->instance_cnt_prach,"eNB_prach_thread") < 0) break;
  }

  LOG_I(PHY, "Exiting eNB thread PRACH\n");

  eNB_thread_prach_status = 0;
  return &eNB_thread_prach_status;
}

#ifdef Rel14
/*!
 * \brief The prach receive thread of eNB for BL/CE UEs.
 * \param param is a \ref eNB_proc_t structure which contains the info what to process.
 * \returns a pointer to an int. The storage is not on the heap and must not be freed.
 */
static void* eNB_thread_prach_br( void* param ) {
  static int eNB_thread_prach_status;


  PHY_VARS_eNB *eNB= (PHY_VARS_eNB *)param;
  eNB_proc_t *proc = &eNB->proc;

  // set default return value
  eNB_thread_prach_status = 0;

  thread_top_init("eNB_thread_prach_br",1,500000L,1000000L,20000000L);

  while (!oai_exit) {
    
    if (oai_exit) break;
    

    if (wait_on_condition(&proc->mutex_prach_br,&proc->cond_prach_br,&proc->instance_cnt_prach_br,"eNB_prach_thread_br") < 0) break;

    LOG_D(PHY,"Running eNB prach procedures for BL/CE UEs\n");
    prach_procedures(eNB,1);
    
    if (release_thread(&proc->mutex_prach_br,&proc->instance_cnt_prach_br,"eNB_prach_thread_br") < 0) break;
  }

  LOG_I(PHY, "Exiting eNB thread PRACH BR\n");

  eNB_thread_prach_status = 0;
  return &eNB_thread_prach_status;
}

#endif


extern void init_fep_thread(PHY_VARS_eNB *, pthread_attr_t *);
extern void init_td_thread(PHY_VARS_eNB *, pthread_attr_t *);
extern void init_te_thread(PHY_VARS_eNB *, pthread_attr_t *);

void init_eNB_proc(int inst) {
  
  int i=0;
  int CC_id;
  PHY_VARS_eNB *eNB;
  eNB_proc_t *proc;
  eNB_rxtx_proc_t *proc_rxtx;
  pthread_attr_t *attr0=NULL,*attr1=NULL,*attr_prach=NULL;
    //*attr_td=NULL,*attr_te=NULL;
#ifdef Rel14
  pthread_attr_t *attr_prach_br=NULL;
#endif

  LOG_I(PHY,"%s(inst:%d) RC.nb_CC[inst]:%d \n",__FUNCTION__,inst,RC.nb_CC[inst]);

  for (CC_id=0; CC_id<RC.nb_CC[inst]; CC_id++) {
    eNB = RC.eNB[inst][CC_id];
#ifndef OCP_FRAMEWORK
    LOG_I(PHY,"Initializing eNB processes instance:%d CC_id %d \n",inst,CC_id);
#endif
    proc = &eNB->proc;

    proc_rxtx                      = proc->proc_rxtx;
    proc_rxtx[0].instance_cnt_rxtx = -1;
    proc_rxtx[1].instance_cnt_rxtx = -1;
    proc->instance_cnt_prach       = -1;
    proc->instance_cnt_asynch_rxtx = -1;
    proc->CC_id                    = CC_id;    

    proc->first_rx=1;
    proc->first_tx=1;
    proc->RU_mask=0;
    proc->RU_mask_prach=0;

    pthread_mutex_init( &eNB->UL_INFO_mutex, NULL);
    pthread_mutex_init( &proc_rxtx[0].mutex_rxtx, NULL);
    pthread_mutex_init( &proc_rxtx[1].mutex_rxtx, NULL);
    pthread_cond_init( &proc_rxtx[0].cond_rxtx, NULL);
    pthread_cond_init( &proc_rxtx[1].cond_rxtx, NULL);

    pthread_mutex_init( &proc->mutex_prach, NULL);
    pthread_mutex_init( &proc->mutex_asynch_rxtx, NULL);
    pthread_mutex_init( &proc->mutex_RU,NULL);
    pthread_mutex_init( &proc->mutex_RU_PRACH,NULL);

    pthread_cond_init( &proc->cond_prach, NULL);
    pthread_cond_init( &proc->cond_asynch_rxtx, NULL);

    pthread_attr_init( &proc->attr_prach);
    pthread_attr_init( &proc->attr_asynch_rxtx);
    //    pthread_attr_init( &proc->attr_td);
    //    pthread_attr_init( &proc->attr_te);
    pthread_attr_init( &proc_rxtx[0].attr_rxtx);
    pthread_attr_init( &proc_rxtx[1].attr_rxtx);
#ifdef Rel14
    proc->instance_cnt_prach_br    = -1;
    proc->RU_mask_prach_br=0;
    pthread_mutex_init( &proc->mutex_prach_br, NULL);
    pthread_mutex_init( &proc->mutex_RU_PRACH_br,NULL);
    pthread_cond_init( &proc->cond_prach_br, NULL);
    pthread_attr_init( &proc->attr_prach_br);
#endif
#ifndef DEADLINE_SCHEDULER
    attr0       = &proc_rxtx[0].attr_rxtx;
    attr1       = &proc_rxtx[1].attr_rxtx;
    attr_prach  = &proc->attr_prach;
#ifdef Rel14
    attr_prach_br  = &proc->attr_prach_br;
#endif

    //    attr_td     = &proc->attr_td;
    //    attr_te     = &proc->attr_te; 
#endif

    LOG_I(PHY,"eNB->single_thread_flag:%d\n", eNB->single_thread_flag);

    if (eNB->single_thread_flag==0) {
      pthread_create( &proc_rxtx[0].pthread_rxtx, attr0, eNB_thread_rxtx, &proc_rxtx[0] );
      pthread_create( &proc_rxtx[1].pthread_rxtx, attr1, eNB_thread_rxtx, &proc_rxtx[1] );
    }
    pthread_create( &proc->pthread_prach, attr_prach, eNB_thread_prach, eNB );
#ifdef Rel14
    pthread_create( &proc->pthread_prach_br, attr_prach_br, eNB_thread_prach_br, eNB );
#endif
    char name[16];
    if (eNB->single_thread_flag==0) {
      snprintf( name, sizeof(name), "RXTX0 %d", i );
      pthread_setname_np( proc_rxtx[0].pthread_rxtx, name );
      snprintf( name, sizeof(name), "RXTX1 %d", i );
      pthread_setname_np( proc_rxtx[1].pthread_rxtx, name );
    }

    AssertFatal(proc->instance_cnt_prach == -1,"instance_cnt_prach = %d\n",proc->instance_cnt_prach);

    
  }

  //for multiple CCs: setup master and slaves
  /* 
     for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
     eNB = PHY_vars_eNB_g[inst][CC_id];

     if (eNB->node_timing == synch_to_ext_device) { //master
     eNB->proc.num_slaves = MAX_NUM_CCs-1;
     eNB->proc.slave_proc = (eNB_proc_t**)malloc(eNB->proc.num_slaves*sizeof(eNB_proc_t*));

     for (i=0; i< eNB->proc.num_slaves; i++) {
     if (i < CC_id)  eNB->proc.slave_proc[i] = &(PHY_vars_eNB_g[inst][i]->proc);
     if (i >= CC_id)  eNB->proc.slave_proc[i] = &(PHY_vars_eNB_g[inst][i+1]->proc);
     }
     }
     }
  */

  /* setup PHY proc TX sync mechanism */
  pthread_mutex_init(&sync_phy_proc.mutex_phy_proc_tx, NULL);
  pthread_cond_init(&sync_phy_proc.cond_phy_proc_tx, NULL);
  sync_phy_proc.phy_proc_CC_id = 0;
}



/*!
 * \brief Terminate eNB TX and RX threads.
 */
void kill_eNB_proc(int inst) {

  int *status;
  PHY_VARS_eNB *eNB;
  eNB_proc_t *proc;
  eNB_rxtx_proc_t *proc_rxtx;
  for (int CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
    eNB=RC.eNB[inst][CC_id];
    
    proc = &eNB->proc;
    proc_rxtx = &proc->proc_rxtx[0];
    

    LOG_I(PHY, "Killing TX CC_id %d inst %d\n", CC_id, inst );

    if (eNB->single_thread_flag==0) {
      proc_rxtx[0].instance_cnt_rxtx = 0; // FIXME data race!
      proc_rxtx[1].instance_cnt_rxtx = 0; // FIXME data race!
      pthread_cond_signal( &proc_rxtx[0].cond_rxtx );    
      pthread_cond_signal( &proc_rxtx[1].cond_rxtx );
    }
    proc->instance_cnt_prach = 0;
    pthread_cond_signal( &proc->cond_prach );

    pthread_cond_broadcast(&sync_phy_proc.cond_phy_proc_tx);
    pthread_join( proc->pthread_prach, (void**)&status );    

    LOG_I(PHY, "Destroying prach mutex/cond\n");
    pthread_mutex_destroy( &proc->mutex_prach );
    pthread_cond_destroy( &proc->cond_prach );
#ifdef Rel14
    proc->instance_cnt_prach_br = 0;
    pthread_cond_signal( &proc->cond_prach_br );
    pthread_join( proc->pthread_prach_br, (void**)&status );    
    pthread_mutex_destroy( &proc->mutex_prach_br );
    pthread_cond_destroy( &proc->cond_prach_br );
#endif         
    LOG_I(PHY, "Destroying UL_INFO mutex\n");
    pthread_mutex_destroy(&eNB->UL_INFO_mutex);
    int i;
    if (eNB->single_thread_flag==0) {
      for (i=0;i<2;i++) {
	LOG_I(PHY, "Joining rxtx[%d] mutex/cond\n",i);
	pthread_join( proc_rxtx[i].pthread_rxtx, (void**)&status );
	LOG_I(PHY, "Destroying rxtx[%d] mutex/cond\n",i);
	pthread_mutex_destroy( &proc_rxtx[i].mutex_rxtx );
	pthread_cond_destroy( &proc_rxtx[i].cond_rxtx );
      }
    }
  }
}




void reset_opp_meas(void) {

  int sfn;
  reset_meas(&softmodem_stats_mt);
  reset_meas(&softmodem_stats_hw);
  
  for (sfn=0; sfn < 10; sfn++) {
    reset_meas(&softmodem_stats_rxtx_sf);
    reset_meas(&softmodem_stats_rx_sf);
  }
}


void print_opp_meas(void) {

  int sfn=0;
  print_meas(&softmodem_stats_mt, "Main ENB Thread", NULL, NULL);
  print_meas(&softmodem_stats_hw, "HW Acquisation", NULL, NULL);
  
  for (sfn=0; sfn < 10; sfn++) {
    print_meas(&softmodem_stats_rxtx_sf,"[eNB][total_phy_proc_rxtx]",NULL, NULL);
    print_meas(&softmodem_stats_rx_sf,"[eNB][total_phy_proc_rx]",NULL,NULL);
  }
}

void init_transport(PHY_VARS_eNB *eNB) {

  int i;
  int j;
  LTE_DL_FRAME_PARMS *fp = &eNB->frame_parms;

  LOG_E(PHY, "Initialise transport\n");

  for (i=0; i<NUMBER_OF_UE_MAX; i++) {
    LOG_I(PHY,"Allocating Transport Channel Buffers for DLSCH, UE %d\n",i);
    for (j=0; j<2; j++) {
      eNB->dlsch[i][j] = new_eNB_dlsch(1,8,NSOFT,fp->N_RB_DL,0,fp);
      if (!eNB->dlsch[i][j]) {
	LOG_E(PHY,"Can't get eNB dlsch structures for UE %d \n", i);
	exit(-1);
      } else {
	eNB->dlsch[i][j]->rnti=0;
	LOG_D(PHY,"dlsch[%d][%d] => %p rnti:%d\n",i,j,eNB->dlsch[i][j], eNB->dlsch[i][j]->rnti);
      }
    }
    
    LOG_I(PHY,"Allocating Transport Channel Buffer for ULSCH, UE %d\n",i);
    eNB->ulsch[1+i] = new_eNB_ulsch(MAX_TURBO_ITERATIONS,fp->N_RB_UL, 0);
    
    if (!eNB->ulsch[1+i]) {
      LOG_E(PHY,"Can't get eNB ulsch structures\n");
      exit(-1);
    }
    
    // this is the transmission mode for the signalling channels
    // this will be overwritten with the real transmission mode by the RRC once the UE is connected
    eNB->transmission_mode[i] = fp->nb_antenna_ports_eNB==1 ? 1 : 2;
  }
  // ULSCH for RA
  eNB->ulsch[0] = new_eNB_ulsch(MAX_TURBO_ITERATIONS, fp->N_RB_UL, 0);
  
  if (!eNB->ulsch[0]) {
    LOG_E(PHY,"Can't get eNB ulsch structures\n");
    exit(-1);
  }
  eNB->dlsch_SI  = new_eNB_dlsch(1,8,NSOFT,fp->N_RB_DL, 0, fp);
  LOG_D(PHY,"eNB %d.%d : SI %p\n",eNB->Mod_id,eNB->CC_id,eNB->dlsch_SI);
  eNB->dlsch_ra  = new_eNB_dlsch(1,8,NSOFT,fp->N_RB_DL, 0, fp);
  LOG_D(PHY,"eNB %d.%d : RA %p\n",eNB->Mod_id,eNB->CC_id,eNB->dlsch_ra);
  eNB->dlsch_MCH = new_eNB_dlsch(1,8,NSOFT,fp->N_RB_DL, 0, fp);
  LOG_D(PHY,"eNB %d.%d : MCH %p\n",eNB->Mod_id,eNB->CC_id,eNB->dlsch_MCH);
  
  
  eNB->rx_total_gain_dB=130;
  
  for(i=0; i<NUMBER_OF_UE_MAX; i++)
    eNB->mu_mimo_mode[i].dl_pow_off = 2;
  
  eNB->check_for_total_transmissions = 0;
  
  eNB->check_for_MUMIMO_transmissions = 0;
  
  eNB->FULL_MUMIMO_transmissions = 0;
  
  eNB->check_for_SUMIMO_transmissions = 0;
  
  fp->pucch_config_common.deltaPUCCH_Shift = 1;
    
} 

void init_eNB_afterRU(void) {

  int inst,CC_id,ru_id,i,aa;
  PHY_VARS_eNB *eNB;

  LOG_I(PHY,"%s() RC.nb_inst:%d\n", __FUNCTION__, RC.nb_inst);

  for (inst=0;inst<RC.nb_inst;inst++) {
    LOG_I(PHY,"RC.nb_CC[inst]:%d\n", RC.nb_CC[inst]);
    for (CC_id=0;CC_id<RC.nb_CC[inst];CC_id++) {

      LOG_I(PHY,"RC.nb_CC[inst:%d][CC_id:%d]:%p\n", inst, CC_id, RC.eNB[inst][CC_id]);

      eNB                                  =  RC.eNB[inst][CC_id];
      phy_init_lte_eNB(eNB,0,0);
      // map antennas and PRACH signals to eNB RX
      if (0) AssertFatal(eNB->num_RU>0,"Number of RU attached to eNB %d is zero\n",eNB->Mod_id);
      LOG_I(PHY,"Mapping RX ports from %d RUs to eNB %d\n",eNB->num_RU,eNB->Mod_id);
      eNB->frame_parms.nb_antennas_rx       = 0;

      LOG_E(PHY,"Overwriting eNB->prach_vars.rxsigF[0]:%p\n", eNB->prach_vars.rxsigF[0]);

      eNB->prach_vars.rxsigF[0] = (int16_t**)malloc16(64*sizeof(int16_t*));
#ifdef Rel14
      for (int ce_level=0;ce_level<4;ce_level++) {
        LOG_E(PHY,"Overwriting eNB->prach_vars_br.rxsigF.rxsigF[0]:%p\n", eNB->prach_vars_br.rxsigF[ce_level]);
        eNB->prach_vars_br.rxsigF[ce_level] = (int16_t**)malloc16(64*sizeof(int16_t*));
      }
#endif

      LOG_I(PHY,"eNB->num_RU:%d\n", eNB->num_RU);

      for (ru_id=0,aa=0;ru_id<eNB->num_RU;ru_id++) {
	eNB->frame_parms.nb_antennas_rx    += eNB->RU_list[ru_id]->nb_rx;

	AssertFatal(eNB->RU_list[ru_id]->common.rxdataF!=NULL,
		    "RU %d : common.rxdataF is NULL\n",
		    eNB->RU_list[ru_id]->idx);

	AssertFatal(eNB->RU_list[ru_id]->prach_rxsigF!=NULL,
		    "RU %d : prach_rxsigF is NULL\n",
		    eNB->RU_list[ru_id]->idx);

	for (i=0;i<eNB->RU_list[ru_id]->nb_rx;aa++,i++) { 
	  LOG_I(PHY,"Attaching RU %d antenna %d to eNB antenna %d\n",eNB->RU_list[ru_id]->idx,i,aa);
	  eNB->prach_vars.rxsigF[0][aa]    =  eNB->RU_list[ru_id]->prach_rxsigF[i];
#ifdef Rel14
	  for (int ce_level=0;ce_level<4;ce_level++)
	    eNB->prach_vars_br.rxsigF[ce_level][aa] = eNB->RU_list[ru_id]->prach_rxsigF_br[ce_level][i];
#endif
	  eNB->common_vars.rxdataF[aa]     =  eNB->RU_list[ru_id]->common.rxdataF[i];
	}
      }




      if (eNB->frame_parms.nb_antennas_rx < 1)
      {
        LOG_I(PHY, "%s() ************* DJP ***** eNB->frame_parms.nb_antennas_rx:%d - GOING TO HARD CODE TO 1", __FUNCTION__, eNB->frame_parms.nb_antennas_rx);
        eNB->frame_parms.nb_antennas_rx = 1;
      }
      else
      {
        LOG_I(PHY," Delete code\n");
      }

      if (eNB->frame_parms.nb_antennas_tx < 1)
      {
        LOG_I(PHY, "%s() ************* DJP ***** eNB->frame_parms.nb_antennas_tx:%d - GOING TO HARD CODE TO 1", __FUNCTION__, eNB->frame_parms.nb_antennas_tx);
        eNB->frame_parms.nb_antennas_tx = 1;
      }
      else
      {
        LOG_I(PHY," Delete code\n");
      }




      AssertFatal(eNB->frame_parms.nb_antennas_rx >0,
		  "inst %d, CC_id %d : nb_antennas_rx %d\n",inst,CC_id,eNB->frame_parms.nb_antennas_rx);
      LOG_I(PHY,"inst %d, CC_id %d : nb_antennas_rx %d\n",inst,CC_id,eNB->frame_parms.nb_antennas_rx);

      init_transport(eNB);
      //init_precoding_weights(RC.eNB[inst][CC_id]);
    }
    printf("RC.nb_CC[inst:%d]:%d CC_id:%d AFTER LOOP - About to init_eNB_proc\n", inst, RC.nb_CC[inst], CC_id);
    init_eNB_proc(inst);
  }
  printf("%s() RC.nb_inst:%d AFTER LOOP\n", __FUNCTION__, RC.nb_inst);

  printf("%s:%d RC.nb_RU:%d\n", __FILE__, __LINE__, RC.nb_RU);
  for (ru_id=0;ru_id<RC.nb_RU;ru_id++) {

    AssertFatal(RC.ru[ru_id]!=NULL,"ru_id %d is null\n",ru_id);
    
    RC.ru[ru_id]->wakeup_rxtx         = wakeup_rxtx;
    RC.ru[ru_id]->wakeup_prach_eNB    = wakeup_prach_eNB;
    RC.ru[ru_id]->wakeup_prach_eNB_br = wakeup_prach_eNB_br;
    RC.ru[ru_id]->eNB_top             = eNB_top;
  }

  LOG_I(PHY,"%s() Exitting\n", __FUNCTION__);
}

void init_eNB(int single_thread_flag,int wait_for_sync) {
  
  int CC_id;
  int inst;
  PHY_VARS_eNB *eNB;

  LOG_I(PHY,"[lte-softmodem.c] eNB structure about to allocated RC.nb_L1_inst:%d RC.nb_L1_CC[0]:%d\n",RC.nb_L1_inst,RC.nb_L1_CC[0]);

  if (RC.eNB == NULL) RC.eNB = (PHY_VARS_eNB***) malloc(RC.nb_L1_inst*sizeof(PHY_VARS_eNB **));
  LOG_I(PHY,"[lte-softmodem.c] eNB structure RC.eNB allocated\n");
  for (inst=0;inst<RC.nb_L1_inst;inst++) {
    if (RC.eNB[inst] == NULL) RC.eNB[inst] = (PHY_VARS_eNB**) malloc(RC.nb_CC[inst]*sizeof(PHY_VARS_eNB *));
    for (CC_id=0;CC_id<RC.nb_L1_CC[inst];CC_id++) {
      if (RC.eNB[inst][CC_id] == NULL) RC.eNB[inst][CC_id] = (PHY_VARS_eNB*) malloc(sizeof(PHY_VARS_eNB));
      eNB                     = RC.eNB[inst][CC_id]; 
      eNB->abstraction_flag   = 0;
      eNB->single_thread_flag = single_thread_flag;


      LOG_I(PHY,"Initializing eNB %d CC_id %d single_thread_flag:%d\n",inst,CC_id,single_thread_flag);
#ifndef OCP_FRAMEWORK
      LOG_I(PHY,"Initializing eNB %d CC_id %d\n",inst,CC_id);
#endif


      eNB->td                   = ulsch_decoding_data;//(single_thread_flag==1) ? ulsch_decoding_data_2thread : ulsch_decoding_data;
      eNB->te                   = dlsch_encoding;//(single_thread_flag==1) ? dlsch_encoding_2threads : dlsch_encoding;

      
      LOG_I(PHY,"Registering with MAC interface module\n");
      AssertFatal((eNB->if_inst         = IF_Module_init(inst))!=NULL,"Cannot register interface");
      eNB->if_inst->schedule_response   = schedule_response;
      eNB->if_inst->PHY_config_req      = phy_config_request;
      memset((void*)&eNB->UL_INFO,0,sizeof(eNB->UL_INFO));
      memset((void*)&eNB->Sched_INFO,0,sizeof(eNB->Sched_INFO));
      LOG_I(PHY,"Setting indication lists\n");
      eNB->UL_INFO.rx_ind.rx_indication_body.rx_pdu_list   = eNB->rx_pdu_list;
      eNB->UL_INFO.crc_ind.crc_indication_body.crc_pdu_list = eNB->crc_pdu_list;
      eNB->UL_INFO.sr_ind.sr_pdu_list = eNB->sr_pdu_list;
      eNB->UL_INFO.harq_ind.harq_pdu_list = eNB->harq_pdu_list;
      eNB->UL_INFO.cqi_ind.cqi_pdu_list = eNB->cqi_pdu_list;
      eNB->UL_INFO.cqi_ind.cqi_raw_pdu_list = eNB->cqi_raw_pdu_list;
    }

  }

  LOG_I(PHY,"[lte-softmodem.c] eNB structure allocated\n");
}


void stop_eNB(int nb_inst) {

  for (int inst=0;inst<nb_inst;inst++) {
    LOG_I(PHY,"Killing eNB %d processing threads\n",inst);
    kill_eNB_proc(inst);
  }
}

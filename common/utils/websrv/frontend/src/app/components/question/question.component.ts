/* eslint-disable @typescript-eslint/naming-convention */
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

/*! \file common/utils/websrv/frontend/src/app/components/question/question.component.ts
 * \brief: implementation of web interface frontend for oai
 * \utility component used to allow entering parameters before sending a request to the backend
 * \author:  Yacine  El Mghazli, Francois TABURET
 * \date 2022
 * \version 0.1
 * \company NOKIA BellLabs France
 * \email: yacine.el_mghazli@nokia-bell-labs.com  francois.taburet@nokia-bell-labs.com
 * \note
 * \warning
 */
import {Component, Inject} from "@angular/core";
import {MAT_DIALOG_DATA, MatDialogRef} from "@angular/material/dialog";
import {CmdCtrl} from "src/app/controls/cmd.control";
import {HelpApi, HelpRequest, HelpResp} from "src/app/api/help.api";
export interface QuestionDialogData {
  title: string;
  control: CmdCtrl;
}
@Component({selector : "app-question", templateUrl : "./question.component.html", styleUrls : [ "./question.component.css" ]})
export class QuestionDialogComponent {
  hlp_question: string = "";
  constructor(
      public dialogRef: MatDialogRef<QuestionDialogComponent>,
      public helpApi: HelpApi,
      @Inject(MAT_DIALOG_DATA) public data: QuestionDialogData,
  )
  {
  this.helpApi.getHelp$({module : "question", command : data.control.cmdname.replace(" ", "_"), object : "input"})
      .subscribe(
      response => {
         if (response.status == 201)
             this.hlp_question = response.body!.text;
         },
         err => { this.hlp_question = ""; },
         );
                 
  }
  onNoClick()
  {
    this.dialogRef.close();
  }
}

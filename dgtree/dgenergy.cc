/*
 * dgenergy.cc

/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*-
 *
 * Copyright (c) 1997, 2000 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Header: /cvsroot/nsnam/ns-2/mobile/energy-model.cc,v 1.6 2005/06/13 17:50:41 haldar Exp $
 */

// Contributed by Satish Kumar <kkumar@isi.edu>

#include "dgenergy.h"

DG_EnergyModel::DG_EnergyModel()
{

}
void DG_EnergyModel::DecrTxEnergy(double txtime, double P_tx)
{
	double dEng = P_tx * txtime;
	//if (energy_ <= dEng)
	//	energy_ = 0.0;
	//else
	//	energy_ = energy_ - dEng;
     //The node dead message should be sent to the sender when eneergy is 0.
	// keep track of total time spent transmitting
	total_sndtime_ += txtime;
	// This variable keeps track of total energy consumption in Transmission..
	et_=et_+dEng;
	//printf("\n Value of Et is %lf \n",total_sndtime_);

}


void DG_EnergyModel::DecrRcvEnergy(double rcvtime, double P_rcv)
{
	double dEng = P_rcv * rcvtime;
	//if (energy_ <= dEng)
	//	energy_ = 0.0;
	//else
	//	energy_ = energy_ - dEng;
	 //The node dead message should be sent to the sender when eneergy is 0.
	// keep track of total receive time
	total_rcvtime_ += rcvtime;

	// This variable keeps track of total energy consumption in RECV mode..
	er_=er_+dEng;
//printf("\n Value of Er is %lf \n",total_rcvtime_);
}

void DG_EnergyModel::DecrIdleEnergy(double idletime, double P_idle)
{
	double dEng = P_idle * idletime;
	if (energy_ <= dEng)
		energy_ = 0.0;
	else
		energy_ = energy_ - dEng;
	 //The node dead message should be sent to the sender when eneergy is 0.

	// keep track of total idle time
	total_idletime_ += idletime;

	// This variable keeps track of total energy consumption in IDLE mode..
	ei_=ei_+dEng;
//printf("\n Value of Ei is %lf \n",total_idletime_);
}


void DG_EnergyModel::DecrSleepEnergy(double sleeptime, double P_sleep)
{
	double dEng = P_sleep * sleeptime;
	if (energy_ <= dEng)
		energy_ = 0.0;
	else
		energy_ = energy_ - dEng;
	 //The node dead message should be sent to the sender when eneergy is 0.

	// keep track of total sleep time
	total_sleeptime_ += sleeptime;

	// This variable keeps track of total energy consumption in SLEEP mode..
	es_=es_+dEng;
	//printf("\n Value of Es is %lf \n",total_sleeptime_);
}

void DG_EnergyModel::DecrTransitionEnergy(double transitiontime, double P_transition)
{
	double dEng = P_transition * transitiontime;
	if (energy_ <= dEng)
		energy_ = 0.0;
	else
		energy_ = energy_ - dEng;
	 //The node dead message should be sent to the sender when eneergy is 0.
}

double DG_EnergyModel::getEnergyConsumed()
{
	double energy = 0;
	energy  = M_SLEEP_POWER * total_sleeptime_;
	energy += M_IDLE_POWER * total_idletime_;
	energy += M_TRANSMIT_POWER * total_sndtime_;
	energy += M_RECEIVE_POWER * total_rcvtime_;
	//energy = es_ + ei_ + er_ + et_ + energy_ ;
   printf("\n Value of Es:%lf Ei:%lf Et:%lf Er:%lf  \n",(M_SLEEP_POWER * total_sleeptime_),(M_IDLE_POWER * total_idletime_),(M_TRANSMIT_POWER * total_sndtime_),(M_RECEIVE_POWER * total_rcvtime_));
	//printf("\n Total energy consumed by %d i
	return energy;
}

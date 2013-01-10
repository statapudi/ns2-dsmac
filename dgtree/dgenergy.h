
#ifndef DGENERGY_H_
#define DGENERGY_H_

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
 * $Header: /cvsroot/nsnam/ns-2/mobile/energy-model.h,v 1.13 2005/06/14 19:43:48 haldar Exp $
 */

// Contributed by Satish Kumar (kkumar@isi.edu)

#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include <float.h>
#include "config.h"
#include "trace.h"
#include "rng.h"
#include "mobilenode.h"
#include "god.h"
#include "random.h"

/* ENERGY CONSUMPTION */
#define M_TRANSMIT_POWER	0.01488 //(0.02475 for comparision)		/* 14.88 mW to transmit */
#define M_RECEIVE_POWER		0.01250 //(0.01350 for comparision)	/* 12.50 mW to receive */
#define M_IDLE_POWER		0.01236 	/* 12.36 mW idle */
#define M_SLEEP_POWER		0.000016	/* 0.016 mW sleep */
#define M_BANDWIDTH			2457.6		/* 2.4 kilobits */ // 19660.8
//#define M_BANDWIDTH			2097152 /*2 Mb*/
#define INITIAL_ENERGY		3.2

class MobileNode;

class DG_EnergyModel : public TclObject {
public:

	DG_EnergyModel();

	inline void   setEnergy(double e) { energy_ = e; }
	inline double getEnergy() const { return energy_; }
	//inline double getInitialenergy() const { return initialenergy_; }
    	double getEnergyConsumed();
	inline double et() const { return et_; }
	inline double er() const { return er_; }
	inline double ei() const { return ei_; }
	inline double es() const { return es_; }

	virtual void DecrTxEnergy(double txtime, double P_tx);
	virtual void DecrRcvEnergy(double rcvtime, double P_rcv);
	virtual void DecrIdleEnergy(double idletime, double P_idle);
//
	virtual void DecrSleepEnergy(double sleeptime, double P_sleep);
	virtual void DecrTransitionEnergy(double transitiontime, double P_transition);
//
	inline virtual double MaxTxtime(double P_tx) {
		return(energy_/P_tx);
	}
	inline virtual double MaxRcvtime(double P_rcv) {
		return(energy_/P_rcv);
	}
	inline virtual double MaxIdletime(double P_idle) {
		return(energy_/P_idle);
	}

	virtual void add_rcvtime(float t) {total_rcvtime_ += t;}
	virtual void add_sndtime(float t) {total_sndtime_ += t;}
//
	virtual void add_sleeptime(float t) {total_sleeptime_ += t;};

protected:
	double energy_;

	double er_; // Total energy consumption in RECV
	double et_; // Total energy consumption in transmission
	double ei_; // Total energy consumption in IDLE mode
	double es_; // Total energy consumption in SLEEP mode

	double initialenergy_;
	double level1_;
	double level2_;

	MobileNode *node_;

   	int sleep_mode_;	 // = 1: radio is turned off
	float total_sleeptime_;  // total time of radio in off mode
   	float total_rcvtime_;	 // total time in receiving data
	float total_sndtime_;	 // total time in sending data
	float total_idletime_;	// total time in idle mode
	float last_time_gosleep; // time when radio is turned off

	bool node_on_;   	 // on-off status of this node -- Chalermek
};



#endif /* DGENERGY_H_ */

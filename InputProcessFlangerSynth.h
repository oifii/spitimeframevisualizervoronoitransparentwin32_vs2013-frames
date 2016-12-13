/*
 * Copyright (c) 2015-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef InputProcessFlangerSynth_h
#define InputProcessFlangerSynth_h

#include "Tonic.h"

using namespace Tonic;

// Expects a RingBuffer called "input" to already exist and be capturing input
class InputProcessFlangerSynth : public Synth {

public:

	InputProcessFlangerSynth(){

		//ControlParameter inputGain = addParameter("inputGain", 0.f).displayName("Input Gain (dbFS)").min(-12.f).max(12.f);
		//ControlParameter ringFreq = addParameter("ringFreq", 10.f).displayName("Ring Mod Frequency").min(10.f).max(1000.f).logarithmic(true); //original
		//ControlParameter ringFreq = addParameter("ringFreq", 1.0f).displayName("Ring Mod Frequency").min(1.0f).max(1000.f).logarithmic(true);

		// This will try to read from a globally-known ring buffer called "input"
		RingBufferReader inputReader = RingBufferReader().bufferName("input");

		//Generator ringModInput = inputReader * (inputGain >> ControlDbToLinear()).smoothed() * SineWave().freq(ringFreq.smoothed(0.5)); //original
		//Generator ringModInput = inputReader * (inputGain >> ControlDbToLinear()).smoothed() * SineWave().freq(ringFreq.smoothed(0.5)); //spi good
		//Generator ringModInput = inputReader * (inputGain >> ControlDbToLinear()).smoothed() * SawtoothWave().freq(ringFreq.smoothed(0.5)); //spi bad
		//Generator ringModInput = inputReader * (inputGain >> ControlDbToLinear()).smoothed(); //spi
		//Generator ringModInput = inputReader * (inputGain >> ControlDbToLinear()).smoothed() * SineWave().freq(ringFreq);

		//BasicDelay delay = BasicDelay(1.2f).delayTime(0.5f).wetLevel(0.4f).dryLevel(1.0f).feedback(0.6f); //original
		BasicDelay delay = BasicDelay(0.0f).delayTime(1.0f).wetLevel(1.0f).dryLevel(0.0f).feedback(0.8f); //spi

		FBCombFilter combfilter = FBCombFilter().delayTime(SineWave().freq(5));
		//spi note: flanger is a comb filter for which we sweep the delay

		//setOutputGen((inputReader >> delay) * 0.5f); //original
		//setOutputGen( (inputReader >> combfilter) * 0.5f); //spi

		//setOutputGen((inputReader >> combfilter) ); //spi
		setOutputGen(inputReader); //spi

		//setOutputGen((delay.input(ringModInput)) * 0.5f + delay2.input(ringModInput)); //spi
		//setOutputGen(ringModInput);
	}

};

TONIC_REGISTER_SYNTH(InputProcessFlangerSynth);


#endif
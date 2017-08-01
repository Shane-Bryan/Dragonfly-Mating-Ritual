/*
 * ledsubdset.h
 *
 *  Created on: Jul 30, 2017
 *      Author: eric
 */

#ifndef LEDSUBDSET_H_
#define LEDSUBDSET_H_
#include <FastLED.h>


class LedSubset{
	public:
		virtual LedSubset(CRGBArray masterLeds, int startLed, int numLeds);
		virtual ~LedSubset();
		CRGBSet leds;

	private:
		unsigned int numLeds;
};


class Petal : public LedSubset {};

class Spar : public LedSubset {};


class PetalRing : public LedSubset{
	public:
		PetalRing(CRGBArray masterLeds, int startLed, unsigned int ledCounts[]);
		Petal petals[6];
};


class SparRing : public LedSubset{
	public:
		SparRing(CRGBArray masterLeds, int startLed, unsigned int ledCounts[]);
		Spar spars[6];
};

class Blossom : public LedSubset{
	public:
		Blossom(CRGBArray masterLeds,
				unsigned int pr0[],
				unsigned int pr1[],
				unsigned int pr2[],
				unsigned int spars[]);

		PetalRing petalRings[3];
		SparRing sparRing;
};

#endif /* LEDSUBDSET_H_ */

/*
 * animation.h
 *
 *  Created on: Jul 30, 2017
 *      Author: eric
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_


class AnimationPattern {
	virtual void onLoop();
};

class SimplePattern : public AnimationPattern {
	SimplePattern(LedSubset target);
	LedSubset target;
};

class MultiPattern : public AnimationPattern {
	MultiPattern(LedSubset targets[]);
};




#endif /* ANIMATION_H_ */

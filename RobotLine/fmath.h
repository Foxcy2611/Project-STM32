#ifndef __MATH__H
#define __MATH__H

static __inline float constrainf(float value, float min, float max){
  return value < min ? min : (value > max ? max : value);
}

#endif 

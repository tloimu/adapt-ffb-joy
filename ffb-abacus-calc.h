#ifndef _FFB_ABACUS_CALC_
#define _FFB_ABACUS_CALC_

#include "ffb-abacus.h"

// Exposed for unit testing purposes

void calcRemainingPhaseTime(FfbEffect *effect, TimeUnit timeDelta);
void calcEnvelope(FfbEffect *effect, ForceUnit *ioFx, ForceUnit *ioFy);
void calcEffectLocalTime(FfbEffect *effect, TimeUnit dt);

void calcEffectConstant(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSine(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSquare(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectTriangle(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectFriction(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectInertia(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSpring(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSawtoothDown(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectSawtoothUp(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectRamp(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectDamper(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectCustom(FfbEffect *effect, PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, TimeUnit timeDelta, ForceUnit *outFx, ForceUnit *outFy);
void calcEffectAutoCenter(PositionUnit x, PositionUnit y, PositionUnit dx, PositionUnit dy, ForceUnit *outFx, ForceUnit *outFy);

#endif // _FFB_ABACUS_CALC_

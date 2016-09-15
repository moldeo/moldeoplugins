#ifndef MOEFFECTPARTICLESFRACTALFACTORY_H
#define MOEFFECTPARTICLESFRACTALFACTORY_H

#include "moEffectParticlesFractal.h"

class moEffectParticlesFractalFactory : public moEffectFactory
{
public:

    moEffectParticlesFractalFactory() {}
    virtual ~moEffectParticlesFractalFactory() {}

    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif // MOEFFECTPARTICLESFRACTALFACTORY_H

#include "moEffectParticlesFractalFactory.h"

//========================
//  Factory
//========================

moEffectParticlesFractalFactory *m_EffectParticlesFractalFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectParticlesFractalFactory==NULL)
		m_EffectParticlesFractalFactory = new moEffectParticlesFractalFactory();
	return(moEffectFactory*) m_EffectParticlesFractalFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectParticlesFractalFactory;
	m_EffectParticlesFractalFactory = NULL;
}

moEffect*  moEffectParticlesFractalFactory::Create() {
	return new moEffectParticlesFractal();
}

void moEffectParticlesFractalFactory::Destroy(moEffect* fx) {
	delete fx;
}

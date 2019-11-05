evgenConfig.description = "Single geantino with flat eta-phi and E = 10 GeV"
evgenConfig.keywords = ["singleParticle"]

include("MC15JobOptions/ParticleGun_Common.py")

import ParticleGun as PG
genSeq.ParticleGun.sampler.pid = 999
genSeq.ParticleGun.sampler.mom = PG.EThetaMPhiSampler(energy=10000, theta=0)
genSeq.ParticleGun.sampler.pos = PG.PosSampler(x=[38.5,39.5],y=0,z=0,t=0.0)

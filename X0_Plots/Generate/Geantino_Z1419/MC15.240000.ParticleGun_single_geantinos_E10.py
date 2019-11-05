evgenConfig.description = "Single geantino with flat eta-phi and E = 10 GeV"
evgenConfig.keywords = ["singleParticle"]

include("MC15JobOptions/ParticleGun_Common.py")

import ParticleGun as PG
genSeq.ParticleGun.sampler.pid = 999
genSeq.ParticleGun.sampler.mom = PG.EEtaMPhiSampler(energy=10000, eta=0)
genSeq.ParticleGun.sampler.pos = PG.PosSampler(x=0.0,y=0.0,z=[1411.82,1425.87],t=0.0)

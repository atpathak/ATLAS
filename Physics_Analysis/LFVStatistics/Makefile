all : Signif fitws keepAllSys printws runBreakdown runPulls InjP0 CalcP0 runasy scanQmu readws loadws combws readworkspace modifyws modifyll scalews SplitHistoSys_ll AddNorm FitCrossCheckForLimits NPscan NPpulls fit_6par fit_mu AddNorm_HWW AddNorm_Htautau fit_Nmu AddMatching_Htautau AddNorm_mH
AddNorm_mH: AddNorm_mH.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o AddNorm_mH AddNorm_mH.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
AddNorm_HWW: AddNorm_HWW.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o AddNorm_HWW AddNorm_HWW.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
AddMatching_Htautau: AddMatching_Htautau.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o AddMatching_Htautau AddMatching_Htautau.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
AddNorm_Htautau: AddNorm_Htautau.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o AddNorm_Htautau AddNorm_Htautau.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
fit_mu: fit_mu.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o fit_mu fit_mu.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
fit_Nmu: fit_Nmu.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o fit_Nmu fit_Nmu.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
fit_6par: fit_6par.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o fit_6par fit_6par.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
NPpulls: NPpulls.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o NPpulls NPpulls.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
NPscan: NPscan.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o NPscan NPscan.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
FitCrossCheckForLimits: FitCrossCheckForLimits.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o FitCrossCheckForLimits FitCrossCheckForLimits.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
AddNorm: AddNorm.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o AddNorm AddNorm.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
SplitHistoSys_ll: SplitHistoSys_ll.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o SplitHistoSys_ll SplitHistoSys_ll.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
keepAllSys: keepAllSys.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o keepAllSys keepAllSys.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
Signif: Signif.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o Signif Signif.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
fitws: fitws.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o fitws fitws.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
printws: printws.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o printws printws.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
readws: readws.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o readws readws.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
scalews: scalews.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o scalews scalews.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
modifyws: modifyws.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o modifyws modifyws.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
modifyll: modifyll.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o modifyll modifyll.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
loadws: loadws.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o loadws loadws.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
combws: combws.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o combws combws.cc               `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
readworkspace: readworkspace.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o readworkspace readworkspace.cc `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
runBreakdown: runBreakdown.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o runBreakdown  runBreakdown.cc  `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm -lMLP -lXMLIO
runPulls: runPulls.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o runPulls  runPulls.cc          `root-config --cflags --libs --ldflags`  -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
InjP0:	InjP0.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o InjP0  InjP0.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
CalcP0:	CalcP0.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o CalcP0  CalcP0.cc              `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
runasy: runAsymptoticsCLs.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o runasy runAsymptoticsCLs.cc    `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
scanQmu: scanQmu.cc
	g++ -g -O2 -fPIC -Wno-deprecated -D__USE_XOPEN2K8 -o scanQmu scanQmu.cc             `root-config --cflags --libs --ldflags` -L${ROOTSYS}/lib -lHistFactory -lXMLParser -lRooStats -lRooFit -lRooFitCore -lThread -lMinuit -lFoam -lMathMore -lHtml -lGpad -lm
clean: 
	rm -f Signif fitws keepAllSys printws runBreakdown runPulls InjP0 CalcP0 runasy scanQmu readws loadws combws readworkspace modifyws modifyll scalews SplitHistoSys_ll AddNorm FitCrossCheckForLimits NPscan NPpulls fit_6par fit_mu AddNorm_HWW AddMatching_Htautau

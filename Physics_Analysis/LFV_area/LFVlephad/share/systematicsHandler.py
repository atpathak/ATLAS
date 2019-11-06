from QFramework import *
from ROOT import *
import os.path

class systematicsObject:
    def __init__ (self,name="", dir="", tree="", alias="",wsalias="",workingdir="",number=-1):
        self.name=name
        self.dir=dir
        self.tree=tree
        self.alias=alias
        self.wsalias=wsalias
        self.workingdir=workingdir
        self.number=number

    def changeFileName(self,name,prefix="samples",isSampleFile=False):
        if isSampleFile and (self.tree=="*" or self.tree==None) and (self.dir=="*" or self.dir==None):
            return name
        nametype=type(name)
        path=str(name).split(":")[0]
        pathname=os.path.split(path)[0]
        pathfile=os.path.split(path)[1]
        samplename=str(name).split(":")[1]
        pathfile=pathfile.replace(prefix,prefix+"_"+self.alias)
        path=os.path.join(pathname,pathfile)
        samplename=self.changeSampleName(samplename)
        return nametype(path+":"+samplename)

    def changeSampleName(self,name,prefix="samples"):
        nametype=type(name)
        name=str(name).replace(prefix,prefix+"_"+self.alias)
        return nametype(name)

def readSystematicsMap(systematicMap,byName=True,listonly=False):
  systematicsMap={}
  systematicMapping={}
  if (os.path.exists(systematicMap)):
    for line in file(systematicMap):
        if line.strip() != "" and "#" != line.strip()[0]:
            systematicMapping[line.split()[0]]=line.split()
    #print systematicMapping
    num=10
    for systematic in systematicMapping:
        systematics=systematicsObject()
        systematics.name=systematic
        systematics.number=num
        num+=1
        if len(systematicMapping[systematic])>1 and systematicMapping[systematic][1] !="*":
           systematics.dir=(systematicMapping[systematic][1].strip("/"))+"/"
        else:
           systematics.dir=""
        if len(systematicMapping[systematic])>2 and systematicMapping[systematic][2] !="*":
           systematics.tree=systematicMapping[systematic][2]
        else:
           systematics.tree=None
        if len(systematicMapping[systematic])>3 and systematicMapping[systematic][3] !="*":
           systematics.alias=systematicMapping[systematic][3]
        else:
           systematics.alias=systematics.name
        if len(systematicMapping[systematic])>4 and systematicMapping[systematic][4] !="*":
           systematics.wsalias=systematicMapping[systematic][4]
        else:
           systematics.wsalias=systematics.alias
        if byName:
           systematicsMap[systematic]=systematics
        else:
           systematicsMap[systematics.alias]=systematics
  else:
    WARN ("systematicMap does not exists "+systematicMap)
  if listonly:
      return systematicsMap.values()
  return systematicsMap

def createSystematics(systematic,systematicMap,returnmap=False,verbose=True):
  if systematic != "":
      if verbose: INFO("requested systematic "+systematic)
  else:
      if returnmap:
          return None, None
      else:
          return None
  if verbose: INFO("load systematicMap"+systematicMap)
  systematicMapping=readSystematicsMap(systematicMap)
  systematics= systematicMapping.get(systematic,None)
  if systematics== None:
     WARN("request systematic not in map: "+systematic)
  if returnmap:
      return systematics, systematicMapping
  else:
      return systematics

def checkForSystematics(config,returnmap=False):
  systematic=str(config.getTagStringDefault("systematic",""))
  systematicMap=str(config.getTagStringDefault("systematicMap","maps/systematicMap_leplep.map"))
  return createSystematics(systematic,systematicMap,returnmap)

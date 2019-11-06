#!/usr/bin/python

import sys

##
# @class AnalysisSetupParser
#
class AnalysisSetupParser(object):
    """
    Class to provide an instance to the analysis' specifics
    """
    def __init__(self, config_file):
        self.__config_file_name = config_file
        self.__module_name = ''
        self.__module_path_name = ''        
        self.__module = None
        self.__ready = self.__read_modules()

    def __read_modules(self):
        print self.__config_file_name

        self.__module_name = self.__config_file_name.split('/')[-1].replace('.py','') # python module names get imported without '.py'
        
        self.__module_path_name = '/'.join(self.__config_file_name.split('/')[:-1]) # check if modname is python module only or contains a path
        
        if self.__module_path_name:
            sys.path.insert(0, self.__module_path_name)

        if self.__module_name:
            exec('import %s' % self.__module_name)
        else:
            print "Error: no module found..."
            return False

        self.__module = sys.modules[self.__module_name]

        mod_smp_list = []
        for k in self.__module.__dict__:
            if k[:2] != '__':
                mod_smp_list.append(self.__module.__dict__[k])

        for smp in mod_smp_list:
            if isinstance(smp, self.__module.AnalysisSetup):
                self.__instance = smp
                break

        return True

    @property 
    def instance(self):
        return self.__instance



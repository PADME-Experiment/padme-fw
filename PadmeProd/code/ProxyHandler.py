#!/usr/bin/python

import re
import subprocess

class ProxyHandler:

    def __init__(self):
        pass

    def run_command(self,command):
        print "> %s"%command
        p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
        return iter(p.stdout.readline,b'')

    def renew_voms_proxy(self,long_proxy_file):

        # Check if current proxy is still valid and renew it if less than 2 hours before it expires
        renew = True
        for line in self.run_command("voms-proxy-info"):
            r = re.match("^timeleft  \: (\d+)\:.*$",line)
            if r and int(r.group(1))>=2: renew = False

        if renew:
            print "- VOMS proxy is missing or will expire in less than 2 hours. Renewing it using %s"%long_proxy_file
            cmd = "voms-proxy-init --noregen --cert %s --key %s --voms vo.padme.org --valid 24:00"%(long_proxy_file,long_proxy_file)
            for line in self.run_command(cmd): print(line.rstrip())

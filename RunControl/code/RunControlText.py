#!/usr/bin/python

import socket
import sys
import readline

# Enable line editing for raw_input()
readline.parse_and_bind('tab: complete')

class RunControlText:

    def __init__(self):

        # Create a TCP/IP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Connect the socket to the port where the server is listening
        server_address = ('localhost', 10000)
        print 'Connecting to RunControl server on host %s port %s' % server_address
        try:
            self.sock.connect(server_address)
        except:
            print "Error connecting to server: "+str(sys.exc_info()[0])
            return

        self.main_loop()

    def get_answer(self):

        # First get length of string
        l = ""
        for i in range(5): # Max 99999 characters
            ch = self.sock.recv(1)
            if ch:
                l += ch
            else:
                print >>sys.stderr, 'no more data from server'
                return "server_close"
        ll = int(l)

        # Then read the right amount of characters from the socket
        ans = ""
        for i in range(ll):
            ch = self.sock.recv(1)
            if ch:
                ans += ch
            else:
                print >>sys.stderr, 'no more data from server'
                return "server_close"

        return ans

    def send_command(self,cmd):

        if len(cmd)<100000:
            self.sock.sendall("%5.5d"%len(cmd)+cmd)
        else:
            print >>sys.stderr,'command too long: cannot send'

    def main_loop(self):

        try:
    
            while True:

                # Get message to send
                message = raw_input("SEND (q or Q to Quit): ")
                if (message == 'q' or message == 'Q'): break

                # Handle special commands
                if (message == 'new_run'): ans = self.new_run()

                elif (message == 'abort_run'): ans = self.abort_run()

                elif (message == 'start_run'): ans = self.start_run()

                elif (message == 'stop_run'): ans = self.stop_run()

                # Handle simple command/answer commands
                else:

                    print >>sys.stderr, 'sending "%s"' % message
                    self.send_command(message)

                    ans = self.get_answer()

                if (ans == "server_close"):
                    print "Server has closed connection: exiting"
                    break
                else:
                    print ans

        finally:

            print >>sys.stderr, 'closing socket'
            self.sock.close()

    def new_run(self):

        self.send_command("new_run")

        ans = self.get_answer()
        if (ans == "server_close"):
            return "server_close"
        elif (ans != "run_number"):
            print 'new_run - protocol error: expected "run_number", received '+ans
            return "error"
        
        message = raw_input("Run number (next or dummy): ")
        if (message != 'next' and message != 'dummy'):
            print 'new_run - invalid input '+message+' - Expected "next" or "dummy"'
            return "error"
        self.send_command(message)

        #ans = self.get_answer()
        #print 'new_run - new run will have number '+ans

        ans = self.get_answer()
        if (ans == "server_close"):
            return "server_close"
        elif (ans != "run_type"):
            print 'new_run - protocol error: expected "run_type", received '+ans
            return "error"
        
        message = raw_input("Run type (TEST, DAQ, COSMIC): ")
        if (message != 'TEST' and message != 'DAQ' and message != 'COSMIC'):
            print 'new_run - invalid input '+message+' - Expected "TEST", "DAQ", or "dummy"'
            return
        self.send_command(message)

        ans = self.get_answer()
        if (ans == "server_close"):
            return "server_close"
        elif (ans != "shift_crew"):
            print 'new_run - protocol error: expected "shift_crew", received '+ans
            return "error"
        
        message = raw_input("Shift crew: ")
        self.send_command(message)

        ans = self.get_answer()
        if (ans == "server_close"):
            return "server_close"
        elif (ans != "run_comment"):
            print 'new_run - protocol error: expected "run_comment", received '+ans
            return "error"
        
        message = raw_input("Start of run comment: ")
        self.send_command(message)

        ans = self.get_answer()
        if (ans == "server_close"):
            return "server_close"
        elif (ans == "error_init"):
            print 'new_run - an error occourred while starting initialization'
            return "error"
        elif (ans != "start_init"):
            print 'new_run - protocol error: expected "start_init", received '+ans
            return "error"

        # Handle initialization procedure
        while True:
            ans = self.get_answer()
            if (ans == "server_close"):
                return "server_close"
            elif (ans == "init_timeout"):
                print 'New run initialization timeout'
                break
            elif (ans == "init_fail"):
                print 'New run initialization failed'
                break
            elif (ans == "init_ready"):
                print 'New run initialization completed correctly'
                break
            else:
                print ans

        return ans

    def abort_run(self):

        self.send_command("abort_run")

        # Handle termination procedure
        while True:
            ans = self.get_answer()
            if (ans == "server_close"):
                return "server_close"
            elif (ans == "terminate_ok"):
                print 'Run terminated correctly'
                break
            elif (ans == "terminate_error"):
                print 'Run terminated with errors'
                break
            else:
                print ans

        return ans

    def start_run(self):

        self.send_command("start_run")

        # Handle termination procedure
        while True:
            ans = self.get_answer()
            if (ans == "server_close"):
                return "server_close"
            elif (ans == "run_started"):
                print 'Run started correctly'
                break
            else:
                print ans

        return ans

    def stop_run(self):

        self.send_command("stop_run")

        ans = self.get_answer()
        if (ans == "server_close"):
            return "server_close"
        elif (ans != "run_comment_end"):
            print 'stop_run - protocol error: expected "run_comment_end", received '+ans
            return "error"
                
        message = raw_input("End of run comment: ")
        self.send_command(message)

        # Handle termination procedure
        while True:
            ans = self.get_answer()
            if (ans == "server_close"):
                return "server_close"
            elif (ans == "terminate_ok"):
                print 'Run terminated correctly'
                break
            elif (ans == "terminate_error"):
                print 'Run terminated with errors'
                break
            else:
                print ans

        return ans

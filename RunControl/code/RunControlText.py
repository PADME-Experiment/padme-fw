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
                self.error_exit("Server closed connection")
        ll = int(l)

        # Then read the right amount of characters from the socket
        ans = ""
        for i in range(ll):
            ch = self.sock.recv(1)
            if ch:
                ans += ch
            else:
                self.error_exit("Server closed connection")

        return ans

    def send_command(self,cmd):

        if len(cmd)<100000:
            self.sock.sendall("%5.5d"%len(cmd)+cmd)
        else:
            self.error_exit("Command too long: cannot send")

    def ask_server(self,cmd):

        self.send_command(cmd)
        return self.get_answer()

    def error_exit(self,msg):

        print "*** FATAL ERROR ***"
        print msg
        self.sock.close()
        exit(1)

    def main_loop(self):

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

                print "Sending %s"%message
                ans = self.ask_server(message)
                if (message == "exit"):
                    print ans
                    print "Server's gone. I'll take my leave as well..."
                    break

            # Show result of previous command
            print ans

        print "Closing socket"
        self.sock.close()

    def new_run(self):

        print "Sending new_run"
        self.send_command("new_run")

        ans = self.get_answer()
        if (ans != "run_number"):
            print "new_run - protocol error: expected run_number, received %s"%ans
            return "error"

        message = raw_input("Run number (next or dummy): ")
        if (message != 'next' and message != 'dummy'):
            print "new_run - invalid input %s - Expected next or dummy"%message
            self.send_command("error")
            return "error"
        print "Sending %s"%message
        self.send_command(message)

        ans = self.get_answer()
        print "new_run - new run will have number %s"%ans

        ans = self.get_answer()
        if (ans != "run_type"):
            print "new_run - protocol error: expected run_type, received %s"%ans
            return "error"

        message = raw_input("Run type (TEST, DAQ, COSMIC): ")
        if (message != 'TEST' and message != 'DAQ' and message != 'COSMIC'):
            print "new_run - invalid input %s - Expected TEST, DAQ, or COSMIC"%message
            self.send_command("error")
            return
        print "Sending %s"%message
        self.send_command(message)

        ans = self.get_answer()
        print "new_run - new run will have type %s"%ans

        ans = self.get_answer()
        if (ans != "shift_crew"):
            print "new_run - protocol error: expected shift_crew, received %s"%ans
            return "error"

        message = raw_input("Shift crew: ")
        print "Sending %s"%message
        self.send_command(message)

        ans = self.get_answer()
        if (ans != "run_comment"):
            print "new_run - protocol error: expected run_comment, received %s"%ans
            return "error"

        message = raw_input("Start of run comment: ")
        print "Sending %s"%message
        self.send_command(message)

        ans = self.get_answer()
        if (ans == "error_init"):
            print "new_run - an error occourred while starting initialization"
            return "error"
        elif (ans != "start_init"):
            print "new_run - protocol error: expected start_init or error_init, received %s"%ans
            return "error"

        # Handle initialization procedure
        while True:
            ans = self.get_answer()
            if (ans == "init_timeout"):
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

        print "Sending abort_run"
        self.send_command("abort_run")

        # Handle termination procedure
        while True:
            ans = self.get_answer()
            if (ans == "terminate_ok"):
                print 'Run terminated correctly'
                break
            elif (ans == "terminate_error"):
                print 'Run terminated with errors'
                break
            else:
                print ans

        return ans

    def start_run(self):

        print "Sending start_run"
        self.send_command("start_run")

        # Handle termination procedure
        while True:
            ans = self.get_answer()
            if (ans == "run_started"):
                print 'Run started correctly'
                break
            else:
                print ans

        return ans

    def stop_run(self):

        print "Sending stop_run"
        self.send_command("stop_run")

        ans = self.get_answer()
        if (ans != "run_comment_end"):
            print "stop_run - protocol error: expected run_comment_end, received %s"%ans
            return "error"

        message = raw_input("End of run comment: ")
        print "Sending %s"%message
        self.send_command(message)

        # Handle termination procedure
        while True:
            ans = self.get_answer()
            if (ans == "terminate_ok"):
                print 'Run terminated correctly'
                break
            elif (ans == "terminate_error"):
                print 'Run terminated with errors'
                break
            else:
                print ans

        return ans

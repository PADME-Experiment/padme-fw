#!/usr/bin/python

import socket
import sys

def get_answer(sock):

    # First get length of string
    l = ""
    for i in range(5): # Max 99999 characters
        ch = sock.recv(1)
        if ch:
            l += ch
        else:
            print >>sys.stderr, 'no more data from server'
            return "server_close"
    ll = int(l)

    # Then read the right amount of characters from the socket
    ans = ""
    for i in range(ll):
        ch = sock.recv(1)
        if ch:
            ans += ch
        else:
            print >>sys.stderr, 'no more data from server'
            return "server_close"

    return ans

def send_command(sock,cmd):

    if len(cmd)<100000:
        #print "%5.5d"%len(cmd)+cmd
        sock.sendall("%5.5d"%len(cmd)+cmd)
    else:
        print >>sys.stderr,'command too long: cannot send'

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 10000)
print >>sys.stderr, 'connecting to %s port %s' % server_address
sock.connect(server_address)

try:
    
    while 1:

        # Get message to send
        message = raw_input("SEND (q or Q to Quit): ")
        if (message == 'q' or message == 'Q'): break

        # Send data
        print >>sys.stderr, 'sending "%s"' % message
        send_command(sock,message)

        ans = get_answer(sock)
        if (ans == "server_close"):
            print "Server has closed connection: exiting"
            break
        else:
            print ans

finally:
    print >>sys.stderr, 'closing socket'
    sock.close()

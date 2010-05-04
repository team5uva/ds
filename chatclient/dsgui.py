#!/usr/bin/python
import pygtk
pygtk.require('2.0')
import gtk, gtk.glade, gobject
import sys, os, socket, struct, time, random, re

# Do something on namechange ack
# Quit nicely

def do_quit(widget = None):
    global registered, history
    if csock:
        csock.close()
    if sock:
        # Disconnect properly
        if registered:
            send_msg(120, 'Exiting client')
        try:
            sock.close()
        except:
            pass
    if gtk.main_level():
        gtk.main_quit()
    fd = open(os.path.expanduser('~/.ds_history'),'w')
    fd.write("\n".join(history))
    sys.exit(0)

def message(msg):
    send_msg(200, "#all " + msg)

def action(msg):
    send_msg(210, "#all " + msg)

def send_msg(type, data='',s=None):
    global sock, chattext, rawlog
    if not s and not sock:
        chattext.append("*** Not connected yet")
    else:
        data = data.strip()
        if not len(data):
            msg = struct.pack(">hh", 4, type)
        else:
            msg = struct.pack(">hh%ds" % len(data), len(data) + 4, type, data)
        l = len(msg)
        rawlog.append("Sending message %s" % str((l, type, data)))
        while l:
            if s:
                m = s.send(msg)
            else:
                m = sock.send(msg)
            l = l -m
            msg = msg[m:]

def process_input(widget, event):
    global chattext, sock, gui, nick, newnick, pingtime, pwd, registered, csock, history, curtext, tpointer
    if event.hardware_keycode == 98: #UP
        if tpointer == 0: curtext = widget.get_text()
        tpointer = max(-len(history),tpointer -1)
        widget.set_text(history[tpointer])
        return
    elif event.hardware_keycode == 104: #DOWN
        if not tpointer: return
        tpointer += 1
        if tpointer == 0: widget.set_text(curtext)
        else: widget.set_text(history[tpointer])
        return
    if len(event.string) == 1 and ord(event.string) == 13:
        curtext = ''
        text = widget.get_text()
        history.append(text.rstrip())
        history = history[-200:]
        tpointer = 0
        if len(text):
            widget.set_text('')
            # Process the text
            if text[0] == '/':
                # Command
                text = text[1:].split(None,1)
                args = ''
                command = text[0].lower()
                if len(text) > 1:
                    args = text[1]
                if command == 'me':
                    action(args.strip())
                elif command == 'help':
                    chattext.append("*** Available commands:")
                    chattext.append("  /connect [server [port]]")
                    chattext.append("  /connect controlserver [port]")
                    chattext.append("  /nick nickname [password]")
                    chattext.append("  /ping")
                    chattext.append("  /msg nickname text")
                    chattext.append("  /quit [message]")
                    chattext.append("  /stopserver")
                    chattext.append("  /raw type data")
                    chattext.append("  /tree")
                elif command in ['quit', 'exit']:
                    if not args:
                        args = 'Leaving'
                    send_msg(120, args.strip())
                    registered = False
                    do_quit()
                elif command == 'nick':
                    if not args:
                        chattext.append("*** Usage: /nick new_nickname")
                    else:
                        data = args.split(None,1)
                        if len(data) > 1:
                            pwd = data[1]
                        else:
                            pwd = ''
                        newnick = data[0]
                        if not sock:
                            nicks.clear()
                            nickiters.clear()
                            nickiters["(%s)" % newnick] = nicks.append(["(%s)" % newnick])
                            gui.get_widget('nickname').set_text(newnick)
                            nick = newnick
                        else:
                            msg = newnick
                            if pwd:
                                msg += ' ' + pwd
                            if not registered:
                                nicks.clear()
                                nickiters.clear()
                                nickiters["(%s)" % newnick] = nicks.append(["(%s)" % newnick])
                                gui.get_widget('nickname').set_text(newnick)
                                send_msg(100, msg)
                            else:
                                send_msg(160, msg)
                elif command == 'ping':
                    chattext.append('*** Sending ping to server')
                    ident = "%d" % random.randint(10000,99999)
                    pingtime[ident] = time.time()
                    send_msg(140,ident)
                elif command == 'connect':
                    if sock or csock:
                        chattext.append('*** Error: already connected')
                    else:
                        port = 2000
                        data = args.split()
                        server = None
                        if len(data) == 1:
                            server = data[0]
                        elif len(data) == 2:
                            (server, port) = data
                            try:
                                port = int(port)
                            except:
                                port = 2000
                        elif not (len(data) == 0):
                            chattext.append('*** usage: /connect [server [port]]')
                            return
                        if not server or server == 'controlserver':
                            if not (port in [2001,3001,4001,5001,6001,7001,8001,9001]):
                                port = 2001
                            chattext.append('*** Connecting to controlserver at owf3.science.uva.nl, port %d' % port)
                          
                            csock = socket.socket()
                            try:
                                csock.connect(('146.50.1.95', port))
                                gobject.io_add_watch(csock, gobject.IO_IN | gobject.IO_PRI | gobject.IO_ERR | gobject.IO_HUP, process_network, True)
                            except:
                                csock = None
                                chattext.append("*** Connection to controlserver failed")
                            else:
                                send_msg(610,s=csock)
                        else:
                            chattext.append("*** Trying to connect to %s, port %d" % (server, port))
                            sock = socket.socket()
                            try:
                                sock.connect((server,port))
                                gobject.io_add_watch(sock, gobject.IO_IN | gobject.IO_PRI | gobject.IO_ERR | gobject.IO_HUP, process_network)
                            except:
                                sock = None
                                chattext.append("*** Connection to '%s' failed" % server)
                                #raise
                            else: # Send identification
                                if pwd:
                                    send_msg(100,"%s %s" % (nick, pwd))
                                else:
                                    send_msg(100,nick)
                elif command == 'msg':
                    if not registered:
                        chattext.append("*** Not yet registered""")
                        return
                    data = args.split(None,2)
                    if len(data) < 2 or data[0] not in nickiters.keys():
                        chattext.append("*** usage: /msg nickname message")
                        print data
                        print nickiters.keys()
                    else:
                        send_msg(200, args)
                elif command == 'stopserver':
                    chattext.append("*** Sending stop-server")
                    send_msg(700)
                elif command == 'raw':
                    if not args.strip():
                        chattext.append("*** usage: /raw type text")
                    else:
                        data = args.split(None,1)
                        try:
                            type = int(data[0])
                        except:
                            chattext.append("*** Invalid message type '%s'" % data[0])
                        else:
                            if len(data) > 1:
                                chattext.append("*** Sending raw message, type %d, text: %s" % (type, data[1]))
                                send_msg(type, data[1])
                            else:
                                chattext.append("*** Sending raw message, type %d" % type)
                                send_msg(type)
                elif command == 'tree':
                    if csock:
                        #send_msg(950,s=csock)
                        chattext.append("*** /tree currently not supported")
                    else:
                        chattext.append("*** Not connected to controlserver")
                else:
                    chattext.append("*** Unknown command: /%s" % command)
            else:
                if not registered:
                    chattext.append("*** Not yet registered""")
                    return
                message(text.strip())

msgbuf = ""
def process_network(rsock, condition, control=False):
    global chattext, msgbuf, registered, nicks, pingtime, nickiters,rawlog,csock, sock, newnick, nick
    error = False
    data = ''

    if condition in [gobject.IO_ERR, gobject.IO_HUP]:
        chattext.append('*** Server disconnected')
        error = True
    else:
        try:
            data = rsock.recv(1000)
        except:
            chattext.append('*** Receiving data from server failed')
            error = True
                
        if len(data) == 0:
            if control:
	      try:
		csock.close()
	      except:
		pass
	      csock = None
	    else:
              chattext.append('*** Server disconnected')
              error = True

        msgbuf += data
        while len(msgbuf) >= 4:
            mlen = struct.unpack(">h", msgbuf[:2])[0]
            if mlen > 200 or mlen < 4:
                chattext.append('*** Malformed message received, length: %s' % mlen)
                error = True
            if mlen > len(msgbuf): # Message not completely received yet
                break
            else:
                # Extract message
                if mlen > 4:
                    (type, text) = struct.unpack(">h%ds" % (mlen -4), msgbuf[2:mlen])
                else:
                    (type, text) = ((struct.unpack(">h", msgbuf[2:4])[0], ''))
                msgbuf = msgbuf[mlen:]
                rawlog.append("Received message %s" % str((mlen, type, text)))
                # Handle message
                if type == 611:

                    if text == 'none':
                        chattext.append('*** No servers are active')
                        error = True
                    else:
                        try:
                            (server, port) = text.split(':')
                            port = int(port)
                        except:
                            chattext.append('*** Invalid data received from controlserver')
                            try:
                                csock.close()
                            except:
                                pass
                            csock = None
                            return False
                        else:
                            chattext.append("*** Trying to connect to %s, port %d" % (server, port))
                            sock = socket.socket()
                            try:
                                sock.connect((server,port))
                                gobject.io_add_watch(sock, gobject.IO_IN | gobject.IO_PRI | gobject.IO_ERR | gobject.IO_HUP, process_network)
                            except:
                                sock = None
                                chattext.append("*** Connection to '%s' failed" % server)
                                #raise
                            else: # Send identification
                                if pwd:
                                    send_msg(100,"%s %s" % (nick, pwd))
                                else:
                                    send_msg(100,nick)
                elif type == 951:
                    chattext.append("*** Current server tree")
                    for line in text.split("\n"):
                        chattext.append(line)
                # Client added
                elif type == 110:
                    if not registered:
                        chattext.append("*** Server sent chat data while not registered")
                    chattext.append("*** %s joined the party" % text)
                    nickiters[text] = nicks.append([text])
                # Client removed
                elif type == 130:
                    if not registered:
                        chattext.append("*** Server sent chat data while not registered")
                    data = text.split(None,1)
                    msg = ''
                    qnick = data[0]
                    if len(data) > 1:
                        msg = data[1]
                    if qnick in nickiters.keys():
                        chattext.append("*** %s has left the building (%s)" % (qnick,msg))
                        nicks.remove(nickiters[qnick])
                        nickiters.pop(qnick)
                    else:
                        chattext.append("*** Received unexpected 'client exited' for %s (%s)" % (qnick,msg))
                # Echo request
                elif type == 140: send_msg(150, text)
                # Echo response
                elif type == 150: 
                    if text.strip() in pingtime.keys():
                        chattext.append("*** Received ping response, %2f seconds" % (time.time() - pingtime[text.strip()]))
                    else:
                        chattext.append("*** Received unexcpected ping response (%s)" % text.strip())
                # Name changed
                elif type == 170:
                    if not registered:
                        chattext.append("*** Server sent chat data while not registered")
                    try:
                        (oldname, newname) = text.split()
                    except:
                        chattext.append("*** Malformed message received")
                    else:
                        if oldname in nickiters.keys():
                            chattext.append("*** %s changed his nick to %s" % (oldname, newname))
                            nicks.remove(nickiters[oldname])
                            nickiters[newname] = nicks.append([newname])
                        else:
                            chattext.append("*** Received unexpected namechange %s => %s" % (oldname, newname))
                # Text message
                elif type == 300:
                    if not registered:
                        chattext.append("*** Server sent chat data while not registered")
                    try:
                        (fr, to, msg) =  text.split(None,2)
                    except:
                        chattext.append("*** Malformed message received")
                    else:
                        if to == "#all":
                            chattext.append("<%s> %s" % (fr, msg))
                        else:
                            chattext.append("-%s- %s" % (fr, msg))
                # Action message
                elif type == 310:
                    if not registered:
                        chattext.append("*** Server sent chat data while not registered")
                    try:
                        (fr, to, msg) =  text.split(None,2)
                    except:
                        chattext.append("*** Malformed message received")
                    else:
                        if to == "#all":
                            chattext.append("* %s %s" % (fr, msg))
                        else:
                            chattext.append("-* %s %s" % (fr, msg))
                # Registration successful
                elif type == 500:
                    nicks.clear()
                    nickiters.clear()
                    registered = True
                # Registration failed
                elif type == 510:
                    chattext.append("*** Registration failed, type /nick name [password] to retry")
                # Nickchange successful
                elif type == 520:
                    gui.get_widget('nickname').set_text(newnick)
                # Nickchange failed
                elif type == 530:
                    chattext.append("*** Namechange failed, type /nick name [password] to retry")
                else:
                    chattext.append("*** Message of unknown type %d received" % type)
    if error:
        rawlog.append("Debug: unregistering sockets")
        msgbuf = ""
        try:
            sock.close()
        except:
            pass
        try:
            csock.close()
        except:
            pass
        registered = False
        sock = None
        csock = None
        nicks.clear()
        nickiters.clear()
        nickiters["(%s)" % nick] = nicks.append(["(%s)" % nick])
        return False # To counter the gobject.io_add_watch
    return True

# Start me up...
gladepath = '.'
if os.access('/opt/stud/opsys/DS2010', os.R_OK):
    gladepath = '/opt/stud/opsys/DS2010'
gui = gtk.glade.XML(os.path.join(gladepath,'chatclient.glade'),'window1')

invalidre = re.compile(r'''[^ a-zA-Z0-9`~!@#$%^&\*\(\)\-_=\+\[\]\{\}\\\|;:'",<\.>/\?]''')
def append(text, textbuffer, scrolledwindow):
    sys.stdout.write(text + "\n")
    if invalidre.search(text):
        text = '*** Invalid data received, see stdout!'
    textbuffer.insert(textbuffer.get_iter_at_offset(textbuffer.get_char_count()),
                      u"%s" % text + "\n")
    scrolledwindow.get_vadjustment().set_value(100000000)

chattext  = gtk.TextBuffer()
chattext.append = lambda x: append(x, chattext, gui.get_widget('scrolledwindow2'))
rawlog    = gtk.TextBuffer()
rawlog.append = lambda x: append(x, rawlog, gui.get_widget('scrolledwindow1'))

chattext.set_text("*** Distributed systems chat 2006 (c) Dennis Kaarsemaker\n")
chattext.append("*** Small changes 2010 by Roy Bakker");
chattext.append("*** Type /help for help")
chattext.append("*** Laatse wijzigingen in het protocol: 29/3/2010")
chattext.append("*** Laatse wijzigingen in de client: 31/3/2010")
chattext.append("*** Zie Blackboard voor meer informatie")
nick      = os.getenv('USER')
newnick   = ''
nicks     = gtk.ListStore(str)
sock      = None
csock     = None
pingtime  = {}
pwd       = ''
registered = False
history = []
tpointer = 0
curtext = ''
try:
    fd = open(os.path.expanduser('~/.ds_history'))
    history = fd.read().split("\n")
except:
    pass

from getopt import gnu_getopt

opts, args = gnu_getopt(sys.argv[1:],'n:s:p:')
_server = _port = ''
for o in opts:
    if o[0] == '-n':
        nick = o[1]
    elif o[0] == '-s':
        _server = o[1]
    elif o[0] == '-p':
        _port = o[1]

if _server or _port:
    gui.get_widget('entry1').set_text('/connect %s %s' % (_server, _port))    

nickiters = {}
nickiters["(%s)" % nick] = nicks.append(["(%s)" % nick])
# Build GUI
signals = {
    'on_window1_destroy': do_quit,
    'on_entry1_key_release_event': process_input,
    'on_quit1_activate': do_quit,
    'on_about1_activate': lambda x: gtk.glade.XML(os.path.join(gladepath,'chatclient.glade'),'aboutdialog1').get_widget('aboutdialog1').show
}
gui.signal_autoconnect(signals)

# Fill initial GUI values
gui.get_widget('nickname').set_text(nick)
gui.get_widget('textview2').set_buffer(chattext)
gui.get_widget('textview1').set_buffer(rawlog)
nicklist = gui.get_widget('treeview1')
nicklist.set_model(nicks)
nickcolumn = gtk.TreeViewColumn('Nicknames')
renderer = gtk.CellRendererText()
nickcolumn.pack_start(renderer)
nickcolumn.set_attributes(renderer, text=0)
nicklist.append_column(nickcolumn)
gui.get_widget('entry1').grab_focus()

# Go Go Go!
gtk.main()

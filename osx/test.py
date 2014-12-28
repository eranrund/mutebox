import threading
import Queue
import rtmidi
import time
import logging
import curses
from rtmidi.midiutil import open_midiport


log = logging.getLogger("midifilter")
stdscr = curses.initscr()

begin_x = 20; begin_y = 7
LCD_W = 20
LCD_H = 2
width = LCD_W + 4 
height = LCD_H + 4 

win = curses.newwin(height, width, begin_y, begin_x)

lcd_buf = ['?'] * 256

def draw_lcd():
    win.addstr(0, 0, '-' * width)

    l1 = ''.join(lcd_buf[:5])
    win.addstr(1, 0, '| ' + ''.join(lcd_buf[:20]) + ' |')
    win.addstr(2, 0, '| ' + ''.join(lcd_buf[64:84]) + ' |')

    win.addstr(3, 0, '-' * width)

    win.refresh()

class MidiDispatcher(threading.Thread):
    def __init__(self, midiin, midiout):
        super(MidiDispatcher, self).__init__()
        self.midiin = midiin
        self.midiout = midiout
        self._wallclock = time.time()
        self.queue = Queue.Queue()

    def __call__(self, event, data=None):
        message, deltatime = event
        self._wallclock += deltatime
        log.debug("IN: @%0.6f %r", self._wallclock, message)
        self.queue.put((message, self._wallclock))

    def run(self):
        log.debug("Attaching MIDI input callback handler.")
        self.midiin.set_callback(self)
        lcd_cursor = 0

        while True:
            event = self.queue.get()

            if event is None:
                break

            #print event
            ((ch, v1, v2), ts) = event

            if ch & 0xe0:
                v = v1 | (v2 << 7)
                if ch == 0xe0:
                    # DATA
                    #print 'DATA', hex(v)
                    if lcd_cursor < len(lcd_buf):
                        lcd_buf[lcd_cursor] = chr(v)
                        lcd_cursor += 1
                elif ch == 0xe1:
                    # CMD
                    if v & 0x80:
                        # SET CURSOR
                        lcd_cursor = v & 0x7f 
                    #print 'CMD', hex(v)
                else:
                    raise Exception(event)

                draw_lcd()

    def stop(self):
        self.queue.put(None)


midiin, inport_name = open_midiport('USB MIDI 4x4 Port 4', "input")
logging.basicConfig(format="%(name)s: %(levelname)s - %(message)s", level=logging.DEBUG if 0 else logging.WARNING,)
dispatcher = MidiDispatcher(midiin, None)

print("Entering main loop. Press Control-C to exit.")
try:
    dispatcher.start()
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    dispatcher.stop()
    dispatcher.join()
    print('')
finally:
    print("Exit.")

    midiin.close_port()
    #midiout.close_port()

    curses.endwin()
    del midiin
#    del midiout

#return 0



# midiout = rtmidi.MidiOut()
# available_ports = midiout.get_ports()

# print available_ports

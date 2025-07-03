
#include <assert.h>

#include "bitop.h"
#include "control.h"
#include "dosbox.h"
#include "inout.h"
#include "jfont.h"
#include "logging.h"
#include "mem.h"
#include "menu.h"
#include "mixer.h"
#include "pc98_artic.h"
#include "pc98_cg.h"
#include "pc98_dac.h"
#include "pc98_gdc.h"
#include "pc98_gdc_const.h"
#include "pci_bus.h"
#include "pic.h"
#include "programs.h"
#include "render.h"
#include "sdlmain.h"
#include "setup.h"
#include "support.h"
#include "timer.h"
#include "util_units.h"
#include "vga.h"
#include "video.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

/* ARTIC (A Relative Time Indication Counter) I/O read.
 * Ports 0x5C and 0x5E.
 * This is required for some MS-DOS drivers such as the OAK CD-ROM driver
 * in order for them to time out properly instead of infinitely hang.
 *
 * "A 24-bit binary counter that counts up at 307.2KHz" */
Bitu pc98_read_artic(Bitu port, Bitu iolen) {
  Bitu count = ((Bitu)(PIC_FullIndex() /*milliseconds*/ * 307.2)) &
               (Bitu)0xFFFFFFul /*mask at 24 bits*/;
  Bitu r = ~0ul;

  if ((port & 0xFFFEul) == 0x5C) /* bits 15:0 */
    r = count & 0xFFFFul;
  else if ((port & 0xFFFEul) == 0x5E) /* bits 23:8 */
    r = (count >> 8u) & 0xFFFFul;

  if (iolen == 1) {
    if (port & 1)
      r >>= 8;
    r &= 0xFF;
  }

  //	LOG_MSG("ARTIC port %x read %x iolen %u",(unsigned int)port,(unsigned
  //int)r,(unsigned int)iolen);

  return r;
}

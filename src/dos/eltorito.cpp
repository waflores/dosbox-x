
#include "dosbox.h"
#include <algorithm>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "bios.h"
#include "bios_disk.h"
#include "builtin.h"
#include "callback.h"
#include "cdrom.h"
#include "control.h"
#include "cpu.h"
#include "cross.h"
#include "dma.h"
#include "dos_inc.h"
#include "dos_system.h"
#include "drives.h"
#include "eltorito.h"
#include "ide.h"
#include "inout.h"
#include "menu.h"
#include "menudef.h"
#include "mouse.h"
#include "programs.h"
#include "qcow2_disk.h"
#include "regs.h"
#include "render.h"
#include "setup.h"
#include "shell.h"
#include "support.h"
#include <time.h>

bool ElTorito_ChecksumRecord(unsigned char *entry /*32 bytes*/) {
  unsigned int chk = 0, i;

  for (i = 0; i < 16; i++) {
    unsigned int word =
        ((unsigned int)entry[0]) + ((unsigned int)entry[1] << 8);
    chk += word;
    entry += 2;
  }
  chk &= 0xFFFF;
  return (chk == 0);
}

bool ElTorito_ScanForBootRecord(CDROM_Interface *drv,
                                unsigned long &boot_record,
                                unsigned long &el_torito_base) {
  unsigned char buffer[2048];
  unsigned int sec;

  for (sec = 16; sec < 32; sec++) {
    if (!drv->ReadSectorsHost(buffer, false, sec, 1))
      break;

    /* stop at terminating volume record */
    if (buffer[0] == 0xFF)
      break;

    /* match boot record and whether it conforms to El Torito */
    if (buffer[0] == 0x00 && memcmp(buffer + 1, "CD001", 5) == 0 &&
        buffer[6] == 0x01 &&
        memcmp(buffer + 7, "EL TORITO SPECIFICATION\0\0\0\0\0\0\0\0\0", 32) ==
            0) {
      boot_record = sec;
      el_torito_base = (unsigned long)buffer[71] +
                       ((unsigned long)buffer[72] << 8UL) +
                       ((unsigned long)buffer[73] << 16UL) +
                       ((unsigned long)buffer[74] << 24UL);

      return true;
    }
  }

  return false;
}

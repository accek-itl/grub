/* debugmem.c - Allocate NVS memory and print its address and content. */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2026, Oracle and/or its affiliates.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/dl.h>
#include <grub/command.h>
#include <grub/misc.h>
#include <grub/memory.h>
#include <grub/types.h>

GRUB_MOD_LICENSE ("GPLv3+");

#define PAGE_SIZE 4096

static grub_err_t
grub_cmd_debugmem (grub_command_t cmd __attribute__ ((unused)),
                   int argc __attribute__ ((unused)),
                   char *argv[] __attribute__ ((unused)))
{
  int handle;
  void *addr;

  addr = grub_mmap_malign_and_register (PAGE_SIZE, PAGE_SIZE, &handle,
                                        GRUB_MEMORY_NVS, 0);
  if (!addr)
    return grub_error (GRUB_ERR_OUT_OF_MEMORY,
                       "failed to allocate NVS page");

  grub_printf ("debugmem: address=%p\n", addr);
  grub_printf ("%s\n", (char *) addr);

  return GRUB_ERR_NONE;
}

static grub_command_t cmd;

GRUB_MOD_INIT (debugmem)
{
  cmd = grub_register_command ("debugmem", grub_cmd_debugmem, 0,
                               N_("Allocate a page of NVS memory and print its address and content."));
}

GRUB_MOD_FINI (debugmem)
{
  grub_unregister_command (cmd);
}

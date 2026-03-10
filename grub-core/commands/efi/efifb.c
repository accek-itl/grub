/* efifb.c - Command to print EFI framebuffer info. */
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
#include <grub/misc.h>
#include <grub/command.h>
#include <grub/efi/efi.h>
#include <grub/efi/graphics_output.h>

GRUB_MOD_LICENSE ("GPLv3+");

static grub_guid_t graphics_output_guid = GRUB_EFI_GOP_GUID;

static grub_err_t
grub_cmd_efifb_info (struct grub_command *cmd __attribute__ ((unused)),
		     int argc __attribute__ ((unused)),
		     char **args __attribute__ ((unused)))
{
  struct grub_efi_gop *gop;
  grub_efi_uintn_t num_handles, i;
  grub_efi_handle_t *handles;

  handles = grub_efi_locate_handle (GRUB_EFI_BY_PROTOCOL,
				    &graphics_output_guid,
				    NULL, &num_handles);
  if (!handles || num_handles == 0)
    return grub_error (GRUB_ERR_FILE_NOT_FOUND,
		       "no EFI GOP handles found");

  for (i = 0; i < num_handles; i++)
    {
      gop = grub_efi_open_protocol (handles[i], &graphics_output_guid,
				    GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
      if (!gop || !gop->mode)
	continue;

      grub_printf ("GOP handle %u:\n", (unsigned) i);
      grub_printf ("  framebuffer base: 0x%016llx\n",
		   (unsigned long long) gop->mode->fb_base);
      grub_printf ("  framebuffer size: 0x%llx (%llu bytes)\n",
		   (unsigned long long) gop->mode->fb_size,
		   (unsigned long long) gop->mode->fb_size);

      if (gop->mode->info)
	{
	  struct grub_efi_gop_mode_info *info = gop->mode->info;
	  const char *fmt;

	  switch (info->pixel_format)
	    {
	    case GRUB_EFI_GOT_RGBA8:
	      fmt = "RGBA8";
	      break;
	    case GRUB_EFI_GOT_BGRA8:
	      fmt = "BGRA8";
	      break;
	    case GRUB_EFI_GOT_BITMASK:
	      fmt = "bitmask";
	      break;
	    case GRUB_EFI_GOT_BLT_ONLY:
	      fmt = "BLT only (no framebuffer)";
	      break;
	    default:
	      fmt = "unknown";
	      break;
	    }

	  grub_printf ("  mode: %ux%u, pixel format: %s, "
		       "pixels per scanline: %u\n",
		       info->width, info->height, fmt,
		       info->pixels_per_scanline);
	}
    }

  grub_free (handles);
  return GRUB_ERR_NONE;
}

static grub_command_t cmd;

GRUB_MOD_INIT (efifb)
{
  cmd = grub_register_command ("efifb_info", grub_cmd_efifb_info, NULL,
			       N_("Print EFI framebuffer address and size."));
}

GRUB_MOD_FINI (efifb)
{
  grub_unregister_command (cmd);
}

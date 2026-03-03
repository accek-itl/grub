/*
 * vtd.c: VT-d support functions
 *
 * Copyright (c) 2019, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2025, Invisible Things Lab.
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

#include <grub/err.h>
#include <grub/i386/mmio.h>
#include <grub/mm.h>
#include <grub/types.h>
#include <grub/acpi.h>
#include <grub/i386/vtd.h>

static struct grub_acpi_dmar *get_vtd_dmar_table(void)
{
    return grub_acpi_find_table(GRUB_ACPI_DMAR_SIGNATURE);
}

bool vtd_bios_enabled(void)
{
    return get_vtd_dmar_table() != NULL; 
}

struct grub_acpi_dmar_remapping *vtd_get_dmar_remap(grub_uint32_t *remap_length)
{
    struct grub_acpi_dmar *dmar = get_vtd_dmar_table();

    if (dmar == NULL || remap_length == NULL) {
        return NULL;
    }

    *remap_length = dmar->hdr.length - sizeof(*dmar);
    return (struct grub_acpi_dmar_remapping*)(dmar->table_offsets);
}

grub_err_t vtd_disable_dma_remap(struct grub_acpi_dmar_remapping *rs)
{
    if (rs->type != GRUB_ACPI_DMAR_REMAPPING_DRHD) {
        return GRUB_ERR_BAD_ARGUMENT;
    }

    grub_uint32_t timeout;
    grub_uint32_t gsts = grub_readl((void *) (rs->register_base_address + VTD_GSTS_OFFSET)) & 0x96FFFFFF;
    
    if (gsts & TE_STAT) {
        /* Clear TE_STAT bit and write back to GCMD */
        gsts &= ~TE_STAT;
        grub_writel(gsts, (void *) (rs->register_base_address + VTD_GCMD_OFFSET));

        /* Wait until GSTS indicates that operation is completed */
        timeout = VTD_OPERATION_TIMEOUT;
        while (grub_readl((void *) (rs->register_base_address + VTD_GSTS_OFFSET)) & TE_STAT) {
            if (--timeout == 0) {
                return GRUB_ERR_TIMEOUT;
            }
        }
    }

    return GRUB_ERR_NONE;
}
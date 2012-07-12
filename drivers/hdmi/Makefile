#
# (C) Copyright 2000-2007
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)libhdmi.a

COBJS-$(CONFIG_NUSMART_HDMI) += hdmi_i2c.o
COBJS-$(CONFIG_CH7033_HDMI) += CH7033.o
COBJS-$(CONFIG_NUSMART_HDMI) += SIITPI.o
COBJS-$(CONFIG_NUSMART_HDMI) += SIIVideoModeTable.o
COBJS-$(CONFIG_NUSMART_HDMI) += SIIAV_Config.o
COBJS-$(CONFIG_NUSMART_HDMI) += SIIEDID.o
COBJS-$(CONFIG_NUSMART_HDMI) += SIIHDCP.o
COBJS-$(CONFIG_NUSMART_HDMI) += Global.o
COBJS-$(CONFIG_NUSMART_HDMI) += SIISiI9022A72.o
COBJS-$(CONFIG_NUSMART_HDMI) += SIITPI_Access.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

#!/bin/sh

mkdir -p ${obj}include
mkdir -p ${obj}board/armltd/integrator

config_file=${obj}include/config.h

if [ "$1" = "ap" ]
then
# ---------------------------------------------------------
# Set the platform defines
# ---------------------------------------------------------
echo -n	"/* Integrator configuration implied "	 > ${config_file}
echo	" by Makefile target */"		>> ${config_file}
echo -n	"#define CONFIG_INTEGRATOR"		>> ${config_file}
echo	" /* Integrator board */"		>> ${config_file}
echo -n	"#define CONFIG_ARCH_INTEGRATOR"	>> ${config_file}
echo	" 1 /* Integrator/AP	 */"		>> ${config_file}
# ---------------------------------------------------------
#	Set the core module defines according to Core Module
# ---------------------------------------------------------
cpu="arm_intcm"
variant="unknown core module"

if [ "$2" = "" ]
then
	echo "$0:: No parameters - using arm_intcm"
else
	variant=`echo $2 | sed -r s/_config//`

	case "$2" in
	ap7_config)
	cpu="arm_intcm"
	;;

	ap966)
	cpu="arm_intcm"
	;;

	ap922_config)
	cpu="arm_intcm"
	;;

	integratorap_config	|	\
	ap_config)
	cpu="arm_intcm"
	;;

	ap720t_config)
	cpu="arm720t"
	echo -n	"#define CONFIG_CM720T"			>> ${config_file}
	echo	" 1 /* CPU core is ARM720T */ "		>> ${config_file}
	;;

	ap922_XA10_config)
	cpu="arm_intcm"
	echo -n	"#define CONFIG_CM922T_XA10"		>> ${config_file}
	echo	" 1 /* CPU core is ARM922T_XA10 */"	>> ${config_file}
	;;

	ap920t_config)
	cpu="arm920t"
	echo -n	"#define CONFIG_CM920T"			>> ${config_file}
	echo	" 1 /* CPU core is ARM920T */"		>> ${config_file}
	;;

	ap926ejs_config)
	cpu="arm926ejs"
	echo -n	"#define CONFIG_CM926EJ_S"		>> ${config_file}
	echo	" 1 /* CPU core is ARM926EJ-S */ "	>> ${config_file}
	;;

	ap946es_config)
	cpu="arm946es"
	echo -n	"#define CONFIG_CM946E_S"		>> ${config_file}
	echo	" 1 /* CPU core is ARM946E-S */ "	>> ${config_file}
	;;

	*)
	echo "$0:: Unknown core module"
	variant="integratorap"
	cpu="arm_intcm"
	;;

	esac
fi

case "$cpu" in
	arm_intcm)
	echo "/* Core module undefined/not ported */"	>> ${config_file}
	echo "#define CONFIG_ARM_INTCM 1"		>> ${config_file}
	echo -n	"#undef CONFIG_CM_MULTIPLE_SSRAM"	>> ${config_file}
	echo -n	"	/* CM may not have "		>> ${config_file}
	echo	"multiple SSRAM mapping */"		>> ${config_file}
	echo -n	"#undef CONFIG_CM_SPD_DETECT "		>> ${config_file}
	echo -n	" /* CM may not support SPD "		>> ${config_file}
	echo	"query */"				>> ${config_file}
	echo -n	"#undef CONFIG_CM_REMAP	"		>> ${config_file}
	echo -n	" /* CM may not support "		>> ${config_file}
	echo	"remapping */"				>> ${config_file}
	echo -n	"#undef CONFIG_CM_INIT	"		>> ${config_file}
	echo -n	" /* CM may not have	"		>> ${config_file}
	echo	"initialization reg */"			>> ${config_file}
	echo -n	"#undef CONFIG_CM_TCRAM	"		>> ${config_file}
	echo	" /* CM may not have TCRAM */"		>> ${config_file}
	echo -n	" /* May not be processor "		>> ${config_file}
	echo	"without cache support */"		>> ${config_file}
	echo	"#define CONFIG_SYS_NO_ICACHE 1"	>> ${config_file}
	echo	"#define CONFIG_SYS_NO_DCACHE 1"	>> ${config_file}
	;;

	arm720t)
	echo -n	" /* May not be processor "		>> ${config_file}
	echo	"without cache support */"		>> ${config_file}
	echo	"#define CONFIG_SYS_NO_ICACHE 1"	>> ${config_file}
	echo	"#define CONFIG_SYS_NO_DCACHE 1"	>> ${config_file}
	;;
esac

else

# ---------------------------------------------------------
# Set the platform defines
# ---------------------------------------------------------
echo -n "/* Integrator configuration implied "   > ${config_file}
echo    " by Makefile target */"		>> ${config_file}
echo -n "#define CONFIG_INTEGRATOR"		>> ${config_file}
echo	 " /* Integrator board */"		>> ${config_file}
echo -n "#define CONFIG_ARCH_CINTEGRATOR"	>> ${config_file}
echo     " 1 /* Integrator/CP   */"		>> ${config_file}

cpu="arm_intcm"
variant="unknown core module"

if [ "$2" = "" ]
then
	echo "$0:: No parameters - using arm_intcm"
else
	variant=`echo $2 | sed -r s/_config//`
	case "$2" in
	cp966_config)
	cpu="arm_intcm"
	;;

	cp922_config)
	cpu="arm_intcm"
	;;

	integratorcp_config	|	\
	cp_config)
	cpu="arm_intcm"
	;;

	cp922_XA10_config)
	cpu="arm_intcm"
	echo -n "#define CONFIG_CM922T_XA10"		>> ${config_file}
	echo    " 1 /* CPU core is ARM922T_XA10 */"	>> ${config_file}
	;;

	cp920t_config)
	cpu="arm920t"
	echo -n "#define CONFIG_CM920T"			>> ${config_file}
	echo    " 1 /* CPU core is ARM920T */"		>> ${config_file}
	;;

	cp926ejs_config)
	cpu="arm926ejs"
	echo -n "#define CONFIG_CM926EJ_S"		>> ${config_file}
	echo    " 1 /* CPU core is ARM926EJ-S */ "	>> ${config_file}
	;;


	cp946es_config)
	cpu="arm946es"
	echo -n "#define CONFIG_CM946E_S"		>> ${config_file}
	echo    " 1 /* CPU core is ARM946E-S */ "	>> ${config_file}
	;;

	cp1026_config)
	cpu="arm_intcm"
	echo -n "#define CONFIG_CM1026EJ_S"		>> tmp.fil
	echo    " 1 /* CPU core is ARM1026J-S */ "	>> tmp.fil
	;;

	cp1136_config)
	cpu="arm1136"
	echo -n "#define CONFIG_CM1136EJF_S"		>> ${config_file}
	echo    " 1 /* CPU core is ARM1136JF-S */ "	>> ${config_file}
	;;

	cp1176_rt_config)
	cpu="arm_intcm"
	echo -n "#define CONFIG_CM1176JFZ_S"		>> tmp.fil
	echo    " 1 /* CPU core is ARM1136JF-S */ "	>> tmp.fil
	echo -n "#define ARM_1176_CP_RT"		>> tmp.fil
	echo    " 1 /* FastSIM model */ "	>> tmp.fil
	;;

	*)
	echo "$0:: Unknown core module"
	variant="unknown core module"
	cpu="arm_intcm"
	;;

	esac

fi

if [ "$cpu" = "arm_intcm" ]
then
	echo "/* Core module undefined/not ported */"	>> ${config_file}
	echo "#define CONFIG_ARM_INTCM 1"		>> ${config_file}
	echo -n "#undef CONFIG_CM_MULTIPLE_SSRAM"	>> ${config_file}
	echo -n "  /* CM may not have "			>> ${config_file}
	echo    "multiple SSRAM mapping */"		>> ${config_file}
	echo -n "#undef CONFIG_CM_SPD_DETECT "		>> ${config_file}
	echo -n " /* CM may not support SPD "		>> ${config_file}
	echo    "query */"				>> ${config_file}
	echo -n "#undef CONFIG_CM_REMAP  "		>> ${config_file}
	echo -n " /* CM may not support "		>> ${config_file}
	echo    "remapping */"				>> ${config_file}
	echo -n "#undef CONFIG_CM_INIT  "		>> ${config_file}
	echo -n " /* CM may not have  "			>> ${config_file}
	echo    "initialization reg */"			>> ${config_file}
	echo -n "#undef CONFIG_CM_TCRAM  "		>> ${config_file}
	echo    " /* CM may not have TCRAM */"		>> ${config_file}
fi

fi # ap

# ---------------------------------------------------------
# Complete the configuration
# ---------------------------------------------------------
$MKCONFIG -a -n "${2%%_config}" integrator$1 arm $cpu integrator armltd
echo "Variant: $variant with core $cpu"

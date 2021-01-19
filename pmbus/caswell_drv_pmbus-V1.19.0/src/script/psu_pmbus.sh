#!/bin/bash

#Environment argument
CONF=/etc/cas-well/psu/pmbus.conf
source $CONF

# Set configurations to get PSU state.
function init_environment() {
	PMBUS_TOOL=../tool/cas_pmb_ctrl
	I2C_BUS=0
	I2C_DET_ADDR=2d
	I2C_ADDR=0x$I2C_DET_ADDR
	PMBUS_DET_ADDR=58
	PMBUS_ADDR=0x$PMBUS_DET_ADDR
	PSU_REG=0x92
	TEMPERATURE_REG=0x14
	VOLTAGE_REG_3=0x10
	VOLTAGE_REG_5=0x11
	VOLTAGE_REG_12=0x12
	OPT_TMP=""
	I2C_TMP=0
}

function print_help(){
	#clear
	echo ""
	echo "Usage of PSU status functionality :"
	echo -e "psu_test.sh [options]\n"
	echo "[options]:"
	echo "  -w  => Show word status."
	echo "  -v  => Show output voltage."
	echo "  -V  => Show input voltage."
	echo "  -i  => Show output current."
	echo "  -I  => Show intput current."
	echo "  -p  => Show output power."
	echo "  -P  => Show output power."
	echo "  -f  => Show fan speed."
	echo "  -t  => Show temperature."
	echo "  -M  => Show mfr information."
	echo "  -h  => Show this message."
	echo ""
}

# Check if the i2c address can be detected.
function psu_bus_check() {
	#Get SMBus adapter
	I2C_BUS=`i2cdetect -l | awk '/smbus/ {print $1}' | cut -d'-' -f2`

	[ "$I2C_BUS" = "" ] \
		&& echo "ERROR: PMBus driver is based on standard linux I2C interface, please make" \
		&& echo "       sure I2C bus driver (i801) is loaded before loading PMBus driver." \
		&& exit 1

	#Get I2C slave address
	echo Y | i2cdetect $I2C_BUS 2>/dev/null | grep $I2C_DET_ADDR >/dev/null
	[ "$?" -ne 0 ] \
		&& echo "ERROR: Can't detect I2C address $I2C_ADDR." && exit 1

	#Check PMBUS driver
	ls /dev/pmbus_ctrl 2>/dev/null | grep pmbus_ctrl >/dev/null
	[ "$?" -ne 0 ] \
		&& echo "ERROR: Please load pmbus driver." && exit 1

	#Check pmbus tool
	ls $PMBUS_TOOL 2>/dev/null | grep $PMBUS_TOOL >/dev/null
	[ "$?" -ne 0 ] \
		&& echo "ERROR: Please compiler pmbus tool." && exit 1
}

# Get PSU Voltage from CR92.
function show_i2c_output_voltage_status() {
	VALUE=$(echo Y | i2cget $I2C_BUS $I2C_ADDR $VOLTAGE_REG_12 2>/dev/null)
	VALUE=$((VALUE*78125*11/100000))
	printf "Output Voltage (12V): %d.%d%d (V)\n" $((VALUE/100)) $(((VALUE/10)%10)) $((VALUE%10))
	VALUE=$(echo Y | i2cget $I2C_BUS $I2C_ADDR $VOLTAGE_REG_5 2>/dev/null)
	VALUE=$((VALUE*78125*6/100000))
	printf "Output Voltage (5V): %d.%d%d (V)\n" $((VALUE/100)) $(((VALUE/10)%10)) $((VALUE%10))
	VALUE=$(echo Y | i2cget $I2C_BUS $I2C_ADDR $VOLTAGE_REG_3 2>/dev/null)
	VALUE=$((VALUE*78125*2/100000))
	printf "Output Voltage (3.3V): %d.%d%d (V)\n\n" $((VALUE/100)) $(((VALUE/10)%10)) $((VALUE%10))
	printf "VOUT Status: Not Supported.\n\n"
}

# Get PSU Temperature from CR92.
function show_i2c_temperature_status() {
	VALUE=$(echo Y | i2cget $I2C_BUS $I2C_ADDR $TEMPERATURE_REG 2>/dev/null)
	printf "Temperature 1: %d.0 (Cel.)\n\n" $VALUE
	printf "Temperature Status: Not Supported.\n\n"
}

function i2c_get_data() {
	if [ "$((MODULE_PRESENT_MASK&0x80))" -eq 0 ]
	then
		if [ "$I2C_TMP" -gt 0 ]
		then
			printf "[Backplane]\n\n"
			if [ "$((I2C_TMP&0x1))" -ne 0 ]
			then
				show_i2c_output_voltage_status
			fi

			if [ "$((I2C_TMP&0x2))" -ne 0 ]
			then
				show_i2c_temperature_status
			fi
		fi
	fi
}

init_environment
psu_bus_check
while getopts "wvViIpPtfMhH:" opt
do
	case $opt in
	w)
		OPT_TMP+="w"
	;;
	v)
		OPT_TMP+="v"
		I2C_TMP=$((I2C_TMP+1))
	;;
	V)
		OPT_TMP+="V"
	;;
	i)
		OPT_TMP+="i"
	;;
	I)
		OPT_TMP+="I"
	;;
	p)
		OPT_TMP+="p"
	;;
	P)
		OPT_TMP+="P"
	;;
	f)
		OPT_TMP+="f"
	;;
	t)
		OPT_TMP+="t"
		I2C_TMP=$((I2C_TMP+2))
	;;
	M)
		OPT_TMP+="M"
	;;
	h|H)
		print_help
	;;
	*)
		echo "Invalid option $opt"
		print_help
	;;
	esac
done

if [ -n "$OPT_TMP" ]
then
	$PMBUS_TOOL -$OPT_TMP
	i2c_get_data
fi
[ "$OPTIND" = 1 ] && print_help

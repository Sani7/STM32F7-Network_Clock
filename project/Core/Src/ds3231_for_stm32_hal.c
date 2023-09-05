
/* An STM32 HAL library written for the DS3231 real-time clock IC. */
/* Library by @eepj www.github.com/eepj */
#include <stdint.h>
#include "ds3231_for_stm32_hal.h"
#include "main.h"
#ifdef __cplusplus
extern "C"{
#endif

I2C_HandleTypeDef *_ds3231_ui2c;

/**
 * @brief Initializes the DS3231 module. Set clock halt bit to 0 to start timing.
 * @param hi2c User I2C handle pointer.
 */
void DS3231_Init(I2C_HandleTypeDef *hi2c) {
	_ds3231_ui2c = hi2c;
	DS3231_EnableAlarm1(DS3231_DISABLED);
	DS3231_EnableAlarm2(DS3231_DISABLED);
	DS3231_ClearAlarm1Flag();
	DS3231_ClearAlarm2Flag();
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}

/**
 * @brief Set the byte in the designated DS3231 register to value.
 * @param regAddr Register address to write.
 * @param val Value to set, 0 to 255.
 */
void DS3231_SetRegByte(uint8_t regAddr, uint8_t val) {
	uint8_t bytes[2] = { regAddr, val };
	HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, bytes, 2, DS3231_TIMEOUT);
}

/**
 * @brief Gets the byte in the designated DS3231 register.
 * @param regAddr Register address to read.
 * @return Value stored in the register, 0 to 255.
 */
uint8_t DS3231_GetRegByte(uint8_t regAddr) {
	uint8_t val;
	HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &regAddr, 1, DS3231_TIMEOUT);
	HAL_I2C_Master_Receive(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &val, 1, DS3231_TIMEOUT);
	return val;
}

/**
 * @brief Enables battery-backed square wave output at the INT#/SQW pin.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableBatterySquareWave(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xbf) | ((enable & 0x01) << DS3231_BBSQW));
}

/**
 * @brief Set the interrupt mode to either alarm interrupt or square wave interrupt.
 * @param mode Interrupt mode to set, DS3231_ALARM_INTERRUPT or DS3231_SQUARE_WAVE_INTERRUPT.
 */
void DS3231_SetInterruptMode(DS3231_InterruptMode mode){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfb) | ((mode & 0x01) << DS3231_INTCN));
}

/**
 * @brief Set frequency of the square wave output
 * @param rate Frequency to set, DS3231_1HZ, DS3231_1024HZ, DS3231_4096HZ or DS3231_8192HZ.
 */
void DS3231_SetRateSelect(DS3231_Rate rate){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xe7) | ((rate & 0x03) << DS3231_RS1));
}

/**
 * @brief Enables clock oscillator.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableOscillator(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0x7f) | ((!enable & 0x01) << DS3231_EOSC));
}

/**
 * @brief Enables alarm 2.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableAlarm2(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfd) | ((enable & 0x01) << DS3231_A2IE));
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}

/**
 * @brief Clears alarm 2 matched flag. Matched flags must be cleared before the next match or the next interrupt will be masked.
 */
void DS3231_ClearAlarm2Flag(){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfd;
	DS3231_SetRegByte(DS3231_REG_STATUS, status & ~(0x01 << DS3231_A2F));
}

/**
 * @brief Sets alarm 2
 * 
 * @param time only the hour, minute, day and Wday fields are used
 */
void DS3231_SetAlarm2Time(ts* time)
{
    uint8_t temp = DS3231_GetRegByte(DS3231_A2_MINUTE) & 0x80;
	uint8_t a2 = temp | (DS3231_EncodeBCD(time->Minute) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_MINUTE, a2);

    temp = DS3231_GetRegByte(DS3231_A2_HOUR) & 0x80;
	a2 = temp | (DS3231_EncodeBCD(time->Hour) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_HOUR, a2);

    temp = DS3231_GetRegByte(DS3231_A2_DATE) & 0x80;
	a2 = temp | (DS3231_EncodeBCD(time->Day) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_DATE, a2);

    temp = DS3231_GetRegByte(DS3231_A2_DATE) & 0x80;
	a2 = temp | (0x01 << DS3231_DYDT) | (DS3231_EncodeBCD(time->Wday) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_DATE, a2);
}

/**
 * @brief Set alarm 2 mode.
 * @param alarmMode Alarm 2 mode, DS3231_A2_EVERY_M, DS3231_A2_MATCH_M, DS3231_A2_MATCH_M_H, DS3231_A2_MATCH_M_H_DATE or DS3231_A2_MATCH_M_H_DAY.
 */
void DS3231_SetAlarm2Mode(DS3231_Alarm2Mode alarmMode){
	uint8_t temp;
	temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_MINUTE, temp | (((alarmMode >> 0) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_HOUR, temp | (((alarmMode >> 1) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_DATE, temp | (((alarmMode >> 2) & 0x01) << DS3231_AXMY) | (alarmMode & 0x80));
}

/**
 * @brief Enables alarm 1.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableAlarm1(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfe) | ((enable & 0x01) << DS3231_A1IE));
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}

/**
 * @brief Clears alarm 1 matched flag. Matched flags must be cleared before the next match or the next interrupt will be masked.
 */
void DS3231_ClearAlarm1Flag(){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfe;
	DS3231_SetRegByte(DS3231_REG_STATUS, status & ~(0x01 << DS3231_A1F));
}

/**
 * @brief Sets alarm 1
 * 
 * @param time only the hour, minute, second, day and Wday fields are used
 */
void DS3231_SetAlarm1Time(ts* time)
{
    uint8_t temp = DS3231_GetRegByte(DS3231_A1_SECOND) & 0x80;
	uint8_t a1 = temp | (DS3231_EncodeBCD(time->Second) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_SECOND, a1);

    temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x80;
	a1 = temp | (DS3231_EncodeBCD(time->Minute) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_MINUTE, a1);

    temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x80;
	a1 = temp | (DS3231_EncodeBCD(time->Hour) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_HOUR, a1);

    temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x80;
	a1 = temp | (DS3231_EncodeBCD(time->Day) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_DATE, a1);

    temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x80;
	a1 = temp | (0x01 << DS3231_DYDT) | (DS3231_EncodeBCD(time->Wday) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_DATE, a1);
}

/**
 * @brief Set alarm 1 mode.
 * @param alarmMode Alarm 1 mode, DS3231_A1_EVERY_S, DS3231_A1_MATCH_S, DS3231_A1_MATCH_S_M, DS3231_A1_MATCH_S_M_H, DS3231_A1_MATCH_S_M_H_DATE or DS3231_A1_MATCH_S_M_H_DAY.
 */
void DS3231_SetAlarm1Mode(DS3231_Alarm1Mode alarmMode){
	uint8_t temp;
	temp = DS3231_GetRegByte(DS3231_A1_SECOND) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_SECOND, temp | (((alarmMode >> 0) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_MINUTE, temp | (((alarmMode >> 1) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_HOUR, temp | (((alarmMode >> 2) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_DATE, temp | (((alarmMode >> 3) & 0x01) << DS3231_AXMY) | (alarmMode & 0x80));
}

/**
 * @brief Check whether the clock oscillator is stopped.
 * @return Oscillator stopped flag (OSF) bit, 0 or 1.
 */
uint8_t DS3231_IsOscillatorStopped(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_OSF) & 0x01;
}

/**
 * @brief Check whether the 32kHz output is enabled.
 * @return EN32kHz flag bit, 0 or 1.
 */
uint8_t DS3231_Is32kHzEnabled(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_EN32KHZ) & 0x01;
}

/**
 * @brief Check if alarm 1 is triggered.
 * @return A1F flag bit, 0 or 1.
 */
uint8_t DS3231_IsAlarm1Triggered(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_A1F) & 0x01;
}

/**
 * @brief Check if alarm 2 is triggered.
 * @return A2F flag bit, 0 or 1.
 */
uint8_t DS3231_IsAlarm2Triggered(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_A2F) & 0x01;
}
/**
 * @brief Get the current time->
 * 
 * @return ts Second, Minute, Hour, Day, Wday, Month, Year fields are set.
 */
void DS3231_GetTime(ts* time)
{
    uint8_t decYear;
    uint16_t century;
    time->Second = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_SECOND));
    time->Minute = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MINUTE));
    time->Hour = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_HOUR) & 0x3f);
    time->Day = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DATE));
    time->Wday = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DOW));
    time->Month = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f);
    decYear = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_YEAR));
	century = (DS3231_GetRegByte(DS3231_REG_MONTH) >> DS3231_CENTURY) * 100 + 2000;
    time->Year = decYear + century - 1970;
}

/**
 * @brief Set the current time->
 * 
 * @param time Second, Minute, Hour, Day, Wday, Month, Year fields are used.
 */
void DS3231_SetTime(ts* time)
{
    uint8_t century;
    uint8_t monthReg;
    DS3231_SetRegByte(DS3231_REG_SECOND, DS3231_EncodeBCD(time->Second));
    DS3231_SetRegByte(DS3231_REG_MINUTE, DS3231_EncodeBCD(time->Minute));
    DS3231_SetRegByte(DS3231_REG_HOUR, DS3231_EncodeBCD(time->Hour));
    DS3231_SetRegByte(DS3231_REG_DATE, DS3231_EncodeBCD(time->Day));
    DS3231_SetRegByte(DS3231_REG_DOW, DS3231_EncodeBCD(time->Wday));
    DS3231_SetRegByte(DS3231_REG_MONTH, DS3231_EncodeBCD(time->Month));
    century = ((time->Year + 1970) / 100) % 20;
	monthReg = (DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f) | (century << DS3231_CENTURY);
	DS3231_SetRegByte(DS3231_REG_MONTH, monthReg);
	DS3231_SetRegByte(DS3231_REG_YEAR, DS3231_EncodeBCD((time->Year + 1970) % 100));
}

/**
 * @brief Decodes the raw binary value stored in registers to decimal format.
 * @param bin Binary-coded decimal value retrieved from register, 0 to 255.
 * @return Decoded decimal value.
 */
uint8_t DS3231_DecodeBCD(uint8_t bin) {
	return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

/**
 * @brief Encodes a decimal number to binaty-coded decimal for storage in registers.
 * @param dec Decimal number to encode.
 * @return Encoded binary-coded decimal value.
 */
uint8_t DS3231_EncodeBCD(uint8_t dec) {
	return (dec % 10 + ((dec / 10) << 4));
}

/**
 * @brief Enable the 32kHz output.
 * @param enable Enable, DS3231_ENABLE or DS3231_DISABLE.
 */
void DS3231_Enable32kHzOutput(DS3231_State enable){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfb;
	DS3231_SetRegByte(DS3231_REG_STATUS, status | (enable << DS3231_EN32KHZ));
}

/**
 * @brief Get the integer part of the temperature.
 * @return Integer part of the temperature, -127 to 127.
 */
int8_t DS3231_GetTemperatureInteger(){
	return DS3231_GetRegByte(DS3231_TEMP_MSB);
}

/**
 * @brief Get the fractional part of the temperature to 2 decimal places.
 * @return Fractional part of the temperature, 0, 25, 50 or 75.
 */
uint8_t DS3231_GetTemperatureFraction(){
	return (DS3231_GetRegByte(DS3231_TEMP_LSB) >> 6) * 25;
}

#ifdef __cplusplus
}
#endif

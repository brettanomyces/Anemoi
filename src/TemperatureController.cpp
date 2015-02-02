#include <Arduino.h>
#include "TemperatureController.h"

TemperatureController::TemperatureController(
		Baffel& baffel,
		Relay& compressorRelay,
		Relay& fanRelay,
		Relay& heaterRelay,
		TemperatureSensor& freezerSensor,
		TemperatureSensor& fridgeSensor
		):
		m_baffel(baffel),
		m_compressorRelay(compressorRelay),
		m_fanRelay(fanRelay),
		m_heaterRelay(heaterRelay),
		m_freezerSensor(freezerSensor),
		m_fridgeSensor(fridgeSensor)
		{
	// default values
	m_fridgeSetValue = 20.0;
	m_freezerSetValue = 10.0;
	m_differenceSetValue = 0.5;
	m_compressorDelayTime = 180000; // millis
	m_compressorTurnedOff = millis();
};

void TemperatureController::setFridgeTemperature(double temperature){
	m_fridgeSetValue = temperature;
}

double TemperatureController::getFrSetTemp(){
	return m_fridgeSetValue;
}

void TemperatureController::setFreezerTemperature(double temperature){
	m_freezerSetValue = temperature;
}

double TemperatureController::getFzSetTemp(){
	return m_freezerSetValue;
}

void TemperatureController::setCompressorDelayTime(unsigned long millis){
	m_compressorDelayTime = millis;
}

void TemperatureController::setDifference(double degrees){
	m_differenceSetValue = degrees;
}

void TemperatureController::maintainTemperature(){
	double currentFridgeTemperature = m_fridgeSensor.readTemperature();
	double currentFreezerTemperature = m_freezerSensor.readTemperature();

	if (currentFridgeTemperature > m_fridgeSetValue + m_differenceSetValue) {
		// fridge to hot
		m_heaterRelay.off();
			if(!m_baffel.isOpen()){
			m_baffel.open();
		}
		m_fanRelay.on();
	} else if (currentFridgeTemperature < m_fridgeSetValue - m_differenceSetValue){
		// fridge to cold
		m_heaterRelay.on();
		if(m_baffel.isOpen()){
			m_baffel.close();
		}
		if (!m_compressorRelay.isOn()){
			m_fanRelay.off();
		}
	} else {
		// fridge temp in ok range
		m_heaterRelay.off();
		if(m_baffel.isOpen()){
			m_baffel.close();
		}
		if (!m_compressorRelay.isOn()){
			m_fanRelay.off();
		}
	}

	if (currentFreezerTemperature > m_freezerSetValue + m_differenceSetValue){
		// freezer to hot
		// wait m_differenceSetValue after turning off compressor to turn it back on
		unsigned long currentMillis = millis();
		if ((unsigned long)(currentMillis - m_compressorTurnedOff) >= m_compressorDelayTime) {
			m_compressorRelay.on();
			m_fanRelay.on();
		}
	} else if (currentFreezerTemperature < m_freezerSetValue - m_differenceSetValue){
		// freezer to cold
		if(m_compressorRelay.isOn()){
			unsigned long m_compressorTurnedOff = millis();
			m_compressorRelay.off();
		}
		if(!m_baffel.isOpen()){
			m_fanRelay.off();
		}
		// We don't worry about heating the freezer section as it doesn't really
		// matter if its a bit too cold and we save power.
	} else {
		// freezer temp in ok range
		if(m_compressorRelay.isOn()){
			unsigned long m_compressorTurnedOff = millis();
			m_compressorRelay.off();
		}
		if(!m_baffel.isOpen()){
			m_fanRelay.off();
		}
	}
}

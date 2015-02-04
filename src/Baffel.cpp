#include <Arduino.h>
#include "Baffel.h"

Baffel::Baffel(	int ena, int in1, int in2, int in3, int in4, int enb, int transformerPin ) : 
	m_stepper(STEPS, in1, in2, in3, in4),
        m_transformer(transformerPin) {
	
	pinMode(ena, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	pinMode(enb, OUTPUT);

	m_ena = ena;
	m_enb = enb;
	m_stepper.setSpeed(SPEED);
}

void Baffel::open(){
	// Check that the baffel is not already open
	if(!isOpen()){
		enable();
		m_stepper.step(6 * STEPS);
		delay(TRAVEL_TIME);
		disable();
		m_open = true;
	}
}

void Baffel::close(){
	if(isOpen()){
		enable();
		m_stepper.step(-6 * STEPS);
		delay(TRAVEL_TIME);
		disable();
		m_open = false;
	}
}

void Baffel::enable(){
	m_transformer.on();
	delay(100);
	digitalWrite(m_ena, HIGH);
	digitalWrite(m_enb, HIGH);
}

void Baffel::disable(){
	digitalWrite(m_ena, LOW);
	digitalWrite(m_enb, LOW);
	m_transformer.off();
}

bool Baffel::isOpen(){
	return m_open;
}

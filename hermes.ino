#include <DoEvery.h>
#include <Baffel.h>
#include <Stepper.h>
#include <Delay.h>
#include <Relay.h>
#include <TemperatureController.h>
#include <TemperatureSensor.h>

// how often do we check the temp
int UPDATE_PERIOD = 10000;  // 10 seconds
double FR_SET_TEMP = 19.0;
double FZ_SET_TEMP = 4.0;
double COMP_DELAY = 300000;  // 5 minutes
double FAN_DELAY = 30000;  // 30 seconds

DoEvery updateTimer(UPDATE_PERIOD);
TemperatureSensor fridgeSensor(2, 2, 10000);
TemperatureSensor freezerSensor(3, 2, 10000);
Baffel baffel(13, 12, 11, 10, 9, 8, 4);
Relay compressor(5, "compressor", COMP_DELAY);
Relay fan(6, "fan", 0);
Relay heater(7, "heater", FAN_DELAY);  // 30 seconds
TemperatureController controller(
    baffel,
    compressor,
    fan,
    heater,
    freezerSensor,
    fridgeSensor);

void setup() {
  // Listen on serial connection for messages from the pc
  Serial.begin(9600);

  // set state of components
  baffel.forceClose();
  compressor.off();
  fan.off();
  heater.off();

  // init timers
  updateTimer.reset();

  // set temps
  controller.setFzSetTemp(FZ_SET_TEMP);
  controller.setFrSetTemp(FR_SET_TEMP);
}

void loop() {
  if (updateTimer.check()) {
    controller.maintainTemperature();
    // output values in the following csv format:
    // frs, fr, fzs, fz, b, c, cw, f, h, hw
    String SEPERATOR = ",";
    Serial.print(controller.getFrSetTemp());
    Serial.print(SEPERATOR);
    Serial.print(fridgeSensor.readTemperature());
    Serial.print(SEPERATOR);
    Serial.print(controller.getFzSetTemp());
    Serial.print(SEPERATOR);
    Serial.print(freezerSensor.readTemperature());
    Serial.print(SEPERATOR);
    Serial.print(baffel.isOpen());
    Serial.print(SEPERATOR);
    Serial.print(compressor.isOn());
    Serial.print(SEPERATOR);
    Serial.print(compressor.waiting());
    Serial.print(SEPERATOR);
    Serial.print(fan.isOn());
    Serial.print(SEPERATOR);
    Serial.print(heater.isOn());
    Serial.print(SEPERATOR);
    Serial.print(heater.waiting());
    Serial.println();
  }
}
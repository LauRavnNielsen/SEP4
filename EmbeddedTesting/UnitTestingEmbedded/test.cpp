#include "gtest/gtest.h"
#include "../fff-master/fff.h"
DEFINE_FFF_GLOBALS

extern "C" {
#include "../UnitTestingEmbedded/Drivers/hih8120.h"
#include "CO2.h"
#include "../UnitTestingEmbedded/Drivers/mh_z19.h"
#include "Temperature_Humidity.h"
#include "../UnitTestingEmbedded/Drivers/serial.h"
#include <stdbool.h>
#include "Sound.h"
#include "Servo.h"
#include "DataPacketHandler.h"
}

uint16_t temp;

//FAKE_VOID_FUNC(vTaskDelay, TickType_t);


class SoundTest : public ::testing::Test {
protected:
	Sound_t soundSensor;
	void SetUp() override {
		soundSensor = sound_create();
	}

	void TearDown() override {

	}
};

FAKE_VALUE_FUNC(mh_z19_return_code_t, mh_z19_take_meassuring);
FAKE_VALUE_FUNC(mh_z19_return_code_t, mh_z19_get_co2_ppm, uint16_t*);

class CO2Test : public ::testing::Test {
protected:
	CO2_t CO2Sensor;
	void SetUp() override {
		CO2Sensor = co2_create();
		RESET_FAKE(mh_z19_take_meassuring);
		RESET_FAKE(mh_z19_get_co2_ppm)
			FFF_RESET_HISTORY();
	}
	void TearDown() override {

	}

};

FAKE_VALUE_FUNC(hih8120DriverReturnCode_t, hih8120Create);

class THTest : public ::testing::Test {
protected:
	Temperature_Humidity_t THSensor;

	void SetUp() override {
		THSensor = th_create();
		RESET_FAKE(hih8120Create);

		FFF_RESET_HISTORY();
	}
	void TearDown() override {}
};

FAKE_VOID_FUNC(rsServoCreate);
FAKE_VOID_FUNC(rcServoSet, uint8_t, uint8_t);

class ServoTest : public ::testing::Test {
protected:
	Servo_t servo;
	void SetUp() override {
		servo = servo_create();
		RESET_FAKE(rcServoSet);
		FFF_RESET_HISTORY();
	}
	void TearDown() override {}
};

class DPHTest : public ::testing::Test {
protected:
	DataPacketHandler_t dph;
	void SetUp() override {
		dph = dph_create();

	}
	void TearDown() {}
};
TEST_F(SoundTest, Sound_Sensor_Creation) {
	EXPECT_TRUE(soundSensor != NULL);

	//also testing sound_getData here
	ASSERT_EQ((uint16_t)-1, sound_getData(soundSensor));
}

TEST_F(SoundTest, Sound_Measure) {
	sound_measure(soundSensor);

	//also testing sound_getData here
	ASSERT_TRUE(sound_getData(soundSensor) > 0 && sound_getData(soundSensor) < 151);
}

TEST_F(CO2Test, CO2_CREATE_AND_GET_CO2) {
	ASSERT_TRUE(NULL != CO2Sensor);
	EXPECT_EQ(0, co2_getData(CO2Sensor));
}

TEST_F(CO2Test, CO2_MEASURE) {
	co2_measureData();
	ASSERT_EQ(1, mh_z19_take_meassuring_fake.call_count);
}

TEST_F(CO2Test, CO2_SET_DATA) {
	co2_setData(CO2Sensor);
	ASSERT_EQ(1, mh_z19_get_co2_ppm_fake.call_count);
}

TEST_F(THTest, DRIVER_INIT) {
	th_initDriver();
	ASSERT_EQ(1, hih8120Create_fake.call_count);
}

TEST_F(THTest, TH_CREATE_TH_GETHUM_TH_GETTEMP) {
	th_initDriver();
	ASSERT_EQ(1, hih8120Create_fake.call_count);
	ASSERT_TRUE(NULL != THSensor);
	ASSERT_EQ((uint16_t)-1, th_getHum(THSensor));
	ASSERT_EQ((uint16_t)-274, th_getTemp(THSensor));
}

TEST_F(ServoTest, SERVO_CREATE) {
	servo_create();

	//ASSERT_EQ(1, rsServoCreate_fake.call_count);
	ASSERT_TRUE(NULL != servo);
}

TEST_F(ServoTest, SERVO_OPEN) {
	servo_openWindow(servo);
	//ASSERT_EQ(1, rcServoSet_fake.call_count);
	
}

TEST_F(ServoTest, SERVO_CLOSE) {
	servo_closeWindow(servo);
	//ASSERT_EQ(1, rcServoSet_fake.call_count);

}

TEST_F(ServoTest, SERVO_OPEN_HALF) {
	servo_halfOpenWindow(servo);
	//ASSERT_EQ(1, rcServoSet_fake.call_count);

}

TEST_F(DPHTest, DPH_CREATE) {
	ASSERT_TRUE(NULL != dph);
}

TEST_F(DPHTest, DPH_SETTERS) {
	dph_setTemperature(16, dph);
	dph_setHumidity(16,dph);
	dph_setDb(16,dph);
	dph_setCO2(16,dph);
}

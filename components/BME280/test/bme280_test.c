#define UNITY_SUPPORT_TEST_CASES
#include <limits.h>
#include "unity.h"
#include <unity_fixture.h>
#include <bme280.h>

struct dataSensor_st
{
    uint64_t timeStamp;

    float temperature;
    float humidity;
    float pressure;

    uint32_t pm1_0;
    uint32_t pm2_5;
    uint32_t pm10;

    uint32_t CO2;
#if (0)
    uint32_t NO;
    uint32_t CO;
    uint32_t SO2;
#endif
};

TEST_CASE("bme_bmp280_test_group", "[bme280_init_test]")
{
    bmp280_t bme280_device;
    bmp280_params_t bme280_params;
    esp_err_t is_Init_ok;
    i2cdev_init();
    is_Init_ok = bme280_init(&bme280_device, &bme280_params, BME280_ADDRESS,
                             CONFIG_BME_I2C_PORT, CONFIG_BME_PIN_NUM_SDA, CONFIG_BME_PIN_NUM_SCL);
    bmp280_free_desc(&bme280_device);
    i2cdev_done();
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_Init_ok, "bme280 isn't initilalized successfully");
}

TEST_CASE("bme_bmp280_test_group", "[bme280_read_data_test]")
{
    bmp280_t bme280_device;
    bmp280_params_t bme280_params;
    struct dataSensor_st data_read_test;
    esp_err_t is_read_ok;
    bool is_read_true = true;
    i2cdev_init();
    bme280_init(&bme280_device, &bme280_params, BME280_ADDRESS,
                CONFIG_BME_I2C_PORT, CONFIG_BME_PIN_NUM_SDA, CONFIG_BME_PIN_NUM_SCL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    is_read_ok = bmp280_read_float(&bme280_device, &(data_read_test.temperature), &(data_read_test.pressure), &(data_read_test.humidity));

    if (is_read_ok != ESP_OK)
    {
        bmp280_free_desc(&bme280_device);
        i2cdev_done();
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_read_ok, "bme280 isn't initilalized successfully");
    }
    else
    {
        if ((data_read_test.temperature < 0) || (data_read_test.temperature > 65) || (data_read_test.pressure < 30000) || (data_read_test.pressure > 1100000) || (data_read_test.humidity < 0) || (data_read_test.humidity > 100))
        {
            is_read_true = false;
        }

        bmp280_free_desc(&bme280_device);
        i2cdev_done();
        TEST_ASSERT_EQUAL_MESSAGE(true, is_read_true, "bme280 return wrong data values");
    }
}

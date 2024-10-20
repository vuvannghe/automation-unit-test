/* Main file of the test application.
 * In this example, it runs unit tests using the Unity framework.
 * However, other types of test applications could also be used.
 */
#include "unity.h"
#include "unity_fixture.h"
#include "testable.h"
#include "time.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "freertos/event_groups.h"

#include "pcf8574.h"
#include "ds3231.h"
#include "DS3231Time.h"
#include "bme280.h"
#include "datamanager.h"
#include "pms7003.h"

#define countof(x) (sizeof(x) / sizeof(x[0]))

i2c_dev_t pcf8574_test; // for testing component pcf8574

// Test group setup
TEST_GROUP(testable);
TEST_SETUP(testable)
{
}

// Testcases
TEST_TEAR_DOWN(testable)
{
}

TEST(testable, mean_of_empty)
{
    const int values[] = {0};
    TEST_ASSERT_EQUAL(0, testable_mean(values, 0));
}

TEST(testable, mean_of_vector)
{
    const int v[] = {1, 3, 5, 7, 9};
    TEST_ASSERT_EQUAL(5, testable_mean(v, countof(v)));
}

// This test is failing (for demonstration purposes), ignore it
IGNORE_TEST(testable, test_fail)
{
    const int v1[] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
    TEST_ASSERT_EQUAL(INT_MAX, testable_mean(v1, countof(v1)));
}

// Test group runner
TEST_GROUP_RUNNER(testable)
{
    RUN_TEST_CASE(testable, mean_of_empty);
    RUN_TEST_CASE(testable, mean_of_vector);
    RUN_TEST_CASE(testable, test_fail);
}
// Test group for component pcf8574
TEST_GROUP(pcf8574_test_group);
TEST_SETUP(pcf8574_test_group)
{
}

// Testcases
TEST_TEAR_DOWN(pcf8574_test_group)
{
}

TEST(pcf8574_test_group, pcf8574_init_returned_desc_test)
{
    i2c_dev_t test;
    bool is_Desc_true = true;
    pcf8574_init_desc(&test, PCF8574_ADDR, PCF8574_I2C_PORT, PCF8574_SDA, PCF8574_SCL);
    if ((test.addr != PCF8574_ADDR) && (test.port != PCF8574_I2C_PORT) && (test.cfg.scl_io_num != PCF8574_SCL) && (test.cfg.sda_io_num != PCF8574_SDA))
    {
        is_Desc_true = false;
    }
    pcf8574_free_desc(&test);
    TEST_ASSERT_EQUAL_MESSAGE(true, is_Desc_true, "pcf8574_init_desc returns a wrong device descriptor");
}

TEST(pcf8574_test_group, pcf8574_init_returned_value_test)
{
    i2c_dev_t test;
    test.mutex = NULL;
    bool is_Returned_OK = true;
    pcf8574_init_desc(&test, PCF8574_ADDR, PCF8574_I2C_PORT, PCF8574_SDA, PCF8574_SCL);
    if (test.mutex == NULL)
    {
        is_Returned_OK = false;
    }
    pcf8574_free_desc(&test);
    TEST_ASSERT_EQUAL_MESSAGE(true, is_Returned_OK, "pcf8574_init_desc doesn't return Mutex");
}

TEST(pcf8574_test_group, pcf8574_free_desc_test)
{
    i2c_dev_t test;
    test.mutex = NULL;
    bool is_Returned_OK = true;
    test.mutex = xSemaphoreCreateMutex();
    if (pcf8574_free_desc(&test) != ESP_OK)
    {
        is_Returned_OK = false;
    }
    TEST_ASSERT_EQUAL_MESSAGE(true, is_Returned_OK, "pcf8574_free_desc doesn't free Mutex");
}

TEST(pcf8574_test_group, pcf8574_portwrite)
{

    esp_err_t actual;
    uint8_t pcf8574_write_1 = 0xA6;
    uint8_t pcf8574_write_2 = 0x59;
    uint8_t pcf8574_write_3 = 0xFF;
    i2cdev_init();
    actual = pcf8574_init_desc(&pcf8574_test, PCF8574_ADDR, PCF8574_I2C_PORT, PCF8574_SDA, PCF8574_SCL);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, actual, "pcf8574_init_desc failed");
    if (actual == ESP_OK)
    {
        actual = pcf8574_port_write(&pcf8574_test, pcf8574_write_1);
        if (actual != ESP_OK)
            goto assert;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        actual = pcf8574_port_write(&pcf8574_test, pcf8574_write_2);
        if (actual != ESP_OK)
            goto assert;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        for (int i = 0; i <= 7; i++)
        {
            pcf8574_write_3 = pcf8574_write_3 << 1;
            actual = pcf8574_port_write(&pcf8574_test, pcf8574_write_3);
            if (actual != ESP_OK)
                break;
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        actual = pcf8574_port_write(&pcf8574_test, 0xFF);
    assert:
        i2cdev_done();
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, actual, "pcf8574_write_port unsuccessfully");
    }
}

TEST(pcf8574_test_group, pcf8574_portread)
{

    esp_err_t actual;
    uint8_t actual_read;
    bool is_read_true = true;
    uint8_t pcf8574_write_1 = 0xA6;
    uint8_t pcf8574_write_2 = 0x59;
    uint8_t pcf8574_write_3 = 0xFF;
    i2cdev_init();
    actual = pcf8574_init_desc(&pcf8574_test, PCF8574_ADDR, PCF8574_I2C_PORT, PCF8574_SDA, PCF8574_SCL);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, actual, "pcf8574_init_desc failed");
    if (actual == ESP_OK)
    {
        pcf8574_port_write(&pcf8574_test, pcf8574_write_1);
        actual = pcf8574_port_read(&pcf8574_test, &actual_read);
        if (actual != ESP_OK)
            goto assert2;
        if (actual_read != pcf8574_write_1)
        {
            is_read_true = false;
            goto assert;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        pcf8574_port_write(&pcf8574_test, pcf8574_write_2);
        actual = pcf8574_port_read(&pcf8574_test, &actual_read);
        if (actual != ESP_OK)
            goto assert2;
        if (actual_read != pcf8574_write_2)
        {
            is_read_true = false;
            goto assert;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        pcf8574_port_write(&pcf8574_test, pcf8574_write_3);
        actual = pcf8574_port_read(&pcf8574_test, &actual_read);
        if (actual != ESP_OK)
            goto assert2;
        if (actual_read != pcf8574_write_3)
        {
            is_read_true = false;
            goto assert;
        }
        i2cdev_done();
    assert:
        TEST_ASSERT_EQUAL_MESSAGE(true, is_read_true, "pcf8574_read_port failed: Read wrong value");
        goto end;
    assert2:
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, actual, "pcf8574_read_port failed: Read unsuccessfully");
    end:
    }
}

// Test group runner
TEST_GROUP_RUNNER(pcf8574_test_group)
{
    RUN_TEST_CASE(pcf8574_test_group, pcf8574_init_returned_value_test);
    RUN_TEST_CASE(pcf8574_test_group, pcf8574_init_returned_desc_test);
    RUN_TEST_CASE(pcf8574_test_group, pcf8574_free_desc_test);
    RUN_TEST_CASE(pcf8574_test_group, pcf8574_portwrite);
    RUN_TEST_CASE(pcf8574_test_group, pcf8574_portread);
}

// Test group for component ds3231
TEST_GROUP(ds3231_test_group);
TEST_SETUP(ds3231_test_group)
{
}

TEST_TEAR_DOWN(ds3231_test_group)
{
}

TEST(ds3231_test_group, ds3231_init_returned_desc_test)
{
    i2c_dev_t test;
    bool is_Desc_true = true;
    ds3231_init_desc(&test, DS3232_I2C_PORT, DS3232_PIN_NUM_SDA, DS3232_PIN_NUM_SCL);
    if ((test.addr != DS3231_ADDR) && (test.port != DS3232_I2C_PORT) && (test.cfg.scl_io_num != DS3232_PIN_NUM_SCL) && (test.cfg.sda_io_num != DS3232_PIN_NUM_SDA))
    {
        is_Desc_true = false;
    }
    ds3231_free_desc(&test);
    TEST_ASSERT_EQUAL_MESSAGE(true, is_Desc_true, "ds3231_init_desc returns a wrong device descriptor");
}

TEST(ds3231_test_group, ds3231_init_returned_value_test)
{
    i2c_dev_t test;
    bool is_return_OK = true;
    test.mutex = NULL;
    ds3231_init_desc(&test, DS3232_I2C_PORT, DS3232_PIN_NUM_SDA, DS3232_PIN_NUM_SCL);
    if (test.mutex == NULL)
    {
        is_return_OK = false;
    }
    ds3231_free_desc(&test);
    TEST_ASSERT_EQUAL_MESSAGE(true, is_return_OK, "ds3231_init_desc doesn't return Mutex");
}

TEST(ds3231_test_group, ds3231_free_desc_test)
{
    i2c_dev_t test;
    test.mutex = NULL;
    bool is_Returned_OK = true;
    test.mutex = xSemaphoreCreateMutex();
    if (ds3231_free_desc(&test) != ESP_OK)
    {
        is_Returned_OK = false;
    }
    TEST_ASSERT_EQUAL_MESSAGE(true, is_Returned_OK, "pcf8574_free_desc doesn't free Mutex");
}

// BME280 Component test
TEST_GROUP(bme_bmp280_test_group);
TEST_SETUP(bme_bmp280_test_group)
{
}

TEST_TEAR_DOWN(bme_bmp280_test_group)
{
}

TEST(bme_bmp280_test_group, bme280_init_test)
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

TEST(bme_bmp280_test_group, bme280_read_data_test)
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

TEST_GROUP_RUNNER(bme_bmp280_test_group)
{
    RUN_TEST_CASE(bme_bmp280_test_group, bme280_init_test);
    RUN_TEST_CASE(bme_bmp280_test_group, bme280_read_data_test);
}

// PMS7003 Component test
TEST_GROUP(pms7003_test_group);
TEST_SETUP(pms7003_test_group)
{
}

TEST_TEAR_DOWN(pms7003_test_group)
{
}

TEST(pms7003_test_group, pms7003_initUart_test)
{
    bool is_init_ok = true;
    uart_config_t pms_uart_config = UART_CONFIG_DEFAULT();
    esp_err_t error_1 = uart_driver_install(PMS7003_UART_PORT, (RX_BUFFER_SIZE * 2), 0, 0, NULL, 0);
    ESP_ERROR_CHECK_WITHOUT_ABORT(error_1);
    esp_err_t error_2 = uart_param_config(PMS7003_UART_PORT, &pms_uart_config);
    ESP_ERROR_CHECK_WITHOUT_ABORT(error_2);
    esp_err_t error_3 = uart_set_pin(PMS7003_UART_PORT, PMS7003_UART_TX_PIN, PMS7003_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_ERROR_CHECK_WITHOUT_ABORT(error_3);

    uart_driver_delete(PMS7003_UART_PORT);
    if (error_1 == ESP_OK &&
        error_2 == ESP_OK &&
        error_3 == ESP_OK)
    {
        TEST_ASSERT(is_init_ok == true);
    }
    else
    {
        is_init_ok = false;
        if ((error_1 != ESP_OK) && (error_2 != ESP_OK) && (error_3 != ESP_OK))
        {
            TEST_ASSERT_EQUAL_MESSAGE(true, is_init_ok, "driver install, param_config and set pin for pms7003's UART unsuccessfully");
        }
        else if ((error_1 != ESP_OK) && (error_2 != ESP_OK))
        {
            TEST_ASSERT_EQUAL_MESSAGE(true, is_init_ok, "driver install, param_config for pms7003's UART unsuccessfully");
        }
        else if ((error_2 != ESP_OK) && (error_3 != ESP_OK))
        {
            TEST_ASSERT_EQUAL_MESSAGE(true, is_init_ok, "param_config and set pin for pms7003's UART unsuccessfully");
        }
        else if ((error_1 != ESP_OK) && (error_3 != ESP_OK))
        {
            TEST_ASSERT_EQUAL_MESSAGE(true, is_init_ok, "driver install and set pin for pms7003's UART unsuccessfully");
        }
        else
        {
            if ((error_1 != ESP_OK))
            {
                TEST_ASSERT_EQUAL_MESSAGE(true, is_init_ok, "driver install for pms7003's UART unsuccessfully");
            }
            else
            {
                if ((error_2 != ESP_OK))
                {
                    TEST_ASSERT_EQUAL_MESSAGE(true, is_init_ok, "param_config for pms7003's UART unsuccessfully");
                }
                else
                {
                    TEST_ASSERT_EQUAL_MESSAGE(true, is_init_ok, "set pin for pms7003's UART unsuccessfully");
                }
            }
        }
    }
}

TEST(pms7003_test_group, pms7003_set_active_mode_test)
{
    esp_err_t is_set_ActiveMode_OK;
    uart_config_t pms_uart_config = UART_CONFIG_DEFAULT();
    is_set_ActiveMode_OK = pms7003_initUart(&pms_uart_config, PMS7003_UART_PORT, PMS7003_UART_TX_PIN, PMS7003_UART_RX_PIN);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_set_ActiveMode_OK, "initialization for pms7003's UART is unsuccessfull, check that function before executing this test case");
    if (is_set_ActiveMode_OK == ESP_OK)
    {
        is_set_ActiveMode_OK = pms7003_activeMode();
        uart_driver_delete(PMS7003_UART_PORT);
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_set_ActiveMode_OK, "sending active mode command to pms7003's UART unsuccessfully");
    }
}

TEST(pms7003_test_group, pms7003_readData_test)
{
    esp_err_t is_set_ActiveMode_OK;
    esp_err_t is_set_ActiveMode_OK_2;
    struct dataSensor_st test_pms_value;
    uart_config_t pms_uart_config = UART_CONFIG_DEFAULT();
    is_set_ActiveMode_OK = pms7003_initUart(&pms_uart_config, PMS7003_UART_PORT, PMS7003_UART_TX_PIN, PMS7003_UART_RX_PIN);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_set_ActiveMode_OK, "initialization for pms7003's UART is unsuccessfull, check that function before executing this test case");
    if (is_set_ActiveMode_OK == ESP_OK)
    {
        int count = 0;
        while (count <= 3)
        {
            is_set_ActiveMode_OK = pms7003_readData(indoor, &(test_pms_value.pm1_0), &(test_pms_value.pm2_5), &(test_pms_value.pm10));
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            is_set_ActiveMode_OK_2 = pms7003_readData(outdoor, &(test_pms_value.pm1_0), &(test_pms_value.pm2_5), &(test_pms_value.pm10));
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            count++;
        }
        uart_driver_delete(PMS7003_UART_PORT);
        if ((is_set_ActiveMode_OK != ESP_OK) && (is_set_ActiveMode_OK_2 != ESP_OK))
        {
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_set_ActiveMode_OK, "reading PMS7003 uart port in indoor mode and out door mode unsuccessfully");
        }
        else
        {
            if (is_set_ActiveMode_OK != ESP_OK)
            {
                TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_set_ActiveMode_OK, "reading PMS7003 uart port in indoor mode unsuccessfully");
            }
            else
            {
                TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_set_ActiveMode_OK_2, "reading PMS7003 uart port in outdoor mode unsuccessfully");
            }
        }
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, is_set_ActiveMode_OK, "reading PMS7003 uart port successfully");
    }
}

TEST_GROUP_RUNNER(pms7003_test_group)
{
    RUN_TEST_CASE(pms7003_test_group, pms7003_initUart_test);
    RUN_TEST_CASE(pms7003_test_group, pms7003_set_active_mode_test);
    RUN_TEST_CASE(pms7003_test_group, pms7003_readData_test);
}

// RUN ALL TEST GROUP
static void run_all_tests(void)
{
    RUN_TEST_GROUP(testable);
    RUN_TEST_GROUP(pcf8574_test_group);
    RUN_TEST_GROUP(bme_bmp280_test_group);
    RUN_TEST_GROUP(pms7003_test_group);
}

// Test application main
void app_main(void)
{
    UNITY_MAIN_FUNC(run_all_tests);
}

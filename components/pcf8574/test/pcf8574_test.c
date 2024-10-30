#define UNITY_SUPPORT_TEST_CASES
#include <limits.h>
#include "unity.h"
#include <unity_fixture.h>
#include "pcf8574.h"
i2c_dev_t pcf8574_test; // for testing component pcf8574
// Test group for component pcf8574

TEST_CASE("pcf8574_test_group", "[pcf8574_init_returned_desc_test]")
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

TEST_CASE("pcf8574_test_group", "[pcf8574_init_returned_value_test]")
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

TEST_CASE("pcf8574_test_group", "[pcf8574_free_desc_test]")
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

TEST_CASE("pcf8574_test_group", "[pcf8574_portwrite]")
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

TEST_CASE("pcf8574_test_group", "[pcf8574_portread]")
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

#define UNITY_SUPPORT_TEST_CASES
#include <limits.h>
#include "unity.h"
#include <unity_fixture.h>
#include "pms7003.h"

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

TEST_CASE("pms7003_test_group", "[pms7003_initUart_test]")
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

TEST_CASE("pms7003_test_group", "[pms7003_set_active_mode_test]")
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

TEST_CASE("pms7003_test_group", "[pms7003_readData_test]")
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

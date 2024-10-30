#define UNITY_SUPPORT_TEST_CASES
#include <limits.h>
#include "unity.h"
#include <unity_fixture.h>
#include "sdcard.h"

TEST_CASE("sdcard_test_group", "[sdcard_initiallize_test")
{

    esp_vfs_fat_mount_config_t mount_config_t = MOUNT_CONFIG_DEFAULT();
    spi_bus_config_t spi_bus_config_t = SPI_BUS_CONFIG_DEFAULT();
    sdmmc_host_t host_t = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_PIN_NUM_CS;
    slot_config.host_id = host_t.slot;

    sdmmc_card_t *SDCARD;
    esp_err_t err_code;

    err_code = spi_bus_initialize(host_t.slot, &spi_bus_config_t, SPI_DMA_CHAN);
    if (err_code != ESP_OK)
    {
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to initialize SPI bus for SD card");
    }
    else
    {
        slot_config.gpio_cs = CONFIG_PIN_NUM_CS;
        slot_config.host_id = host_t.slot;
        err_code = esp_vfs_fat_sdspi_mount(mount_point, &host_t, &slot_config, &mount_config_t, &SDCARD);
        esp_vfs_fat_sdcard_unmount(mount_point, SDCARD);
        spi_bus_free(host_t.slot);
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to mount filesystem for SD card");
    }
}

TEST_CASE("sdcard_test_group", "[sdcard_write_and_read_file_test")
{

    char *filename1 = "filedat";
    char *filename2 = "filenam";
    char *filename3 = "filevu";

    esp_vfs_fat_mount_config_t mount_config_t = MOUNT_CONFIG_DEFAULT();
    spi_bus_config_t spi_bus_config_t = SPI_BUS_CONFIG_DEFAULT();
    sdmmc_host_t host_t = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_PIN_NUM_CS;
    slot_config.host_id = host_t.slot;

    sdmmc_card_t *SDCARD;
    esp_err_t err_code = spi_bus_initialize(host_t.slot, &spi_bus_config_t, SPI_DMA_CHAN);
    err_code = esp_vfs_fat_sdspi_mount(mount_point, &host_t, &slot_config, &mount_config_t, &SDCARD);
    esp_err_t err_code_1;
    esp_err_t err_code_2;
    if (err_code != ESP_OK)
    {
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to initialize SD card");
    }
    else
    {
        err_code = sdcard_writeDataToFile(filename1, "test case 1");
        if ((err_code == ESP_ERROR_SD_WRITE_DATA_FAILED) || (err_code == ESP_FAIL))
        {
            sdcard_removeFile(filename1);
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to write string to SD card");
            goto end;
        }
        else
        {
            if (err_code == ESP_ERROR_SD_OPEN_FILE_FAILED)
            {
                TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to open SD card file");
                goto end;
            }
        }
        err_code = sdcard_writeDataToFile(filename2, "test case akldasjkldja");
        if ((err_code == ESP_ERROR_SD_WRITE_DATA_FAILED) || (err_code == ESP_FAIL))
        {
            sdcard_removeFile(filename2);
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to write string to SD card");
            goto end;
        }
        else
        {
            if (err_code == ESP_ERROR_SD_OPEN_FILE_FAILED)
            {
                TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to open SD card file");
                goto end;
            }
        }
        err_code = sdcard_writeDataToFile(filename3, "test case 123%xyasdasdasd");
        if ((err_code == ESP_ERROR_SD_WRITE_DATA_FAILED) || (err_code == ESP_FAIL))
        {
            sdcard_removeFile(filename3);
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to write string to SD card");
            goto end;
        }
        else
        {
            if (err_code == ESP_ERROR_SD_OPEN_FILE_FAILED)
            {
                TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to open SD card file");
                goto end;
            }
        }
        sdcard_removeFile(filename1);
        sdcard_removeFile(filename2);
        sdcard_removeFile(filename3);
    }
end:
    esp_vfs_fat_sdcard_unmount(mount_point, SDCARD);
    spi_bus_free(host_t.slot);
    TEST_ASSERT_EQUAL(ESP_OK, ESP_OK);
}

TEST_CASE("sdcard_test_group", "[sdcard_read_file_test]")
{
    char *filename1 = "filedat";

    esp_vfs_fat_mount_config_t mount_config_t = MOUNT_CONFIG_DEFAULT();
    spi_bus_config_t spi_bus_config_t = SPI_BUS_CONFIG_DEFAULT();
    sdmmc_host_t host_t = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_PIN_NUM_CS;
    slot_config.host_id = host_t.slot;

    sdmmc_card_t *SDCARD;
    esp_err_t err_code = spi_bus_initialize(host_t.slot, &spi_bus_config_t, SPI_DMA_CHAN);
    err_code = esp_vfs_fat_sdspi_mount(mount_point, &host_t, &slot_config, &mount_config_t, &SDCARD);
    esp_err_t err_code_1;
    esp_err_t err_code_2;
    if (err_code != ESP_OK)
    {
        TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to initialize SD card");
    }
    else
    {
        err_code = sdcard_writeDataToFile(filename1, "testcase1\ntestcase2\nvucasexyzabc\n");
        if ((err_code == ESP_ERROR_SD_WRITE_DATA_FAILED) || (err_code == ESP_FAIL))
        {
            sdcard_removeFile(filename1);
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to write string to SD card");
            goto end;
        }
        else
        {
            if (err_code == ESP_ERROR_SD_OPEN_FILE_FAILED)
            {
                TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_code, "Failed to open SD card file");
                goto end;
            }
        }
        char pathFile[64];
        sprintf(pathFile, "%s/%s.txt", mount_point, filename1);
        //     sdcard_readDataFromFile(const char *nameFile, const char *format, ...)
        FILE *file = fopen(pathFile, "r");
        if (file == NULL)
        {
            TEST_ASSERT_EQUAL_MESSAGE(1, 0, "Failed to open SD card file");
            goto end;
        }
        char *read_string = (char *)malloc((size_t)256);
        memset(read_string, 0, strlen(read_string));
        char tc[3][256];
        int i = 0;
        while (!feof(file) && (i < 3))
        {
            fscanf(file, "%s", read_string);
            strcpy(tc[i], read_string);
            i++;
        }
        sdcard_removeFile(filename1);
        free(read_string);
        bool is_read_true = true;

        if ((strcmp(tc[0], "testcase1") != 0) || (strcmp(tc[1], "testcase2") != 0) || (strcmp(tc[2], "vucasexyzabc") != 0))
        {
            is_read_true = false;
        }
        TEST_ASSERT_EQUAL_MESSAGE(true, is_read_true, "Reading wrong string from file");
    end:

        esp_vfs_fat_sdcard_unmount(mount_point, SDCARD);
        spi_bus_free(host_t.slot);
    }
}

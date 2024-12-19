// Data logging task for the ESP32
#include "dataLogging.h"


void data_log_task(void* parameters)
{
    static const char* const TAG = "DATA_LOGGING"; // tag for the data logging task
    ESP_LOGI(TAG, "Start data logging task"); // log the start of the data logging task

    // local variables for the data logging task
    esp_err_t ret; // var for error checking

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    // Initialize SPI bus
    spi_bus_config_t bus_cfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;


    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    if(ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set the format_if_mount_failed flag to true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "SD card mounted");

    sdmmc_card_print_info(stdout, card);

    FILE* f = fopen("/sdcard/data.txt", "w");
    if(f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
    }
    else
    {
        fprintf(f, "sensor_label, timestamp, speed\n");
        fclose(f);
    }   

    while(true)
    {
        gpio_set_level(LED_GPIO, record_state); // set the LED state to the record state
        if(data_log_queue_handle != NULL)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        else if(uxQueueMessagesWaiting(data_log_queue_handle) > 0)
        {
            data_t data;
            if(xQueueReceive(data_log_queue_handle, &data, 0) == pdTRUE)
            {
                FILE* f = fopen("/sdcard/data.txt", "a");
                if(f == NULL)
                {
                    ESP_LOGE(TAG, "Failed to open file for writing");
                }
                else
                {
                    fprintf(f, "%d, %f, %f\n", data.sensor_label, data.timestamp, data.speed);
                    fclose(f);
                }
            }
        }
        else
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

    }
}
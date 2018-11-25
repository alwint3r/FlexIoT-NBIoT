#include <Arduino.h>
#include "esp_sara_nbiot.h"
#include "esp_sara_config.h"

volatile bool mqtt_connected = false;

static esp_err_t sara_event_handle(esp_sara_event_handle_t *event)
{
    const char *TAG = "EVENT";
    esp_sara_client_handle_t * client = event->client;
    switch (event->event_id)
    {
    case SARA_EVENT_SIGNAL_FOUND:
    {
        Serial.printf("Signal found\n");
        if (event->payload_size > 0)
            Serial.printf("Signal strengh %d\n", event->payload[0]);
    }
    break;
    case SARA_EVENT_SIGNAL_NOT_FOUND:
    {
        Serial.printf("Signal not found\n");
    }
    break;
    case SARA_EVENT_ATTACHED:
    {
        Serial.printf("Atttached\n");
        if (event->payload_size > 0)
            Serial.printf("Attached %d\n", event->payload[0]);
    }
    break;
    case SARA_EVENT_DETTACHED:
    {
        Serial.printf("Dettached\n");
        if (event->payload_size > 0)
            Serial.printf("Attached %d\n", event->payload[0]);
    }
    break;
    case SARA_EVENT_MQTT_CONNECTED:
    {
        Serial.printf("MQTT_CONNECTED\n");
        mqtt_connected = true;
        esp_sara_subscribe_mqtt(client, "/test/rx", 1);
    }
    case SARA_EVENT_MQTT_DATA:
    {
        Serial.printf("MQTT_DATA\n");
        Serial.printf("topic: %s mesg: %s\n", event->topic, event->payload);
    }
    break;
    case SARA_EVENT_PUBLISHED:
    {
        Serial.printf("MQTT_PUBLISHED\n");
    }
    break;
    case SARA_EVENT_PUBLISH_FAILED:
    {
        Serial.printf("MQTT_PUBLISHED_FAILED\n");
    }
    break;
    case SARA_EVENT_MQTT_ERR:
    {
        Serial.printf("MQTT ERROR %d %d\n", *(event->payload), *((event->payload + 4)));
    }
    case SARA_EVENT_CME_ERROR:
    {
        Serial.printf("CME ERROR %s\n", (char*)event->payload);
    }
    default:
        break;
    }
    return ESP_OK;
}

esp_sara_client_handle_t *sara;
static int i = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    pinMode(SARA_UART_DTR_PIN, OUTPUT);
    digitalWrite(SARA_UART_DTR_PIN, LOW);
    pinMode(SARA_UART_RTS_PIN, OUTPUT);
    digitalWrite(SARA_UART_RTS_PIN, LOW);

    esp_sara_client_config_t config = {};
    config.apn = "iotxl";
    config.rat = 8;
    config.use_hex = false;
    config.transport = SARA_TRANSPORT_MQTT;
    config.event_handle = sara_event_handle;

    esp_sara_mqtt_client_config_t mqtt_config = {};
    mqtt_config.client_id = "dytrax-nbshield-ketang";
    mqtt_config.host = "52.221.141.22";//"52.230.4.141";
    mqtt_config.port = 1883;
    mqtt_config.timeout = 120;
    mqtt_config.clean_session = false;
    mqtt_config.username = "generic_brand_617-generic_device-v1nm_1740";
    mqtt_config.password = "1542998040_1740";

    config.transport_config = (esp_sara_transport_config_t*)&mqtt_config;

    sara = esp_sara_client_init(&config);
    esp_sara_start(sara);
}

void loop() {
    Serial.printf("esp_free_heap %u\n", esp_get_free_heap_size());

    if(mqtt_connected)
    {
        int csq = 99;
        esp_sara_get_csq(sara, &csq);
        char msg[] = "{\"eventName\":\"tst\",\"status\":\"1\",\"d\":1,\"m\":\"4205236004488867\"}";
        
        esp_sara_publish_mqtt(sara, "/test/tx", (char*)msg, false, 1, 0);
        Serial.printf("%s\n", msg);
    }

    delay(10000 / portTICK_PERIOD_MS);
}

#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_http_client.h>

#define WIFI_SSID "@IoE"       // Substitua pelo seu SSID
#define WIFI_PASS "123456789"    // Substitua pela sua senha Wi-Fi
#define BASE_URL "https://supervisor-system.onrender.com/esp32" // URL base

static const char *TAG = "HTTPS_WIFI";

// Função de tratamento de eventos HTTP
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_DISCONNECTED: // Correção aqui
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
        default:
            ESP_LOGI(TAG, "Unknown event: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}

// Função para realizar a requisição GET com um valor dinâmico
void https_get_request(const char *value) {
    char url[256];

    // Construa a URL completa
    snprintf(url, sizeof(url), "%s/%s", BASE_URL, value);

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        // .cert_pem = your_cert_pem_start, // Adicione seu certificado se necessário
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Realiza a requisição
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "GET Status = %d, content_length = %" PRIi64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    // Limpa o cliente
    esp_http_client_cleanup(client);
}

// Função de conexão ao Wi-Fi
void wifi_init_sta() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Inicializa o Wi-Fi
    esp_netif_init();
    esp_event_loop_create_default();

    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Configura os parâmetros do Wi-Fi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Conecta ao Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_connect());
    ESP_LOGI(TAG, "Connecting to Wi-Fi...");
}

// Função principal
void app_main(void) {
    wifi_init_sta();

    // Aguarda a conexão
    //vTaskDelay(5000 / portTICK_PERIOD_MS); // Aguarda 5 segundos para conexão

    // Valor a ser enviado na URL
    const char *value = "70"; // Substitua pelo valor desejado
    https_get_request(value);
    
}

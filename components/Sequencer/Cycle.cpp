#include "Cycle.hpp"

Cycle::Cycle()
{
}

esp_err_t Cycle::_pushPass(Pass const &pass)
{
    esp_err_t ret = ESP_OK;
    if (_lastId >= MAX_PASSES)
    {
        ret = ESP_ERR_INVALID_SIZE;
        ESP_LOGE(TAG, "Max nr. of passes in group array was exceeded,"
                      " current _lastId is: %d and max available items is: %d",
                 _lastId, MAX_PASSES);
    }
    else
    {
        _passes[_lastId] = pass;
        ++_lastId;
        ESP_LOGI(TAG, "Pushing a pass, _lastId now is: %d", _lastId);
    }
    return ret;
}

void Cycle::_reset()
{
    _lastId = 0U;
}

void Cycle::_logContent()
{
    ESP_LOGI(TAG, "\n****************CYCLE LOGGING START***************");
    for (int i = 0; i < _lastId; ++i)
    {
        ESP_LOGI(TAG, "Logging pass %d\n", i);
        _passes[i]._logContent();
        printf("\n");
    }
    ESP_LOGI(TAG, "****************CYCLE LOGGING END***************");
}

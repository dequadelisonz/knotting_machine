idf_component_register(SRCS "Downloader.cpp" "Importer.cpp" "ImporterWIFI.cpp" "ImporterSD.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash esp-tls esp_http_client esp_wifi SDDriver Sequencer)

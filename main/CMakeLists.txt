idf_component_register(SRCS "HMI.cpp" "main.cpp" "KnotEngine.cpp"
                    INCLUDE_DIRS "." "include"
                    REQUIRES pthread NORVIEXF16 I2CSocket RemoteInputs Menu SequenceEngine SSD1306 Sequencer)


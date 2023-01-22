idf_component_register(SRCS "HMI.cpp" "main.cpp" 
                    INCLUDE_DIRS "." "include"
                    REQUIRES I2CSocket RemoteInputs Menu SequenceEngine SSD1306 Sequencer)


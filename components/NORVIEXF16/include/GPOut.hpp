#ifndef GPOut_hpp
#define GPOut_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "NORVIEXF16.hpp"

class GPOutArray
{
public:
    const char *TAG = "GPOutArray";

    static constexpr uint8_t MAX_GPOut = 16U;
    static constexpr uint8_t MAX_EXPANSIONS = 6U;

private:
    uint8_t _expansionQty = 0U;

    class GPOutPin
    {
        friend class GPOutArray;

    public:
        const char *TAG = "GPOutPin";

        /*
            **address 0x27**
            NORVI-EX-F16 MCP23017
            GPIOA
                0x1 =Q4
                0x2=Q1
                0x4=Q2
                0x8=Q3
                0x10=Q8
                0x20=Q5
                0x40=Q6
                0x80=Q7

            GPIOB
                0x1=Q12
                0x2=Q9
                0x4=Q10
                0x8=Q11
                0x10=Q16
                0x20=Q13
                0x40=Q14
                0x80=Q15

            **address 0x23**

            GPIOA
                0x1 =Q1
                0x2=Q2
                0x4=Q3
                0x8=Q4
                0x10=Q5
                0x20=Q6
                0x40=Q7
                0x80=Q8

            GPIOB
                0x1=Q9
                0x2=Q10
                0x4=Q11
                0x8=Q12
                0x10=Q13
                0x20=Q14
                0x40=Q15
                0x80=Q16



        */

        static constexpr uint8_t swOnMap[MAX_EXPANSIONS][MAX_GPOut] = {
            {
                0x2,  // Q1
                0x4,  // Q2
                0x8,  // Q3
                0x1,  // Q4
                0x20, // Q5
                0x40, // Q6
                0x80, // Q7
                0x10, // Q8
                0x2,  // Q9
                0x4,  // Q10
                0x8,  // Q11
                0x1,  // Q12
                0x20, // Q13
                0x40, // Q14
                0x80, // Q15
                0x10  // Q16
            },
            {
                0x1,  // Q1                                                                                                                                               0x2,  // Q1
                0x2,  // Q2
                0x4,  // Q3
                0x8,  // Q4
                0x10, // Q5
                0x20, // Q6
                0x40, // Q7
                0x80, // Q8
                0x1,  // Q9
                0x2,  // Q10
                0x4,  // Q11
                0x8,  // Q12
                0x10, // Q13
                0x20, // Q14
                0x40, // Q15
                0x80  // Q16

            },
            {0},{0},{0},{0}};

        static constexpr uint8_t swOffMap[MAX_EXPANSIONS][MAX_GPOut] = {
            {
                0xFD, // Q1
                0xFB, // Q2
                0xF7, // Q3
                0xFE, // Q4
                0xDF, // Q5
                0xBF, // Q6
                0x7F, // Q7
                0xEF, // Q8
                0xFD, // Q9
                0xFB, // Q10
                0xF7, // Q11
                0xFE, // Q12
                0xDF, // Q13
                0xBF, // Q14
                0x7F, // Q15
                0xEF  // Q16
            },
            {
                0xFE, // Q1
                0xFD, // Q2
                0xFB, // Q3
                0xF7, // Q4
                0xEF, // Q5
                0xDF, // Q6
                0xBF, // Q7
                0x7F, // Q8
                0xFE, // Q9
                0xFD, // Q10
                0xFB, // Q11
                0xF7, // Q12
                0xEF, // Q13
                0xDF, // Q14
                0xBF, // Q15
                0x7F  // Q16

            },
            {0},{0},{0},{0}};

        uint8_t _expansionId = 0;
        uint8_t _pinInExpansion = 0;
        NORVIEXF16::gpioGroupEnum _group = NORVIEXF16::gpioGroupEnum::GPIOA;

    public:
        GPOutPin(){};
        GPOutPin(uint8_t pin);
    };

    NORVIEXF16 _expansions[MAX_EXPANSIONS];

    GPOutPin _pins[MAX_EXPANSIONS * MAX_GPOut];

    esp_err_t _setGPOStatus(uint8_t const gpo, uint8_t v, bool const on);

public:
    GPOutArray(const uint8_t addresses[MAX_EXPANSIONS]);

    esp_err_t set(uint8_t gpo, uint8_t status);
};

#endif
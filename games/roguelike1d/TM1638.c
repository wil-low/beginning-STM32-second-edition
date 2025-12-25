/**
 **********************************************************************************
 * @file   TM1638.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  TM1638 chip driver
 *         Functionalities of the this file:
 *          + Display config and control functions
 *          + Keypad scan functions
 **********************************************************************************
 *
 * Copyright (c) 2021 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */

/* Includes
 * ---------------------------------------------------------------------*/
#include "TM1638.h"

/* Private Constants
 * ------------------------------------------------------------*/
/**
 * @brief  Instruction description
 */
#define DataInstructionSet 0x40           // 0b01000000
#define DisplayControlInstructionSet 0x80 // 0b10000000
#define AddressInstructionSet 0xC0        // 0b11000000

/**
 * @brief  Data instruction set
 */
#define WriteDataToRegister 0x00 // 0b00000000
#define ReadKeyScanData 0x02     // 0b00000010
#define AutoAddressAdd 0x00      // 0b00000000
#define FixedAddress 0x04        // 0b00000100
#define NormalMode 0x00          // 0b00000000
#define TestMode 0x08            // 0b00001000

/**
 * @brief  Display ControlInstruction Set
 */
#define ShowTurnOff 0x00 // 0b00000000
#define ShowTurnOn 0x08  // 0b00001000

/**
 ==================================================================================
                           ##### Private Functions #####
 ==================================================================================
 */

uint8_t siekoo(uint8_t c) {
    // https://fakoo.de/en/siekoo.html
    switch (c) {
    case '0':
        return 0b00111111;
    case '1':
        return 0b00000110;
    case '2':
        return 0b01011011;
    case '3':
        return 0b01001111;
    case '4':
        return 0b01100110;
    case '5':
        return 0b01101101;
    case '6':
        return 0b01111101;
    case '7':
        return 0b00000111;
    case '8':
        return 0b01111111;
    case '9':
        return 0b01101111;
    case 'a':
        return 0b01011111;
    case 'b':
        return 0b01111100;
    case 'c':
        return 0b01011000;
    case 'd':
        return 0b01011110;
    case 'e':
        return 0b01111001;
    case 'f':
        return 0b01110001;
    case 'g':
        return 0b00111101;
    case 'h':
        return 0b01110100;
    case 'i':
        return 0b00010001;
    case 'j':
        return 0b00001101;
    case 'k':
        return 0b01110101;
    case 'l':
        return 0b00111000;
    case 'm':
        return 0b01010101;
    case 'n':
        return 0b01010100;
    case 'o':
        return 0b01011100;
    case 'p':
        return 0b01110011;
    case 'q':
        return 0b01100111;
    case 'r':
        return 0b01010000;
    case 's':
        return 0b00101101;
    case 't':
        return 0b01111000;
    case 'u':
        return 0b00011100;
    case 'v':
        return 0b00101010;
    case 'w':
        return 0b01101010;
    case 'x':
        return 0b00010100;
    case 'y':
        return 0b01101110;
    case 'z':
        return 0b00011011;
    case ' ':
        return 0b00000000;
    case '.':
        return 0b00010000;
    case ',':
        return 0b00001100;
    case ';':
        return 0b00001010;
    case ':':
        return 0b00001001;
    case '=':
        return 0b01001000;
    case '+':
        return 0b01000110;
    case '/':
        return 0b01010010;
    case 0x5c:
        return 0b01100100; // backslash
    case '!':
        return 0b01101011;
    case '?':
        return 0b01001011;
    case '_':
        return 0b00001000;
    case '-':
        return 0b01000000;
    case '^':
        return 0b00000001;
    case '"':
        return 0b00100000;
    case '\'':
        return 0b00100010;
    case '%':
        return 0b00100100;
    case '(':
        return 0b00111001;
    case ')':
        return 0b00001111;
    case '@':
        return 0b00010111;
    case '*':
        return 0b01001001;
    case '#':
        return 0b00110110;
    case '<':
        return 0b00100001;
    case '>':
        return 0b00000011;
    }
    return 0b10000000;
}

static inline void TM1638_StartComunication(TM1638_Handler_t *Handler) {
    Handler->StbWrite(0);
}

static inline void TM1638_StopComunication(TM1638_Handler_t *Handler) {
    Handler->StbWrite(1);
}

static void TM1638_WriteBytes(TM1638_Handler_t *Handler, const uint8_t *Data,
                              uint8_t NumOfBytes) {
    uint8_t i, j, Buff;

    Handler->DioConfigOut();

    for (j = 0; j < NumOfBytes; j++) {
        for (i = 0, Buff = Data[j]; i < 8; ++i, Buff >>= 1) {
            Handler->ClkWrite(0);
            Handler->DelayUs(1);
            Handler->DioWrite(Buff & 0x01);
            Handler->ClkWrite(1);
            Handler->DelayUs(1);
        }
    }
}

static void TM1638_ReadBytes(TM1638_Handler_t *Handler, uint8_t *Data,
                             uint8_t NumOfBytes) {
    uint8_t i, j, Buff;

    Handler->DioConfigIn();

    Handler->DelayUs(5);

    for (j = 0; j < NumOfBytes; j++) {
        for (i = 0, Buff = 0; i < 8; i++) {
            Handler->ClkWrite(0);
            Handler->DelayUs(1);
            Handler->ClkWrite(1);
            Buff |= (Handler->DioRead() << i);
            Handler->DelayUs(1);
        }

        Data[j] = Buff;
        Handler->DelayUs(2);
    }
}

static void TM1638_SetMultipleDisplayRegister(TM1638_Handler_t *Handler,
                                              const uint8_t *DigitData,
                                              uint8_t StartAddr,
                                              uint8_t Count) {
    uint8_t Data = DataInstructionSet | WriteDataToRegister | NormalMode;

    StartAddr *= 2;

    TM1638_StartComunication(Handler);
    TM1638_WriteBytes(Handler, &Data, 1);
    TM1638_StopComunication(Handler);

    for (uint8_t i = 0; i < Count; i++) {
        Data = AddressInstructionSet | StartAddr;
        TM1638_StartComunication(Handler);
        TM1638_WriteBytes(Handler, &Data, 1);
        TM1638_WriteBytes(Handler, &DigitData[i], 1);
        TM1638_StopComunication(Handler);
        StartAddr += 2;
    }
}

static void TM1638_ScanKeyRegs(TM1638_Handler_t *Handler, uint8_t *KeyRegs) {
    uint8_t Data =
        DataInstructionSet | ReadKeyScanData | AutoAddressAdd | NormalMode;

    TM1638_StartComunication(Handler);
    TM1638_WriteBytes(Handler, &Data, 1);
    TM1638_ReadBytes(Handler, KeyRegs, 4);
    TM1638_StopComunication(Handler);
}

/**
 ==================================================================================
                           ##### Common Functions #####
 ==================================================================================
 */

/**
 * @brief  Initialize TM1638.
 * @param  Handler: Pointer to handler
 *
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful.
 */
TM1638_Result_t TM1638_Init(TM1638_Handler_t *Handler) {
    Handler->PlatformInit();
    return TM1638_OK;
}

/**
 * @brief  De-Initialize TM1638.
 * @param  Handler: Pointer to handler
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful.
 */
TM1638_Result_t TM1638_DeInit(TM1638_Handler_t *Handler) {
    Handler->PlatformDeInit();
    return TM1638_OK;
}

/**
 ==================================================================================
                        ##### Public Display Functions #####
 ==================================================================================
 */

/**
 * @brief  Config display parameters
 * @param  Handler: Pointer to handler
 * @param  Brightness: Set brightness level
 *         - 0: Display pulse width is set as 1/16
 *         - 1: Display pulse width is set as 2/16
 *         - 2: Display pulse width is set as 4/16
 *         - 3: Display pulse width is set as 10/16
 *         - 4: Display pulse width is set as 11/16
 *         - 5: Display pulse width is set as 12/16
 *         - 6: Display pulse width is set as 13/16
 *         - 7: Display pulse width is set as 14/16
 *
 * @param  DisplayState: Set display ON or OFF
 *         - TM1638DisplayStateOFF: Set display state OFF
 *         - TM1638DisplayStateON: Set display state ON
 *
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_ConfigDisplay(TM1638_Handler_t *Handler,
                                     uint8_t Brightness, uint8_t DisplayState) {
    uint8_t Data = DisplayControlInstructionSet;
    Data |= Brightness & 0x07;
    Data |= (DisplayState) ? (ShowTurnOn) : (ShowTurnOff);

    TM1638_StartComunication(Handler);
    TM1638_WriteBytes(Handler, &Data, 1);
    TM1638_StopComunication(Handler);

    TM1638_SetMultipleLEDs(Handler, 0);
    for (uint8_t i = 0; i < 8; i++)
        TM1638_SetSingleDigit(Handler, 0, i);

    return TM1638_OK;
}

/**
 * @brief  Set data to single digit in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Digit data
 * @param  DigitPos: Digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 *
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_SetSingleDigit(TM1638_Handler_t *Handler,
                                      uint8_t DigitData, uint8_t DigitPos) {
    TM1638_SetMultipleDisplayRegister(Handler, &DigitData, DigitPos, 1);
    return TM1638_OK;
}

/**
 * @brief  Set data to multiple digits in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 *
 * @param  Count: Number of segments to write data
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_SetMultipleDigit(TM1638_Handler_t *Handler,
                                        const uint8_t *DigitData,
                                        uint8_t StartAddr, uint8_t Count) {
    TM1638_SetMultipleDisplayRegister(Handler, DigitData, StartAddr, Count);
    return TM1638_OK;
}

/**
 * @brief  Set data to multiple digits in 7-segment format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Digit data (0, 1, ... , 15, a, A, b, B, ... , f, F)
 * @param  DigitPos: Digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 *
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_SetSingleDigit_HEX(TM1638_Handler_t *Handler,
                                          uint8_t DigitData, uint8_t DigitPos) {
    uint8_t DigitDataHEX = 0;
    uint8_t DecimalPoint = DigitData & 0x80;

    DigitData &= 0x7F;

    if (DigitData <= 15) {
        DigitDataHEX = siekoo(DigitData + '0') | DecimalPoint;
    } else {
        switch (DigitData) {
        case 'A':
        case 'a':
            DigitDataHEX = siekoo('a') | DecimalPoint;
            break;

        case 'B':
        case 'b':
            DigitDataHEX = siekoo('b') | DecimalPoint;
            break;

        case 'C':
        case 'c':
            DigitDataHEX = siekoo('c') | DecimalPoint;
            break;

        case 'D':
        case 'd':
            DigitDataHEX = siekoo('d') | DecimalPoint;
            break;

        case 'E':
        case 'e':
            DigitDataHEX = siekoo('e') | DecimalPoint;
            break;

        case 'F':
        case 'f':
            DigitDataHEX = siekoo('f') | DecimalPoint;
            break;

        default:
            DigitDataHEX = 0;
            break;
        }
    }

    return TM1638_SetSingleDigit(Handler, DigitDataHEX, DigitPos);
}

/**
 * @brief  Set data to multiple digits in hexadecimal format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data.
 *                    (0, 1, ... , 15, a, A, b, B, ... , f, F)
 *
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 *
 * @param  Count: Number of segments to write data
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_SetMultipleDigit_HEX(TM1638_Handler_t *Handler,
                                            const uint8_t *DigitData,
                                            uint8_t StartAddr, uint8_t Count) {
    uint8_t DigitDataHEX[10];
    uint8_t DecimalPoint = 0;

    for (uint8_t i = 0; i < Count; i++) {
        DecimalPoint = DigitData[i] & 0x80;

        uint8_t Digit = DigitData[i] & 0x7F;

        if (Digit <= 15) {
            DigitDataHEX[i] = siekoo(Digit + '0') | DecimalPoint;
        } else {
            switch (Digit) {
            case 'A':
            case 'a':
                DigitDataHEX[i] = siekoo('a') | DecimalPoint;
                break;

            case 'B':
            case 'b':
                DigitDataHEX[i] = siekoo('b') | DecimalPoint;
                break;

            case 'C':
            case 'c':
                DigitDataHEX[i] = siekoo('c') | DecimalPoint;
                break;

            case 'D':
            case 'd':
                DigitDataHEX[i] = siekoo('d') | DecimalPoint;
                break;

            case 'E':
            case 'e':
                DigitDataHEX[i] = siekoo('e') | DecimalPoint;
                break;

            case 'F':
            case 'f':
                DigitDataHEX[i] = siekoo('f') | DecimalPoint;
                break;

            default:
                DigitDataHEX[i] = 0;
                break;
            }
        }
    }

    return TM1638_SetMultipleDigit(Handler, (const uint8_t *)DigitDataHEX,
                                   StartAddr, Count);
}

/**
 * @brief  Set data to multiple digits in char format
 * @param  Handler: Pointer to handler
 * @param  DigitData: Array to Digits data.
 *                    Supported chars 0,1,2,3,4,5,6,7,8,9
 *                                    A,b,C,d,E,F,g,G,h,H,i,I,j,l,L,n,N,o,O,P,q,r,S,
 *                                    t,u,U,y,_,-,Overscore (use ~ to set)
 *
 * @param  StartAddr: First digit position
 *         - 0: Seg1
 *         - 1: Seg2
 *         - .
 *         - .
 *         - .
 *
 * @param  Count: Number of segments to write data
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_SetMultipleDigit_CHAR(TM1638_Handler_t *Handler,
                                             const uint8_t *DigitData,
                                             uint8_t StartAddr, uint8_t Count) {
    uint8_t DigitDataHEX[10];
    uint8_t DecimalPoint = 0;

    if (Count > 8)
        Count = 8;

    for (uint8_t i = 0; i < Count; i++) {
        DecimalPoint = DigitData[i] & 0x80;
        DigitDataHEX[i] = siekoo(DigitData[i] & 0x7F) | DecimalPoint;
    }

    return TM1638_SetMultipleDigit(Handler, (const uint8_t *)DigitDataHEX,
                                   StartAddr, Count);
}

/**
 ==================================================================================
                      ##### Public Keypad Functions #####
 ==================================================================================
 */

/**
 * @brief  Scan all 24 keys connected to TM1638
 * @note
 *                   SEG1         SEG2         SEG3       ......      SEG8
 *                     |            |            |                      |
 *         K1  --  |K1_SEG1|    |K1_SEG2|    |K1_SEG3|    ......    |K1_SEG8|
 *         K2  --  |K2_SEG1|    |K2_SEG2|    |K2_SEG3|    ......    |K2_SEG8|
 *         K3  --  |K3_SEG1|    |K3_SEG2|    |K3_SEG3|    ......    |K3_SEG8|
 *
 * @param  Handler: Pointer to handler
 * @param  Keys: pointer to save key scan result
 *         - bit0=>K1_SEG1, bit1=>K1_SEG2, ..., bit7=>K1_SEG8,
 *         - bit8=>K2_SEG1, bit9=>K2_SEG2, ..., bit15=>K2_SEG8,
 *         - bit16=>K3_SEG1, bit17=>K3_SEG2, ..., bit23=>K3_SEG8,
 *
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_ScanKeys(TM1638_Handler_t *Handler, uint32_t *Keys) {
    uint8_t KeyRegs[4];
    uint32_t KeysBuff = 0;
    uint8_t Kn = 0x01;

    TM1638_ScanKeyRegs(Handler, KeyRegs);

    for (uint8_t n = 0; n < 3; n++) {
        for (int8_t i = 3; i >= 0; i--) {
            KeysBuff <<= 1;

            if (KeyRegs[i] & (Kn << 4))
                KeysBuff |= 1;

            KeysBuff <<= 1;

            if (KeyRegs[i] & Kn)
                KeysBuff |= 1;
        }

        Kn <<= 1;
    }

    *Keys = KeysBuff;

    return TM1638_OK;
}

/**
 ==================================================================================
                           ##### LED Functions #####
 ==================================================================================
 */

/**
 * @brief  Set data to multiple LEDs
 * @param  Handler: Pointer to handler
 * @param  LEDData: bits 0-7
 * @retval TM1638_Result_t
 *         - TM1638_OK: Operation was successful
 */
TM1638_Result_t TM1638_SetMultipleLEDs(TM1638_Handler_t *Handler,
                                       uint8_t LEDData) {

    uint8_t Data = DataInstructionSet | WriteDataToRegister | NormalMode;

    TM1638_StartComunication(Handler);
    TM1638_WriteBytes(Handler, &Data, 1);
    TM1638_StopComunication(Handler);

    for (uint8_t i = 0; i < 8; i++) {
        Data = AddressInstructionSet | (i * 2 + 1);
        TM1638_StartComunication(Handler);
        TM1638_WriteBytes(Handler, &Data, 1);
        Data = (LEDData & 1);
        TM1638_WriteBytes(Handler, &Data, 1);
        TM1638_StopComunication(Handler);
        LEDData >>= 1;
    }
    return TM1638_OK;
}

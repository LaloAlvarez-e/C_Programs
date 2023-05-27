// First.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include <time.h>

#include "crc16.h"
#include "crc16_ccitt.h"

HANDLE Main__pvOpenSerialPort(const char* pcDevice, uint32_t u32BaudRate);
uint32_t Main__u32WritePort(HANDLE pvPort, uint8_t* pu8BufferIn, size_t szBufferSize);
size_t Main__szReadPort(HANDLE pvPort, uint8_t* pu8BufferOut, size_t szBufferSize);
size_t Main__szReadString(HANDLE pvPort, uint8_t* pu8BufferOut);


char pcPlainText[11] = " 123456789";
char pu8Number[30] = { 0 };
char pcBufferfromDevice[1000UL] = { 0 };
int main()
{
    uint16_t u16Crc = 0U;
    char* pcBufferpointer = 0;
#if 0
    u16Crc = CRC__u16Calculate((const uint8_t*)pcPlainText, 9U, 0U);
    printf("CRC16 BUYPASS calculated: 0x%X\n", (int)u16Crc);
    u16Crc = CRC__u16Calculate((const uint8_t*)pcPlainText, 9U, 0x800D);
    printf("CRC16 DDS-110 calculated: 0x%X\n", (int)u16Crc);

    u16Crc = CRC_CCITT__u16Calculate((const uint8_t*)pcPlainText, 9U, 0xFFFFU);
    printf("CRC16 CCITT-FALSE calculated: 0x%X\n", (int)u16Crc);
    u16Crc = CRC_CCITT__u16Calculate((const uint8_t*)pcPlainText, 9U, 0x1D0FU);
    printf("CRC16 AUG-CCITT calculated: 0x%X\n", (int)u16Crc);
    u16Crc = CRC_CCITT__u16Calculate((const uint8_t*)pcPlainText, 9U, 0U);
    printf("CRC16 XMODEM calculated: 0x%X\n", (int)u16Crc);
#endif
    char* pcDevice = 0;
    char pcPortNumber[20UL] = {'/','/','.','/','/','/','/'};
    uint32_t u32BaudRate = 9600UL;
    char* pcGetValue =  0 ;
    int s32ComComparison = 0;
    errno_t xError = 0;

    printf("Select Slave port COM number: ");
    pcGetValue = fgets(&pcPortNumber[7], 12, stdin);
    xError = _strupr_s(pcGetValue,12);
    s32ComComparison = strncmp(pcGetValue, "COM",sizeof("COM") - 1);
    if (0 == s32ComComparison)
    {
        unsigned long u32Values = 0;
        char* pcNextValue = 0;
        u32Values = strtoul(&pcGetValue[3], &pcNextValue, 10);
        *pcNextValue = 0;
        if (0 == u32Values)
        {
            printf("Invalid COM Port number or name");
            return(3);
        }
        else if (9 > u32Values)
        {
            pcDevice = pcGetValue;
        }
        else
        {
            pcDevice = pcPortNumber;
        }
    }
    else
    {
        printf("Invalid COM Port Name, ex. COM10, COM1, COM100");
        return (2);
    }

    HANDLE pvPort = Main__pvOpenSerialPort(pcDevice, u32BaudRate);
    if (INVALID_HANDLE_VALUE == pvPort)
    {   
        printf("COM Port is disconnected or is used by other process");
        return (1);
    }

    while (1)
    {
        pcBufferfromDevice[0] = 0;
        time_t xTime = 0ULL;
        size_t szReceived = Main__szReadPort(pvPort, (uint8_t*)pcBufferfromDevice, 1);
        printf("Value Received from slave: %d\n\r", (uint32_t) *pcBufferfromDevice);
        int32_t s32Comp = strcmp(pcBufferfromDevice, "\x34");
        if (0 == s32Comp)
        {
            xTime = time(NULL);
            sprintf_s(pu8Number, "%12d", xTime);
            printf("Enviando Informacion: %12ul\n\r\n\r", xTime);
            Main__u32WritePort(pvPort, (uint8_t*) "\x0C", 1);
            Main__u32WritePort(pvPort, (uint8_t*)pu8Number, 12);
        }
        


    }

}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln


HANDLE Main__pvOpenSerialPort(const char* pcDevice, uint32_t u32BaudRate)
{
    HANDLE pvPort = CreateFileA(pcDevice, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == pvPort)
    {
        return (INVALID_HANDLE_VALUE);
    }
    BOOL boSucess = FlushFileBuffers(pvPort);
    if (FALSE == boSucess)
    {
        CloseHandle(pvPort);
        return (INVALID_HANDLE_VALUE);
    }

    COMMTIMEOUTS stTimeouts = { 0 };

    stTimeouts.ReadIntervalTimeout = 0;
    stTimeouts.ReadTotalTimeoutConstant = 1000;
    stTimeouts.ReadTotalTimeoutMultiplier = 0;
    stTimeouts.WriteTotalTimeoutConstant = 100;
    stTimeouts.WriteTotalTimeoutMultiplier = 0;

    boSucess = SetCommTimeouts(pvPort, &stTimeouts);
    if (FALSE == boSucess)
    {
        CloseHandle(pvPort);
        return (INVALID_HANDLE_VALUE);
    }

    DCB stState = { 0 };

    stState.DCBlength = sizeof(DCB);
    stState.BaudRate = u32BaudRate;
    stState.ByteSize = 8;
    stState.Parity = NOPARITY;
    stState.StopBits = ONESTOPBIT;

    boSucess = SetCommState(pvPort, &stState);
    if (FALSE == boSucess)
    {
        CloseHandle(pvPort);
        return (INVALID_HANDLE_VALUE);
    }

    return (pvPort);
}


uint32_t Main__u32WritePort(HANDLE pvPort, uint8_t* pu8BufferIn, size_t szBufferSize)
{
    DWORD u32Written = 0;
    BOOL boSuccess = WriteFile(pvPort, pu8BufferIn, szBufferSize, &u32Written, NULL);
    if (FALSE == boSuccess)
    {
        return (-1);
    }
    if (u32Written != szBufferSize)
    {
        return (-1);
    }

    return 0;
}

size_t Main__szReadPort(HANDLE pvPort, uint8_t* pu8BufferOut, size_t szBufferSize)
{
    DWORD u32Received = 0;
    BOOL boSuccess = ReadFile(pvPort, pu8BufferOut, szBufferSize, &u32Received, NULL);
    if (FALSE == boSuccess)
    {
        return (-1);
    }
    return (u32Received);
}



size_t Main__szReadString(HANDLE pvPort, uint8_t* pu8BufferOut)
{
    DWORD u32Received = 0;
    do
    {
        BOOL boSuccess = ReadFile(pvPort, pu8BufferOut, 1, &u32Received, NULL);
        if (FALSE == boSuccess)
        {
            return (-1);
        }
        u32Received += u32Received;
    } while (*(pu8BufferOut++) != '\r');
    *pu8BufferOut = 0;
    return (u32Received);
}
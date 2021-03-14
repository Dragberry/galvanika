#ifndef INC_FLASH_HPP_
#define INC_FLASH_HPP_

#include <stdint.h>
#include "stm32f1xx_hal.h"

namespace STM32 {

template<typename Data>
class Flash {
private:
  const uint8_t data_width;
  const uint32_t start_address;
  const uint32_t end_address;
  const uint32_t pages;

public:
  Flash(const uint32_t address, const uint32_t pages) :
      data_width(sizeof(Data)),
      start_address(address),
      end_address(address + (((1024 * pages) / data_width) * data_width)),
      pages(pages) {
  }

  uint32_t erase() {
    FLASH_EraseInitTypeDef EraseInitStruct;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = start_address;
    EraseInitStruct.NbPages = pages;
    uint32_t page_error = 0;

    HAL_FLASH_Unlock();

    uint32_t error = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &page_error) != HAL_OK) {
      error = HAL_FLASH_GetError();
    }

    HAL_FLASH_Lock();

    return error;
  }

  uint32_t find_adress_to_write() {
    for (uint32_t address = start_address; address < end_address; address += data_width) {
      if (*(uint8_t*)(address + data_width - 1) == 0xFF) {
        return address;
      }
    }
    erase();
    return start_address;
  }

  uint32_t write(Data *data) {
    uint16_t* buffer = (uint16_t*) data;
    uint32_t address = find_adress_to_write();

    HAL_FLASH_Unlock();

    uint32_t error = 0;
    for (register uint32_t i = 0; i < data_width / 2; i++) {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, buffer[i]) != HAL_OK) {
        error = HAL_FLASH_GetError();
        break;
      }
      address += 2;
    }

    HAL_FLASH_Lock();
    return error;
  }

  uint32_t find_adress_to_read() {
    for (uint32_t address = start_address + data_width; address < end_address; address += data_width) {
      if (*(uint8_t*)(address + data_width - 1) == 0xFF) {
        return address - data_width;
      }
    }
    return end_address - data_width;
  }

  void read(Data* data) {
    uint16_t* buffer = (uint16_t*) data;
    uint32_t address = find_adress_to_read();

    for (register uint32_t i = 0; i < data_width / 2 ; i++) {
      buffer[i] = *(uint16_t*)address;
      address += 2;
    }
  }

};
}

#endif

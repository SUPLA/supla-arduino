/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "key_value.h"
#include <string.h>

namespace Supla {
  namespace Storage {

    KeyValue::~KeyValue() {
      auto element = first;
      while (element) {
        first = element->getNext();
        delete element;
        element = first;
      }
    }

    KeyValueElement *KeyValue::find(const char *key) {
      auto element = first;
      while (element) {
        if (element->isKeyEqual(key)) {
          return element;
        }
        element = element->getNext();
      }
      return nullptr;
    }

    KeyValueElement *KeyValue::findOrCreate(const char *key) {
      auto element = find(key);
      if (!element) {
        element = new KeyValueElement(key);
        element->addNext(first);
        first = element;
      }
      return element;
    }

    bool KeyValue::setString(const char* key, const char* value) {
      auto element = findOrCreate(key);
      return element->setString(value);
    }

    bool KeyValue::getString(const char* key, char* value, size_t maxSize) {
      auto element = find(key);
      if (!element) {
        return false;
      }
      return element->getString(value, maxSize);
    }

    int KeyValue::getStringSize(const char* key) {
      auto element = find(key);
      if (!element) {
        return 0;
      }
      return element->getStringSize();
    }

    bool KeyValue::setBlob(const char* key, const char* value, size_t blobSize) {
      auto element = findOrCreate(key);
      return element->setBlob(value, blobSize);
    }

    bool KeyValue::getBlob(const char* key, char* value, size_t blobSize) {
      auto element = find(key);
      if (!element) {
        return 0;
      }
      return element->getBlob(value, blobSize);
    }

    int KeyValue::getBlobSize(const char* key) {
      auto element = find(key);
      if (!element) {
        return 0;
      }
      return element->getBlobSize();
    }

    bool KeyValue::getInt8(const char* key, int8_t& result) {
      auto element = find(key);
      if (!element) {
        return 0;
      }
      return element->getInt8(result);
    }

    bool KeyValue::getUInt8(const char* key, uint8_t& result) {
      auto element = find(key);
      if (!element) {
        return 0;
      }
      return element->getUInt8(result);
    }

    bool KeyValue::getInt32(const char* key, int32_t& result) {
      auto element = find(key);
      if (!element) {
        return 0;
      }
      return element->getInt32(result);
    }

    bool KeyValue::getUInt32(const char* key, uint32_t& result) {
      auto element = find(key);
      if (!element) {
        return 0;
      }
      return element->getUInt32(result);
    }

    bool KeyValue::setInt8(const char* key, const int8_t value) {
      auto element = findOrCreate(key);
      return element->setInt8(value);
    }

    bool KeyValue::setUInt8(const char* key, const uint8_t value) {
      auto element = findOrCreate(key);
      return element->setUInt8(value);
    }

    bool KeyValue::setInt32(const char* key, const int32_t value) {
      auto element = findOrCreate(key);
      return element->setInt32(value);
    }

    bool KeyValue::setUInt32(const char* key, const uint32_t value) {
      auto element = findOrCreate(key);
      return element->setUInt32(value);
    }

    KeyValueElement::KeyValueElement(const char *keyName) {
      strncpy(key, keyName, SUPLA_STORAGE_KEY_SIZE);
    }

    KeyValueElement::~KeyValueElement() {
      if (size > 0 && data.charPtr && dataType == DATA_TYPE_STRING) {
        delete[] data.charPtr;
        size = 0;
        data.charPtr = nullptr;
      }

      if (size > 0 && data.uint8ptr && dataType == DATA_TYPE_BLOB) {
        delete[] data.uint8ptr;
        size = 0;
        data.uint8ptr = nullptr;
      }
    }

    bool KeyValueElement::isKeyEqual(const char *keyToCheck) {
      return strncmp(key, keyToCheck, SUPLA_STORAGE_KEY_SIZE) == 0;
    }

    KeyValueElement *KeyValueElement::getNext() {
      return next;
    }

    bool KeyValueElement::hasNext() {
      return next != nullptr;
    }

    void KeyValueElement::addNext(KeyValueElement *toBeAdded) {
      if (!hasNext()) {
        next = toBeAdded;
        return;
      }

      auto tmp = getNext();
      next = toBeAdded;
      toBeAdded->addNext(tmp);

    }

    bool KeyValueElement::setString(const char* value) {
      unsigned int newSize = strlen(value) + 1;
      if (dataType == DATA_TYPE_NOT_SET) {
        dataType = DATA_TYPE_STRING;
        size = newSize;
        data.charPtr = new char[size];
      }
      if (dataType != DATA_TYPE_STRING) {
        return false;
      }
      if (newSize != size) {
        delete[] data.charPtr;
        size = newSize;
        data.charPtr = new char[size];
      }
      strncpy(data.charPtr, value, size);
      return true;
    }

    bool KeyValueElement::getString(char* value, size_t maxSize) {
      if (dataType != DATA_TYPE_STRING) {
        return false;
      }

      if (size <= maxSize) {
        strncpy(value, data.charPtr, maxSize);
        return true;
      }
      return false;
    }

    int KeyValueElement::getStringSize() {
      if (dataType != DATA_TYPE_STRING) {
        return 0;
      }
      return size;
    }

    bool KeyValueElement::setBlob(const char* value, size_t blobSize) {
      if (dataType == DATA_TYPE_NOT_SET) {
        dataType = DATA_TYPE_BLOB;
        size = blobSize;
        data.uint8ptr = new uint8_t[size];
      }
      if (dataType != DATA_TYPE_BLOB) {
        return false;
      }
      if (blobSize != size) {
        delete[] data.uint8ptr;
        size = blobSize;
        data.uint8ptr = new uint8_t[size];
      }
      memcpy(data.uint8ptr, value, blobSize);
      return true;
    }

    bool KeyValueElement::getBlob(char* value, size_t blobSize) {
      if (dataType != DATA_TYPE_BLOB) {
        return false;
      }

      if (size == blobSize) {
        memcpy(value, data.uint8ptr, blobSize);
        return true;
      }
      return false;
    }

    int KeyValueElement::getBlobSize() {
      if (dataType != DATA_TYPE_BLOB) {
        return 0;
      }
      return size;
    }

    bool KeyValueElement::getInt8(int8_t& result) {
      if (dataType == DATA_TYPE_INT8) {
        result = data.int8;
        return true;
      }
      return false;
    }

    bool KeyValueElement::getUInt8(uint8_t& result) {
      if (dataType == DATA_TYPE_UINT8) {
        result = data.uint8;
        return true;
      }
      return false;
    }

    bool KeyValueElement::getInt32(int32_t& result) {
      if (dataType == DATA_TYPE_INT32) {
        result = data.int32;
        return true;
      }
      return false;
    }

    bool KeyValueElement::getUInt32(uint32_t& result) {
      if (dataType == DATA_TYPE_UINT32) {
        result = data.uint32;
        return true;
      }
      return false;
    }

    bool KeyValueElement::setInt8(const int8_t value) {
      if (dataType == DATA_TYPE_NOT_SET) {
        dataType = DATA_TYPE_INT8;
      }

      if (dataType == DATA_TYPE_INT8) {
        data.int8 = value;
        return true;
      }

      return false;
    }

    bool KeyValueElement::setUInt8(const uint8_t value) {
      if (dataType == DATA_TYPE_NOT_SET) {
        dataType = DATA_TYPE_UINT8;
      }

      if (dataType == DATA_TYPE_UINT8) {
        data.uint8 = value;
        return true;
      }

      return false;
    }

    bool KeyValueElement::setInt32(const int32_t value) {
      if (dataType == DATA_TYPE_NOT_SET) {
        dataType = DATA_TYPE_INT32;
      }

      if (dataType == DATA_TYPE_INT32) {
        data.int32 = value;
        return true;
      }

      return false;
    }

    bool KeyValueElement::setUInt32(const uint32_t value) {
      if (dataType == DATA_TYPE_NOT_SET) {
        dataType = DATA_TYPE_UINT32;
      }

      if (dataType == DATA_TYPE_UINT32) {
        data.uint32 = value;
        return true;
      }

      return false;
    }


  };
};

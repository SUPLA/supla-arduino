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

#ifndef _supla_storage_key_value_h
#define _supla_storage_key_value_h

#include <stdint.h>
#include <stddef.h>

#define SUPLA_STORAGE_KEY_SIZE 15

namespace Supla {
  namespace Storage {
    class KeyValueElement;

    class KeyValue {
      public:
        ~KeyValue();
        bool setString(const char* key, const char* value);
        bool getString(const char* key, char* value, size_t maxSize);
        int getStringSize(const char* key);

        bool setBlob(const char* key, const char* value, size_t blobSize);
        bool getBlob(const char* key, char* value, size_t blobSize);
        int getBlobSize(const char* key);

        bool getInt8(const char* key, int8_t& result);
        bool getUInt8(const char* key, uint8_t& result);
        bool getInt32(const char* key, int32_t& result);
        bool getUInt32(const char* key, uint32_t& result);

        bool setInt8(const char* key, const int8_t value);
        bool setUInt8(const char* key, const uint8_t value);
        bool setInt32(const char* key, const int32_t value);
        bool setUInt32(const char* key, const uint32_t value);


      protected:
        KeyValueElement *find(const char *key);
        KeyValueElement *findOrCreate(const char *key);
        KeyValueElement *first = nullptr;
    };

    enum DataType {
      DATA_TYPE_NOT_SET,
      DATA_TYPE_UINT8,
      DATA_TYPE_INT8,
      DATA_TYPE_UINT32,
      DATA_TYPE_INT32,
      DATA_TYPE_BLOB,
      DATA_TYPE_STRING
    };

    class KeyValueElement {
      public:
        KeyValueElement(const char *keyName);
        ~KeyValueElement();
        bool isKeyEqual(const char *keyToCheck);
        KeyValueElement *getNext();
        bool hasNext();
        void addNext(KeyValueElement *toBeAdded);
        bool setString(const char* value);
        bool getString(char* value, size_t maxSize);
        int getStringSize();

        bool setBlob(const char* value, size_t blobSize);
        bool getBlob(char* value, size_t blobSize);
        int getBlobSize();

        bool getInt8(int8_t& result);
        bool getUInt8(uint8_t& result);
        bool getInt32(int32_t& result);
        bool getUInt32(uint32_t& result);

        bool setInt8(const int8_t value);
        bool setUInt8(const uint8_t value);
        bool setInt32(const int32_t value);
        bool setUInt32(const uint32_t value);

      protected:
        KeyValueElement *next = nullptr;
        char key[SUPLA_STORAGE_KEY_SIZE] = {};
        enum DataType dataType = DATA_TYPE_NOT_SET;
        unsigned int size = 0; // set only for blob and string
        union {
          uint8_t *uint8ptr = nullptr;
          char *charPtr;
          uint8_t uint8;
          int8_t int8;
          uint32_t uint32;
          int32_t int32;
        } data;

    };
  };  // namespace Storage
};  // namespace Supla

#endif /* _supla_storage_key_value_h */


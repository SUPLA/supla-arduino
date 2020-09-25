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

#include <Arduino.h>

#include "storage.h"

#define SUPLA_STORAGE_VERSION 1

using namespace Supla;

Storage *Storage::instance = nullptr;

Storage *Storage::Instance() {
  return instance;
}

bool Storage::Init() {
  if (Instance()) {
    return Instance()->init();
  }
  return false;
}

bool Storage::ReadState(unsigned char *buf, int size) {
  if (Instance()) {
    return Instance()->readState(buf, size);
  }
  return false;
}

bool Storage::WriteState(const unsigned char *buf, int size) {
  if (Instance()) {
    return Instance()->writeState(buf, size);
  }
  return false;
}

bool Storage::LoadDeviceConfig() {
  if (Instance()) {
    return Instance()->loadDeviceConfig();
  }
  return false;
}

bool Storage::LoadElementConfig() {
  if (Instance()) {
    return Instance()->loadElementConfig();
  }
  return false;
}

void Storage::PrepareState() {
  if (Instance()) {
    Instance()->prepareState();
  }
}

void Storage::FinalizeSaveState() {
  if (Instance()) {
    Instance()->finalizeSaveState();
  }
}

bool Storage::SaveStateAllowed(unsigned long ms) {
  if (Instance()) {
    return Instance()->saveStateAllowed(ms);
  }
  return false;
}

Storage::Storage(unsigned int storageStartingOffset)
    : storageStartingOffset(storageStartingOffset),
      currentStateOffset(0),
      deviceConfigOffset(0),
      elementConfigOffset(0),
      elementStateOffset(0),
      newSectionSize(0),
      newSectionElementsCount(0),
      sectionsCount(0) {
  instance = this;
}

void Storage::prepareState() {
  newSectionSize = 0;
  newSectionElementsCount = 0;
  currentStateOffset = elementStateOffset + sizeof(SectionPreamble);
}

bool Storage::readState(unsigned char *buf, int size) {
  if (elementStateOffset + sizeof(SectionPreamble) + elementStateSize <
      currentStateOffset + size) {
    Serial.println(F("Warning! Attempt to read state outside of section size"));
    return false;
  }
  currentStateOffset += readStorage(currentStateOffset, buf, size);
  return true;
}

bool Storage::writeState(const unsigned char *buf, int size) {
  if (size == 0) {
    return true;
  }

  if (elementStateSize > 0 &&
      elementStateOffset + sizeof(SectionPreamble) + elementStateSize <
          currentStateOffset + size) {
    Serial.println(F("Warning! Attempt to write state outside of section size."));
    Serial.println(
        F("Storage: rewriting element state section. All data will be lost."));
    elementStateSize = 0;
    elementStateOffset = 0;
    return false;
  }

  // Calculation of offset for section data - in case sector is missing
  if (elementStateOffset == 0) {
    Serial.print(F("Initialization of elementStateOffset: "));
    elementStateOffset = storageStartingOffset + sizeof(Preamble);
    if (deviceConfigOffset != 0) {
      elementStateOffset += sizeof(SectionPreamble) + deviceConfigSize;
    }
    if (elementConfigOffset != 0) {
      elementStateOffset += sizeof(SectionPreamble) + elementConfigSize;
    }
    Serial.println(elementStateOffset);

    currentStateOffset = elementStateOffset + sizeof(SectionPreamble);

    sectionsCount++;

    // Update Storage preamble with new section count
    Serial.println(F("Update Storage preamble"));
    unsigned char suplaTag[] = {'S', 'U', 'P', 'L', 'A'};
    Preamble preamble;
    memcpy(preamble.suplaTag, suplaTag, 5);
    preamble.version = SUPLA_STORAGE_VERSION;
    preamble.sectionsCount = sectionsCount;

    updateStorage(
        storageStartingOffset, (unsigned char *)&preamble, sizeof(preamble));
  }

  newSectionElementsCount++;
  newSectionSize += size;

  currentStateOffset += updateStorage(currentStateOffset, buf, size);

  return true;
}

void Storage::finalizeSaveState() {
  SectionPreamble preamble;
  preamble.type = STORAGE_SECTION_TYPE_ELEMENT_STATE;
  preamble.size = newSectionSize;
  preamble.crc1 = 0;
  preamble.crc2 = 0;
  // TODO add crc calculation

  updateStorage(
      elementStateOffset, (unsigned char *)&preamble, sizeof(preamble));

  commit();
}

bool Storage::init() {
  Serial.println(F("Storage initialization"));
  int currentOffset = storageStartingOffset;
  Preamble preamble;
  currentOffset +=
      readStorage(currentOffset, (unsigned char *)&preamble, sizeof(preamble));

  unsigned char suplaTag[] = {'S', 'U', 'P', 'L', 'A'};

  if (memcmp(suplaTag, preamble.suplaTag, 5)) {
    Serial.println(F("Storage: missing Supla tag. Rewriting..."));

    memcpy(preamble.suplaTag, suplaTag, 5);
    preamble.version = SUPLA_STORAGE_VERSION;
    preamble.sectionsCount = 0;

    writeStorage(
        storageStartingOffset, (unsigned char *)&preamble, sizeof(preamble));
    commit();

  } else if (preamble.version != SUPLA_STORAGE_VERSION) {
    Serial.print(F("Storage: storage version ["));
    Serial.print(preamble.version);
    Serial.println(F("] is not supported. Storage not initialized"));
    return false;
  } else {
    Serial.print(F("Storage: Number of sections "));
    Serial.println(preamble.sectionsCount);
  }

  if (preamble.sectionsCount == 0) {
    return true;
  }

  for (int i = 0; i < preamble.sectionsCount; i++) {
    Serial.print(F("Reading section: "));
    Serial.println(i);
    SectionPreamble section;
    int sectionOffset = currentOffset;
    currentOffset +=
        readStorage(currentOffset, (unsigned char *)&section, sizeof(section));

    Serial.print(F("Section type: "));
    Serial.print(static_cast<int>(section.type));
    Serial.print(F("; size: "));
    Serial.println(section.size);

    if (section.crc1 != section.crc2) {
      Serial.println(
          F("Warning! CRC copies on section doesn't match. Please check your "
          "storage hardware"));
    }

    switch (section.type) {
      case STORAGE_SECTION_TYPE_DEVICE_CONFIG: {
        deviceConfigOffset = sectionOffset;
        deviceConfigSize = section.size;
        break;
      }
      case STORAGE_SECTION_TYPE_ELEMENT_CONFIG: {
        elementConfigOffset = sectionOffset;
        elementConfigSize = section.size;
        break;
      }
      case STORAGE_SECTION_TYPE_ELEMENT_STATE: {
        elementStateOffset = sectionOffset;
        elementStateSize = section.size;
        break;
      }
      default: {
        Serial.println(F("Warning! Unknown section type"));
        break;
      }
    }
    currentOffset += section.size;
  }

  return true;
}

bool Storage::loadDeviceConfig() {
  return true;
}

bool Storage::loadElementConfig() {
  return true;
}

int Storage::updateStorage(int offset, const unsigned char *buf, int size) {
  unsigned char currentData[size];
  readStorage(offset, currentData, size, false);

  if (memcmp(currentData, buf, size)) {
    return writeStorage(offset, buf, size);
  }
  return size;
}

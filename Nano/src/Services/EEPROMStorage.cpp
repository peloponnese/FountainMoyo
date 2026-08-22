#include "Services/EEPROMStorage.h"

#include <EEPROM.h>

#include "Services/Settings.h"

constexpr uint32_t EEPROM_MAGIC = 0xF012A55A;

struct StoredSettings
{
    uint32_t magic;
    Settings settings;
};

bool eepromLoad()
{
    StoredSettings data;

    EEPROM.get(0, data);

    if (data.magic == EEPROM_MAGIC)
    {
        settings = data.settings;
        return true;
    }
    else
    {
        settingsBegin();
        eepromSave();
        return false;
    }
}

bool eepromSave()
{
    const StoredSettings data =
    {
        EEPROM_MAGIC,
        settings
    };

    EEPROM.put(0, data);

    return true;
}

void eepromFactoryReset()
{
    settingsBegin();
    eepromSave();
}
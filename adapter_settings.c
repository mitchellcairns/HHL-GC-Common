#include "adapter_settings.h"

adapter_settings_s global_loaded_settings = {0};

// Internal functions for command processing
void _generate_mac()
{
  printf("Generated MAC: ");
  for(uint8_t i = 0; i < 6; i++)
  {
    global_loaded_settings.switch_mac_address[0][i] = adapter_ll_generate_random_8();
    global_loaded_settings.switch_mac_address[1][i] = adapter_ll_generate_random_8();
    global_loaded_settings.switch_mac_address[2][i] = adapter_ll_generate_random_8();
    global_loaded_settings.switch_mac_address[3][i] = adapter_ll_generate_random_8();
  }
  printf("\n");
}

void settings_save()
{
  adapter_ll_write_settings_to_memory(&global_loaded_settings);
}

// Returns true if loaded ok
// returns false if no settings and reset to default
bool settings_load()
{
  adapter_ll_load_settings_from_memory(&global_loaded_settings);

  // Check that the version matches, otherwise reset to default and save.
  if(global_loaded_settings.settings_version != ADAPTER_SETTINGS_VERSION)
  {
    printf("Settings version does not match. Resetting... \n");
    settings_reset_to_default();
    settings_save();
    return false;
  }

  return true;
}

void settings_reset_to_default()
{
  const adapter_settings_s set = {
    .settings_version = ADAPTER_SETTINGS_VERSION,
    .input_mode = INPUT_MODE_SWPRO,
  };
  memcpy(&global_loaded_settings, &set, sizeof(adapter_settings_s));
  _generate_mac();
}


volatile bool _webusb_indicate = false;

void settings_save_webindicate()
{
  _webusb_indicate = true;
}


void settings_set_mode(input_mode_t mode)
{
  global_loaded_settings.input_mode = mode;
}

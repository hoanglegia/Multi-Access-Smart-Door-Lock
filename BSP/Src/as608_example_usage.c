#if 1

#include "bsp_as608.h"
#include "ssd1306_app.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"

bsp_as608_handle_t as608_handle;

void registry_new_finger(uint16_t location_id);
void check_finger(void);
void delete_all_fingers(void);
void display_finger_count(void);
void delete_single_finger(uint16_t finger_id_to_delete);

void registry_new_finger(uint16_t location_id)
{
  bsp_as608_status_t status;

  ssd1306_app_display_message("Enrolling...");
  HAL_Delay(2000);
  ssd1306_app_display_message("Place Finger");

  status = BSP_AS608_NO_FINGER;
  do
  {
    status = bsp_as608_get_image(&as608_handle);
  } 
  while (status == BSP_AS608_NO_FINGER);

  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message(" Get Image FAILED ");
    HAL_Delay(2000);
    return;
  }

  status = bsp_as608_create_char_file(&as608_handle, AS608_BUFFER_1);
  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Create Feat FAILED");
    HAL_Delay(2000);
    return;
  }

  ssd1306_app_display_message("Remove Finger");
  HAL_Delay(2000);
  ssd1306_app_display_message("Place SAME Finger Again");

  status = BSP_AS608_NO_FINGER;
  do
  {
    status = bsp_as608_get_image(&as608_handle);
  } 
  while (status == BSP_AS608_NO_FINGER);

  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Get Image 2 FAILED");
    HAL_Delay(2000);
    return;
  }

  status = bsp_as608_create_char_file(&as608_handle, AS608_BUFFER_2);
  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Create Feat 2 FAILED");
    HAL_Delay(2000);
    return;
  }

  status = bsp_as608_crate_template(&as608_handle);
  if (status == BSP_AS608_NOT_MATCH)
  {
    ssd1306_app_display_message("Fingers Not Match");
    HAL_Delay(2000);
    return;
  }
  else if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Create Template Failed");
    HAL_Delay(2000);
    return;
  }

  status = bsp_as608_store_template(&as608_handle, AS608_BUFFER_1, location_id);
  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Store Failed");
    HAL_Delay(2000);
  }
  else
  {
    char msg[30];
    sprintf(msg, "Stored ID: %d", location_id);
    ssd1306_app_display_message(msg);
    HAL_Delay(2000);
    ssd1306_app_display_message("Enroll OK!");
    HAL_Delay(2000);
  }

}

void check_finger(void)
{
  bsp_as608_status_t status;
  uint16_t found_id = 0;
  uint16_t match_score = 0;

  ssd1306_app_display_message("Place Finger to Check");

  do
  {
    status = bsp_as608_get_image(&as608_handle);
    HAL_Delay(100);
  } 
  while (status == BSP_AS608_NO_FINGER);

  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Get Image Failed");
    HAL_Delay(1500);
    return;
  }

  status = bsp_as608_create_char_file(&as608_handle, AS608_BUFFER_1);
  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Create Feat Failed");
    HAL_Delay(1500);
    return;
  }

  status = bsp_as608_search_fingerprint(&as608_handle, 
                                        AS608_BUFFER_1, 0,
                                        BSP_AS608_MAX_FINGERPRINTS, 
                                        &found_id, &match_score);
  if (status == BSP_AS608_NOT_FOUND)
  {
    ssd1306_app_display_message("Finger Not Found");
    HAL_Delay(2000);
  }
  else if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Search Failed");
    HAL_Delay(2000);
  }
  else
  {
    char msg[30];
    sprintf(msg, " Found ID: %d ", found_id);
    ssd1306_app_display_message(msg);
    HAL_Delay(1000);
    sprintf(msg, " Score: %d ", match_score);
    ssd1306_app_display_message(msg);
    HAL_Delay(3000);
  }

}
void delete_all_fingers(void)
{
  bsp_as608_status_t status;

  ssd1306_app_display_message("Deleting All...");
  HAL_Delay(2000);

  status = bsp_as608_delete_all_templates(&as608_handle);
  if (status != BSP_AS608_OK)
  {
    ssd1306_app_display_message("Delete Failed");
    HAL_Delay(2000);
  }
  else
  {
    ssd1306_app_display_message("All Deleted!");
    HAL_Delay(2000);
  }
}
void delete_single_finger(uint16_t finger_id_to_delete)
{
  char msg[30];

  if (!BSP_AS608_IS_VALID_ID(finger_id_to_delete))
  {
    ssd1306_app_display_message("Invalid ID!");
    HAL_Delay(2000);
    return;
  }

  sprintf(msg, "Deleting ID: %d...", finger_id_to_delete);
  ssd1306_app_display_message(msg);
  HAL_Delay(1000);

  bsp_as608_status_t status = bsp_as608_delete_template(&as608_handle, finger_id_to_delete);

  if (status == BSP_AS608_OK)
  {
    sprintf(msg, "ID %d DELETED!", finger_id_to_delete);
    ssd1306_app_display_message(msg);
    HAL_Delay(3000);
  }
  else
  {
    sprintf(msg, "Delete FAILED! Code:%d", status);
    ssd1306_app_display_message(msg);
    HAL_Delay(3000);
  }
}
void display_finger_count(void)
{
  uint16_t count = 0;
  bsp_as608_status_t status = bsp_as608_get_template_count(&as608_handle, &count);

  if (status == BSP_AS608_OK)
  {
    char msg[25];
    sprintf(msg, "Stored Fingers: %d", count);
    ssd1306_app_display_message("--- Finger List ---");
    ssd1306_app_display_message(msg);
    HAL_Delay(3000);
  }
  else
  {
    ssd1306_app_display_message("Failed to get count");
  }

}
#endif

/**
 * @file tasks.h
 * @author Jordi Gauchía (jgauchia@jgauchia.com)
 * @brief  Core Tasks functions
 * @version 0.1
 * @date 2022-10-10
 */

/**
 * @brief Task 1 - Read GPS data
 *
 * @param pvParameters
 */
void Read_GPS(void *pvParameters)
{
  debug->print(PSTR("Task1 - Read GPS - running on core "));
  debug->println(xPortGetCoreID());
  for (;;)
  {
    while (gps->available() > 0)
    {
      GPS.encode(gps->read());

#ifdef OUTPUT_NMEA
      {
        debug->write(gps->read());
      }
#endif
    }
    vTaskDelay(1);
  }
}

/**
 * @brief Task 2 - Main program
 *
 * @param pvParameters
 */
void Main_prog(void *pvParameters)
{
  debug->print(PSTR("Task2 - Main Program - running on core "));
  debug->println(xPortGetCoreID());
  for (;;)
  {
    delay(1);
  }
}

/**
 * @brief Init Core tasks
 *
 */
void init_tasks()
{
  xTaskCreatePinnedToCore(Read_GPS, PSTR("Read GPS"), 1000, NULL, 1, NULL, 1);
  delay(500);
  // xTaskCreatePinnedToCore(Main_prog, PSTR("Main Program"), 16384, NULL, 4, NULL, 1);
  // delay(500);
}
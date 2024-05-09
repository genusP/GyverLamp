#ifdef USE_BLYNK

#define CYCLE_DONT_OFF          (1U)          // Не отключать режим Цикл при выключении лампы = 1U, отключать = 0U
#define CYCLE_TIMER             (60U)         // Интервал смены эффектов 60 секунд
#define CYCLE_TIMER_PLUS        (0U)          // + случайное время от нуля до 0U секунд
#define CYCLE_1ST_EFFECT        (2U)          // Эффекты до "2. Смена цвета" не будут демонстрироваться (в Блинке это номер 3)
#define CYCLE_LAST_EFFECT       (77U)         // Эффекты после "77. Блуждающий кубик" не будут демонстрироваться (в Блинке это номер 78)

BLYNK_CONNECTED()
{
  updateRemoteBlynkParams();
}

// кнопка POWER ON / OFF
BLYNK_WRITE(V0)
{
  int value = param.asInt();
  if (value == 1)
    processParams("P_ON", "");
  else
    processParams("P_OFF", "");

  updateRemoteBlynkParams();
}

// бегунок яркости от 1 до 255
BLYNK_WRITE(V1)
{
  processParams("BRI", param.asString());
}

// бегунок скорости от 1 до 255
BLYNK_WRITE(V2)
{
  processParams("SPD", param.asString());
}

// бегунок масштаба от 1 до 100
BLYNK_WRITE(V3)
{
  processParams("SCA", param.asString());
}

// выбор эффекта из списка или по номеру
BLYNK_WRITE(V4)
{
  int value = param.asInt() - 1;
  processParams("EFF", String(value).c_str());

  updateRemoteBlynkParams();
}

// кнопка ЦИКЛ ON / OFF
BLYNK_WRITE(V5)
{
  int value = param.asInt();
  if (value == 1){
    //"ON"
    //FavoritesManager::ConfigureFavorites(inputBuffer);
      FavoritesManager::FavoritesRunning = 1U;
      FavoritesManager::nextModeAt = 0;
      FavoritesManager::Interval = CYCLE_TIMER;
      FavoritesManager::Dispersion = CYCLE_TIMER_PLUS;
      FavoritesManager::UseSavedFavoritesRunning = CYCLE_DONT_OFF;
      for (uint8_t i = 0; i < MODE_AMOUNT; i++)
      {
        FavoritesManager::FavoriteModes[i] = (i < CYCLE_1ST_EFFECT || i > CYCLE_LAST_EFFECT) ? 0U : 1U;
      }
  }
  else{
    //"OFF"
    //FavoritesManager::ConfigureFavorites(inputBuffer);
      FavoritesManager::FavoritesRunning = 0U;
  }
  settChanged = true;
  eepromTimeout = millis();
  #if (USE_MQTT)
  MqttManager::needToPublish = true;
  #endif
  
  updateRemoteBlynkParams();
}


void updateRemoteBlynkParams()
{
  if (espMode == 1U && WiFi.isConnected())
  {
    Blynk.virtualWrite(V0, ONflag ? 1 : 0);
    Blynk.virtualWrite(V1, modes[currentMode].Brightness);
    Blynk.virtualWrite(V2, modes[currentMode].Speed);
    Blynk.virtualWrite(V3, modes[currentMode].Scale);
    Blynk.virtualWrite(V4, currentMode + 1);
    Blynk.virtualWrite(V5, FavoritesManager::FavoritesRunning ? 1 : 0);
  }
}

void processParams(char *prefix, const char *paramValue)
{
  char charBuf[16];
  String value = prefix + String(paramValue);
  value.toCharArray(charBuf, 26);
  processInputBuffer(charBuf, NULL, false);
  
  // добавляем сброс настроек на значения по умолчанию при выборе всех единичек на всех бегунках
  if (modes[currentMode].Brightness == 1U && modes[currentMode].Speed == 1U && modes[currentMode].Scale == 1U) {
    restoreSettings();
    loadingFlag = true;
    settChanged = true;
    eepromTimeout = millis();
    #if (USE_MQTT)
    if (espMode == 1U)
    {
      MqttManager::needToPublish = true;
    }
    #endif
    
    updateRemoteBlynkParams();
  }
  #ifdef OTA
  else if ((currentMode == MODE_AMOUNT - 1U) && modes[currentMode].Brightness == 255U && modes[currentMode].Speed == 255U && modes[currentMode].Scale == 100U){
    // добавляем включение прошивки по воздуху
      modes[currentMode].Brightness = 10U;
      modes[currentMode].Speed      = 99U;
      modes[currentMode].Scale      = 38U;        
      otaManager.RequestOtaUpdate();
      FastLED.delay(70);
      //if (otaManager.RequestOtaUpdate()) по идее, нужен положительный ответ от менеджера, но он не поступает с первого раза...
      otaManager.RequestOtaUpdate();
      //{
        currentMode = EFF_MATRIX;                             // принудительное включение режима "Матрица" для индикации перехода в режим обновления по воздуху
        FastLED.clear();
        FastLED.delay(1);
        ONflag = true;
        changePower();
      //}
      updateRemoteBlynkParams();
  }
  #endif // OTA
}

#endif

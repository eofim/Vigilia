class Vigilia_Logger extends PluginBase {
  FileHandle m_LogFile;
  FileHandle m_DeathLogFile;
  FileHandle m_DisconnectLogFile;

  static string logSep = VIGILIA_LOG_SEPARATOR;

  // clang-format off
  private RestContext lootRestContext;
  private RestContext deathRestContext;
  private RestContext disconnectRestContext;
  // clang-format on

  void Vigilia_Logger() {
    if (isHost()) {
      Print(Vigilia_Constant+ " Logger Init");

      if (!FileExist(VIGILIA_ROOT_DIR)) {
        Print(Vigilia_Constant+ "'" + VIGILIA_ROOT_DIR + "' does not exist, creating...");
        MakeDirectory(VIGILIA_ROOT_DIR);
      }

      if (!FileExist(VIGILIA_LOGS_DIR)) {
        Print(Vigilia_Constant+ "'" + VIGILIA_LOGS_DIR + "' does not exist, creating...");
        MakeDirectory(VIGILIA_LOGS_DIR);
      }

      if (!FileExist(VIGILIA_LOGS_ARCHIVES_DIR)) {
        Print(Vigilia_Constant+ "'" + VIGILIA_LOGS_ARCHIVES_DIR + "' does not exist, creating...");
        MakeDirectory(VIGILIA_LOGS_ARCHIVES_DIR);
      }

      ArchiveOldLogs();
      CreateNewLogFiles();
    }
  };

  void ~Vigilia_Logger() {
    if (isHost()) {
      Print(Vigilia_Constant+ " Logger Closed");
      CloseFile(m_LogFile);
      CloseFile(m_DeathLogFile);
      CloseFile(m_DisconnectLogFile);
    }
  };

  bool isHost() {
    if (!GetGame()) {
      return false;
    }
    return GetGame().IsMultiplayer();
  }

  void ArchiveOldLogs() {
    string currentTime = GetDate(false) + "_" + GetTime("-");
    
    // Arquivar log de loot
    if (FileExist(VIGILIA_LOGS_FILENAME)) {
      string newLootFileName = VIGILIA_LOGS_ARCHIVES_DIR + currentTime + "_loot.log";
      CopyFile(VIGILIA_LOGS_FILENAME, newLootFileName);
      DeleteFile(VIGILIA_LOGS_FILENAME);
    }
    
    // Arquivar log de morte
    if (FileExist(VIGILIA_DEATH_LOGS_FILENAME)) {
      string newDeathFileName = VIGILIA_LOGS_ARCHIVES_DIR + currentTime + "_death.log";
      CopyFile(VIGILIA_DEATH_LOGS_FILENAME, newDeathFileName);
      DeleteFile(VIGILIA_DEATH_LOGS_FILENAME);
    }
    
    // Arquivar log de desconexão
    if (FileExist(VIGILIA_DISCONNECT_LOGS_FILENAME)) {
      string newDisconnectFileName = VIGILIA_LOGS_ARCHIVES_DIR + currentTime + "_disconnect.log";
      CopyFile(VIGILIA_DISCONNECT_LOGS_FILENAME, newDisconnectFileName);
      DeleteFile(VIGILIA_DISCONNECT_LOGS_FILENAME);
    }
  }

  void CreateNewLogFiles() {
    Print(Vigilia_Constant+ " Logger Create Log Files");
    Vigilia_ModConfig config = GetVigilia_ModConfig();

    if (isHost()) {
      // Criar arquivo de log de loot
      if (config.isLootLoggingEnabled) {
        m_LogFile = OpenFile(VIGILIA_LOGS_FILENAME, FileMode.WRITE);
        if (m_LogFile != 0) {
          FPrintln(m_LogFile, "Date Time" + logSep + "Action" + logSep + "persistentID" + logSep + "itemType" + logSep + "itemName" + logSep + "isStackable" + logSep + "stackSize" + logSep + "distance" + logSep + "oldLocText" + logSep + "oldLocParentType" + logSep + "oldLocParentName" + logSep + "oldLocPos" + logSep + "oldParentPersistentID" + logSep + "oldSteamId" + logSep + "oldName" + logSep + "oldPlayerPos" + logSep + "newLocText" + logSep + "newLocParentType" + logSep + "newLocParentName" + logSep + "newLocPos" + logSep + "newParentPersistentID" + logSep + "newSteamId" + logSep + "newName" + logSep + "newPlayerPos");
        } else {
          Print(Vigilia_Constant+ "ERROR: Unable to create " + VIGILIA_LOGS_FILENAME);
        }
      }

      // Criar arquivo de log de morte
      if (config.isDeathLoggingEnabled) {
        m_DeathLogFile = OpenFile(VIGILIA_DEATH_LOGS_FILENAME, FileMode.WRITE);
        if (m_DeathLogFile != 0) {
          FPrintln(m_DeathLogFile, "Date Time" + logSep + "Action" + logSep + "PlayerSteamId" + logSep + "PlayerName" + logSep + "PlayerPos" + logSep + "DeathType" + logSep + "DeathCause" + logSep + "KillerSteamId" + logSep + "KillerName" + logSep + "KillerPos" + logSep + "GameTime" + logSep + "Health" + logSep + "Blood" + logSep + "Shock");
        } else {
          Print(Vigilia_Constant+ "ERROR: Unable to create " + VIGILIA_DEATH_LOGS_FILENAME);
        }
      }

      // Criar arquivo de log de desconexão
      if (config.isDisconnectLoggingEnabled) {
        m_DisconnectLogFile = OpenFile(VIGILIA_DISCONNECT_LOGS_FILENAME, FileMode.WRITE);
        if (m_DisconnectLogFile != 0) {
          FPrintln(m_DisconnectLogFile, "Date Time" + logSep + "Action" + logSep + "PlayerSteamId" + logSep + "PlayerName" + logSep + "PlayerPos" + logSep + "Reason" + logSep + "Health" + logSep + "Blood" + logSep + "InCombat" + logSep + "NearEnemies" + logSep + "GameTime");
        } else {
          Print(Vigilia_Constant+ "ERROR: Unable to create " + VIGILIA_DISCONNECT_LOGS_FILENAME);
        }
      }
    }
  };

  void Log(string text) { 
    if (m_LogFile) {
      FPrintln(m_LogFile, GetDate(true) + " " + GetTime() + logSep + text); 
    }
  };

  void LogDeath(string text) { 
    if (m_DeathLogFile) {
      FPrintln(m_DeathLogFile, GetDate(true) + " " + GetTime() + logSep + text); 
    }
  };

  void LogDisconnect(string text) { 
    if (m_DisconnectLogFile) {
      FPrintln(m_DisconnectLogFile, GetDate(true) + " " + GetTime() + logSep + text); 
    }
  };

  void Debug(string text) { 
    if (GetVigilia_ModConfig().isDebugEnabled) {
      Log("DEBUG" + logSep + text); 
    }
  };

  static string GetDate(bool forLog, string sep = "-") {
    int year, month, day;
    GetYearMonthDay(year, month, day);

    string y = year.ToString();
    string m = month.ToStringLen(2);
    string d = day.ToStringLen(2);

    if (forLog) {
      return d + sep + m + sep + y;
    }

    return y + sep + m + sep + d;
  }

  static string GetTime(string sep = ":") {
    int hour, minute, second;
    GetHourMinuteSecond(hour, minute, second);

    string h = hour.ToStringLen(2);
    string m = minute.ToStringLen(2);
    string s = second.ToStringLen(2);

    return h + sep + m + sep + s;
  }

  static string GetParedPosition(vector position) {
    if (position) {
      return position[0].ToString() + "," + position[2].ToString();
    }
    return "-1,-1";
  }

  static string GetLocType(InventoryLocationType type) {
    switch (type) {
    case InventoryLocationType.GROUND: {
      return "GROUND";
    }
    case InventoryLocationType.ATTACHMENT: {
      return "ATTACHMENT";
    }
    case InventoryLocationType.CARGO: {
      return "CARGO";
    }
    case InventoryLocationType.HANDS: {
      return "HANDS";
    }
    case InventoryLocationType.PROXYCARGO: {
      return "PROXYCARGO";
    }
    default: {
      return "UNKNOWN";
    }
    }
    return "UNKNOWN";
  }

  static string GetActionType(Vigilia_ActionType type) {
    switch (type) {
    case Vigilia_ActionType.MOVE: {
      return "MOVE";
    }
    case Vigilia_ActionType.TAKE: {
      return "TAKE";
    }
    case Vigilia_ActionType.DROP: {
      return "DROP";
    }
    case Vigilia_ActionType.DEATH: {
      return "DEATH";
    }
    case Vigilia_ActionType.DISCONNECT: {
      return "DISCONNECT";
    }
    default: {
      return "UNKNOWN";
    }
    }
    return "UNKNOWN";
  }

  static string GetDeathType(Vigilia_DeathType type) {
    switch (type) {
    case Vigilia_DeathType.PVP: {
      return "PVP";
    }
    case Vigilia_DeathType.PVE_ZOMBIE: {
      return "PVE_ZOMBIE";
    }
    case Vigilia_DeathType.PVE_ANIMAL: {
      return "PVE_ANIMAL";
    }
    case Vigilia_DeathType.ENVIRONMENTAL: {
      return "ENVIRONMENTAL";
    }
    case Vigilia_DeathType.SUICIDE: {
      return "SUICIDE";
    }
    case Vigilia_DeathType.OBJECT: {
      return "OBJECT";
    }
    default: {
      return "UNKNOWN";
    }
    }
    return "UNKNOWN";
  }

  static string GetPlayerInfo(PlayerBase player) {
    if (!player || !player.GetIdentity() || !player.GetIdentity().GetPlainId()) {
      return "0\tunknown\t-1,-1";
    }

    string playerInfo = player.GetIdentity().GetPlainId() + logSep + player.GetIdentity().GetName() + logSep + GetParedPosition(player.GetPosition());
    return playerInfo;
  }

  void SendToDiscord(string url, string title, string message) {
    if (url == "") {
      return;
    }

    if (!lootRestContext) {
      lootRestContext = GetRestApi().GetRestContext(url);
      lootRestContext.SetHeader("application/json");
    }

    LogDiscordMessage data = new LogDiscordMessage(title, message);
    string jsonData = JsonFileLoader<LogDiscordMessage>.JsonMakeData(data);
    string payload = "{\"embeds\":[" + jsonData + "]}";
    
    lootRestContext.POST(null, "", payload);
  }
};

class LogDiscordMessage {
  string title;
  string description;
  int color = 15744574; // Cor laranja padrão

  void LogDiscordMessage(string ttle, string descr) {
    title = ttle;
    description = descr;
    
    // Definir cores específicas baseadas no título
    if (ttle.Contains("Morte")) {
      color = 15158332; // Vermelho para mortes
    } else if (ttle.Contains("Desconexão")) {
      color = 15105570; // Laranja escuro para desconexões
    } else if (ttle.Contains("cheater")) {
      color = 15744574; // Laranja para cheats
    }
  }
};
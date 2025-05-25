class MPG_TDML_ModConfig {
  int configVersion = 2;
  string documentation = "https://docs.mpg-dayz.ru/tdm-logs/";
  string timestampApiUrl;
  bool isModDisabled = false;
  bool isDebugEnabled = false;

  // === CONFIGURAÇÕES GERAIS DE LOOT LOGGING ===
  bool isLootLoggingEnabled = true;
  float maxDistance = 5.0;
  ref TStringArray ignoreItems = new TStringArray;
  ref TStringArray ignoreContainers = new TStringArray;

  // === CONFIGURAÇÕES DE DISCORD PARA LOOT ===
  string lootDiscordUrl;
  string lootDiscordTitle = "PossÃ­vel cheater";

  // === CONFIGURAÇÕES DE DEATH LOGGING ===
  bool isDeathLoggingEnabled = true;
  string deathDiscordUrl;
  string deathDiscordTitle = "Morte de Personagem";
  
  // === CONFIGURAÇÕES DE DISCONNECT LOGGING ===
  bool isDisconnectLoggingEnabled = false;
  string disconnectDiscordUrl;
  string disconnectDiscordTitle = "DesconexÃ£o Suspeita";

  void MPG_TDML_ModConfig() {
    if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
      Print(MPG_TDML + " Init");
      LoadConfig();
    }
  }

  bool IsIgnoredItem(string type) { return IsIgnored(type, ignoreItems); }
  bool IsIgnoredContainer(string type) { return IsIgnored(type, ignoreContainers); }

  // clang-format off
  private bool IsIgnored(string type, ref TStringArray itemsToCheck) {
    foreach (string ignoreItem : itemsToCheck) {
      bool isStrictCheck = false;
      string ignoreItemClass = ignoreItem;

      // Verifica se o item contém "|", o que indica modo de comparação estrita
      if (ignoreItem.Contains("|")) {
        TStringArray params = new TStringArray;
        ignoreItem.Split("|", params);

        // Verificações segura do conteúdo de params
        if (params.Count() >= 2) {
          ignoreItemClass = params[0];
          isStrictCheck = params[1].ToInt() == 1;
        } else if (params.Count() == 1) {
          ignoreItemClass = params[0];
        }
      }

      // comparação estrita (nome exato)
      if (isStrictCheck) {
        if (ignoreItemClass == type) {
          return true;
        }
      }
      // comparação por herança usando IsKindOf
      else {
        if (GetGame().IsKindOf(type, ignoreItemClass)) {
          return true;
        }
      }
    }
    return false;
  }


  // clang-format off
  private void LoadConfig() {
    // clang-format on
    if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
      if (FileExist(MPG_TDML_CONFIG_FILENAME)) {
        Print(MPG_TDML + " config found, loading...");
        JsonFileLoader<MPG_TDML_ModConfig>.JsonLoadFile(MPG_TDML_CONFIG_FILENAME, this);
        UpdateConfig();
      } else {
        Print(MPG_TDML + " config NOT found, creating...");
        CreateMPG_TDML_ModConfig();
      }
    }
  }

  // clang-format off
  private void CreateMPG_TDML_ModConfig() {
    // clang-format on
    if (!FileExist(MPG_TDML_ROOT_DIR)) {
      Print(MPG_TDML + "'" + MPG_TDML_ROOT_DIR + "' does not exist, creating...");
      MakeDirectory(MPG_TDML_ROOT_DIR);
    }

    // Configurações padrão
    ignoreItems = { "Apple", "CowboyHat_green", "HuntingKnife|1" };
    ignoreContainers = { "AliceBag_Black", "Bear_Dark" };

    // URLs de exemplo (devem ser configuradas pelo usuário)
    lootDiscordUrl = "";
    deathDiscordUrl = "";
    disconnectDiscordUrl = "";

    JsonFileLoader<MPG_TDML_ModConfig>.JsonSaveFile(MPG_TDML_CONFIG_FILENAME, this);
    Print(MPG_TDML + " Default config created. Please configure Discord URLs in: " + MPG_TDML_CONFIG_FILENAME);
  }

  // clang-format off
  private void UpdateConfig() {
    // clang-format on
    bool needsUpdate = false;
    
    if (!configVersion || configVersion < 2) {
      configVersion = 2;
      
      // Adicionar novas Configurações se não existirem
      if (!isLootLoggingEnabled) isLootLoggingEnabled = true;
      if (!isDeathLoggingEnabled) isDeathLoggingEnabled = true;
      if (!isDisconnectLoggingEnabled) isDisconnectLoggingEnabled = false;
      
      needsUpdate = true;
    }
    
    if (needsUpdate) {
      JsonFileLoader<MPG_TDML_ModConfig>.JsonSaveFile(MPG_TDML_CONFIG_FILENAME, this);
      Print(MPG_TDML + " Config updated to version " + configVersion);
    }
  }
}

static ref MPG_TDML_ModConfig g_MPG_TDML_ModConfig;

static ref MPG_TDML_ModConfig GetMPG_TDML_ModConfig() {
  if (!g_MPG_TDML_ModConfig) {
    g_MPG_TDML_ModConfig = new MPG_TDML_ModConfig();
  }
  return g_MPG_TDML_ModConfig;
}
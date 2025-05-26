class Vigilia_ModConfig {
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
  string disconnectDiscordTitle = "Desconexão Suspeita";

  void Vigilia_ModConfig() {
    if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
      Print(Vigilia_Constant+ " Init");
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
      if (FileExist(VIGILIA_CONFIG_FILENAME)) {
        Print(Vigilia_Constant+ " config found, loading...");
        JsonFileLoader<Vigilia_ModConfig>.JsonLoadFile(VIGILIA_CONFIG_FILENAME, this);
        UpdateConfig();
      } else {
        Print(Vigilia_Constant+ " config NOT found, creating...");
        CreateVigilia_ModConfig();
      }
    }
  }

  // clang-format off
  private void CreateVigilia_ModConfig() {
    // clang-format on
    if (!FileExist(VIGILIA_ROOT_DIR)) {
      Print(Vigilia_Constant+ "'" + VIGILIA_ROOT_DIR + "' does not exist, creating...");
      MakeDirectory(VIGILIA_ROOT_DIR);
    }

    // Configurações padrão
    ignoreItems = { "Apple", "CowboyHat_green", "HuntingKnife|1" };
    ignoreContainers = { "AliceBag_Black", "Bear_Dark" };

    // URLs de exemplo (devem ser configuradas pelo usuário)
    lootDiscordUrl = "";
    deathDiscordUrl = "";
    disconnectDiscordUrl = "";

    JsonFileLoader<Vigilia_ModConfig>.JsonSaveFile(VIGILIA_CONFIG_FILENAME, this);
    Print(Vigilia_Constant+ " Default config created. Please configure Discord URLs in: " + VIGILIA_CONFIG_FILENAME);
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
      JsonFileLoader<Vigilia_ModConfig>.JsonSaveFile(VIGILIA_CONFIG_FILENAME, this);
      Print(Vigilia_Constant+ " Config updated to version " + configVersion);
    }
  }
}

static ref Vigilia_ModConfig g_Vigilia_ModConfig;

static ref Vigilia_ModConfig GetVigilia_ModConfig() {
  if (!g_Vigilia_ModConfig) {
    g_Vigilia_ModConfig = new Vigilia_ModConfig();
  }
  return g_Vigilia_ModConfig;
}
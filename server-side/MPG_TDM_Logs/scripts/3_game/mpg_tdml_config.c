class MPG_TDML_ModConfig {
  int configVersion = 2;
  string documentation = "https://docs.mpg-dayz.ru/tdm-logs/";
  bool isModDisabled = false;
  bool isDebugEnabled = false;

  // === CONFIGURAÇÕES GERAIS DE LOOT LOGGING ===
  bool isLootLoggingEnabled = true;
  float maxDistance = 5.0;
  ref TStringArray ignoreItems = new TStringArray;
  ref TStringArray ignoreContainers = new TStringArray;

  // === CONFIGURAÇÕES DE DISCORD PARA LOOT ===
  string lootDiscordUrl;
  string lootDiscordTitle = "Possível cheater";
  string lootDiscordTextTpl = "`%1`\nItem: **%2**\nDistância: %3 m.\n**Coordenadas**\nDe: `%4` [mapa](https://www.izurvive.com/chernarusplus/#location=%4;5)\nPara: `%5` [mapa](https://www.izurvive.com/chernarusplus/#location=%5;5)\nid do item: `%6`\n**Log completo:**\n```%7```";

  // === CONFIGURAÇÕES DE DEATH LOGGING ===
  bool isDeathLoggingEnabled = true;
  string deathDiscordUrl;
  string deathDiscordTitle = "Morte de Personagem";
  string deathDiscordTextTpl = "**🔴 MORTE DE PERSONAGEM**\n\n**Jogador:** `%1`\n**Causa da Morte:** %2\n**Assassino/Causa:** %3\n**Localização:** `%4` [mapa](https://www.izurvive.com/chernarusplus/#location=%4;5)\n**Data/Hora:** %5\n**Tempo de Jogo:** %6s\n\n**Log completo:**\n```%7```";
  
  // === CONFIGURAÇÕES DE DISCONNECT LOGGING ===
  bool isDisconnectLoggingEnabled = false;
  string disconnectDiscordUrl;
  string disconnectDiscordTitle = "Desconexão Suspeita";
  string disconnectDiscordTextTpl = "**⚠️ DESCONEXÃO SUSPEITA**\n\n**Jogador:** `%1`\n**Situação:** %2\n**Localização:** `%3` [mapa](https://www.izurvive.com/chernarusplus/#location=%3;5)\n**Vida:** %4%\n**Data/Hora:** %5\n\n**Detalhes:**\n```%6```";

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
    // clang-format on
    foreach (string ignoreItem : itemsToCheck) {
      bool isStrictCheck = false;
      string ignoreItemClass = ignoreItem;

      if (ignoreItem.Contains("|")) {
        TStringArray params = new TStringArray;
        ignoreItem.Split("|", params);
        ignoreItemClass = params[0];
        isStrictCheck = params[1] == "1";
      }

      if (isStrictCheck) {
        if (ignoreItemClass == type) {
          return true;
        }
      } else {
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
      
      // Adicionar novas configurações se não existirem
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
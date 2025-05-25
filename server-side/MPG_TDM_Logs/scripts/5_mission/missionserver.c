modded class MissionServer extends MissionBase {
  // Aguardando correção do bug https://feedback.bistudio.com/T183081
  // void MissionServer() { Não pode usar o construtor

  override void OnInit() {
    super.OnInit();
    
    // Inicializar apenas no servidor
    if (GetGame().IsServer() || !GetGame().IsMultiplayer()) {
      Print(MPG_TDML + " MissionServer Init");
      
      // Resetar configuração global para garantir carregamento correto
      g_MPG_TDML_ModConfig = NULL;
      GetMPG_TDML_ModConfig();
      
      // Verificar se as configurações foram carregadas corretamente
      MPG_TDML_ModConfig config = GetMPG_TDML_ModConfig();
      if (config) {
        Print(MPG_TDML + " Config loaded successfully");
        Print(MPG_TDML + " Loot Logging: " + config.isLootLoggingEnabled.ToString());
        Print(MPG_TDML + " Death Logging: " + config.isDeathLoggingEnabled.ToString());
        Print(MPG_TDML + " Disconnect Logging: " + config.isDisconnectLoggingEnabled.ToString());
        Print(MPG_TDML + " Debug Mode: " + config.isDebugEnabled.ToString());
        
        if (config.isModDisabled) {
          Print(MPG_TDML + " WARNING: Mod is disabled in config!");
        }
      } else {
        Print(MPG_TDML + " ERROR: Failed to load config!");
      }
    }
  }

  override void OnMissionFinish() {
    super.OnMissionFinish();
    
    if (GetGame().IsServer() || !GetGame().IsMultiplayer()) {
      Print(MPG_TDML + " MissionServer Finish");
      
      // Limpar recursos se necessário
      if (g_MPG_TDML_ModConfig) {
        Print(MPG_TDML + " Cleaning up config resources");
      }
    }
  }
}
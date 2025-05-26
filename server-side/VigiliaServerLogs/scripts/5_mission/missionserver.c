modded class MissionServer extends MissionBase {
  // Aguardando correção do bug https://feedback.bistudio.com/T183081
  // void MissionServer() { Não pode usar o construtor

  override void OnInit() {
    super.OnInit();
    
    // Inicializar apenas no servidor
    if (GetGame().IsServer() || !GetGame().IsMultiplayer()) {
      Print(Vigilia_Constant + " MissionServer iniciado");
      
      // Resetar configuração global para garantir carregamento correto
      g_Vigilia_ModConfig = NULL;
      GetVigilia_ModConfig();
      
      // Verificar se as configurações foram carregadas corretamente
      Vigilia_ModConfig config = GetVigilia_ModConfig();
      if (config) {
        Print(Vigilia_Constant + " Configuração carregada com sucesso");
        Print(Vigilia_Constant + " Registro de loot: " + config.isLootLoggingEnabled.ToString());
        Print(Vigilia_Constant + " Registro de mortes: " + config.isDeathLoggingEnabled.ToString());
        Print(Vigilia_Constant + " Registro de desconexões: " + config.isDisconnectLoggingEnabled.ToString());
        Print(Vigilia_Constant + " Modo de depuração: " + config.isDebugEnabled.ToString());
        
        if (config.isModDisabled) {
          Print(Vigilia_Constant + " AVISO: O mod está desativado na configuração!");
        }
      } else {
        Print(Vigilia_Constant + " ERRO: Falha ao carregar a configuração!");
      }
    }
  }

  override void OnMissionFinish() {
    super.OnMissionFinish();
    
    if (GetGame().IsServer() || !GetGame().IsMultiplayer()) {
      Print(Vigilia_Constant + " MissionServer finalizado");
      
      // Limpar recursos se necessário
      if (g_Vigilia_ModConfig) {
        Print(Vigilia_Constant + " Limpando recursos da configuração");
      }
    }
  }
}

modded class PluginManager {
  override void Init() {
    super.Init();
    
    // Registrar o plugin principal de logging
    RegisterPlugin("MPG_TDM_Logger", false, true);
    
    Print(MPG_TDML + " PluginManager - MPG_TDM_Logger registered");
  };
};
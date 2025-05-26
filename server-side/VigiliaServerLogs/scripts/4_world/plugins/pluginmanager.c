modded class PluginManager {
  override void Init() {
    super.Init();
    
    // Registrar o plugin principal de logging
    RegisterPlugin("Vigilia_Logger", false, true);
    
    Print(Vigilia_Constant+ " PluginManager - Vigilia_Logger registered");
  };
};
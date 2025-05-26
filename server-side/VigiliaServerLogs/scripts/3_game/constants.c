const string Vigilia_Constant= "[VIGILIALogs]";
const string VIGILIA_LOG_SEPARATOR = "\t";
const string VIGILIA_ROOT_DIR = "$profile:VIGILIALogs\\";
const string VIGILIA_CONFIG_FILENAME = VIGILIA_ROOT_DIR + "Config.json";
const string VIGILIA_LOGS_DIR = VIGILIA_ROOT_DIR + "Logs\\";
const string VIGILIA_LOGS_ARCHIVES_DIR = VIGILIA_LOGS_DIR + "LogArchives\\";

// Arquivos de log específicos
const string VIGILIA_LOGS_FILENAME = VIGILIA_LOGS_DIR + "VIGILIALog.log";
const string VIGILIA_DEATH_LOGS_FILENAME = VIGILIA_LOGS_DIR + "VIGILIADeathLog.log";
const string VIGILIA_DISCONNECT_LOGS_FILENAME = VIGILIA_LOGS_DIR + "VIGILIADisconnectLog.log";

// Enum expandido para tipos de ação
enum Vigilia_ActionType {
  MOVE,       // Movimento de loot entre inventários no chão, ou no inventário de um jogador
  TAKE,       // Entrada de loot no inventário de um jogador
  DROP,       // Saída de loot do inventário de um jogador
  DEATH,      // Morte de personagem
  DISCONNECT  // Desconexão suspeita
};

// Enum para tipos de morte
enum Vigilia_DeathType {
  PVP,          // Jogador matou jogador
  PVE_ZOMBIE,   // Zombie matou jogador
  PVE_ANIMAL,   // Animal matou jogador
  ENVIRONMENTAL,// Morte ambiental (fome, sede, frio, etc)
  SUICIDE,      // Suicídio
  OBJECT,       // Morte por objeto/acidente
  UNKNOWN       // Causa desconhecida
};
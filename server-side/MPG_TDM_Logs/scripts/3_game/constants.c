const string MPG_TDML = "[MPG_TDMLogs]";
const string MPG_TDML_LOG_SEPARATOR = "\t";
const string MPG_TDML_ROOT_DIR = "$profile:MPG_TDMLogs\\";
const string MPG_TDML_CONFIG_FILENAME = MPG_TDML_ROOT_DIR + "Config.json";
const string MPG_TDML_LOGS_DIR = MPG_TDML_ROOT_DIR + "Logs\\";
const string MPG_TDML_LOGS_ARCHIVES_DIR = MPG_TDML_LOGS_DIR + "LogArchives\\";

// Arquivos de log específicos
const string MPG_TDML_LOGS_FILENAME = MPG_TDML_LOGS_DIR + "MPG_TDMLog.log";
const string MPG_TDML_DEATH_LOGS_FILENAME = MPG_TDML_LOGS_DIR + "MPG_DeathLog.log";
const string MPG_TDML_DISCONNECT_LOGS_FILENAME = MPG_TDML_LOGS_DIR + "MPG_DisconnectLog.log";

// Enum expandido para tipos de ação
enum MPG_TDML_ActionType {
  MOVE,       // Movimento de loot entre inventários no chão, ou no inventário de um jogador
  TAKE,       // Entrada de loot no inventário de um jogador
  DROP,       // Saída de loot do inventário de um jogador
  DEATH,      // Morte de personagem
  DISCONNECT  // Desconexão suspeita
};

// Enum para tipos de morte
enum MPG_TDML_DeathType {
  PVP,          // Jogador matou jogador
  PVE_ZOMBIE,   // Zombie matou jogador
  PVE_ANIMAL,   // Animal matou jogador
  ENVIRONMENTAL,// Morte ambiental (fome, sede, frio, etc)
  SUICIDE,      // Suicídio
  OBJECT,       // Morte por objeto/acidente
  UNKNOWN       // Causa desconhecida
};
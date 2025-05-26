// Corrigido para evitar uso direto de IsPlayer() em Object
modded class PlayerBase {
  Vigilia_Logger VigiliaLogger = Vigilia_Logger.Cast(GetPlugin(Vigilia_Logger));
  Vigilia_ModConfig VIGILIA_Config = g_Vigilia_ModConfig;

  private bool m_VIGILIA_IsInCombat = false;
  private float m_VIGILIA_LastCombatTime = 0;
  private vector m_VIGILIA_LastKnownPosition;
  private bool m_VIGILIA_PlayerLoaded = false;

  override void OnPlayerLoaded() {
    super.OnPlayerLoaded();
    m_VIGILIA_LastKnownPosition = GetPosition();
    m_VIGILIA_PlayerLoaded = true;

    if (VIGILIA_Config && VIGILIA_Config.isDebugEnabled) {
      VigiliaLogger.Debug("Player loaded: " + GetIdentity().GetName());
    }
  }

  override void EEKilled(Object killer) {
    super.EEKilled(killer);

    if (VIGILIA_Config && VIGILIA_Config.isDeathLoggingEnabled && m_VIGILIA_PlayerLoaded) {
      LogPlayerDeath(killer);
    }
  }

  override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef) {
    super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

    PlayerBase attacker = PlayerBase.Cast(source);
    if (attacker) {
      m_VIGILIA_IsInCombat = true;
      m_VIGILIA_LastCombatTime = GetGame().GetTime();

      if (VIGILIA_Config && VIGILIA_Config.isDebugEnabled && attacker.GetIdentity()) {
        VigiliaLogger.Debug("Player " + GetIdentity().GetName() + " entered combat with " + attacker.GetIdentity().GetName());
      }
    }
  }

  // Função auxiliar fora de LogPlayerDeath
  string GetShockCategory(float shockValue) {
      float invertedShock = 100 - shockValue;
      if (invertedShock <= 0) {
          return "Nenhum trauma físico (" + invertedShock.ToString() + "%)";
      } else if (invertedShock <= 49) {
          return "Trauma leve (" + invertedShock.ToString() + "%)";
      } else if (invertedShock <= 74) {
          return "Trauma moderado (" + invertedShock.ToString() + "%)";
      } else {
          return "Velório caixão fechado (" + invertedShock.ToString() + "%)";
      }
  }

  void LogPlayerDeath(Object killer) {
      if (!VigiliaLogger || !m_VIGILIA_PlayerLoaded || !GetIdentity()) {
        return;
      }

      string playerInfo = VigiliaLogger.GetPlayerInfo(this);
      string killerInfo = "Unknown";
      string deathCause = "Unknown";
      Vigilia_DeathType deathType = Vigilia_DeathType.UNKNOWN;
      vector deathPos = GetPosition();

      float health = GetHealth01("GlobalHealth", "Health") * 100;
      float blood = GetHealth01("GlobalHealth", "Blood") * 5000;
      float shock = GetHealth01("GlobalHealth", "Shock") * 100;

      // Primeiro tenta cast direto para PlayerBase
      PlayerBase killerPlayer = PlayerBase.Cast(killer);
      string weaponUsed = "";
      
      // Se não for PlayerBase diretamente, tenta buscar o dono da arma/item
      if (!killerPlayer && killer) {
        // Captura o nome da arma/objeto que causou a morte
        weaponUsed = killer.GetType();
        
        // Se for uma arma ou item, tenta buscar quem estava segurando
        EntityAI killerEntity = EntityAI.Cast(killer);
        if (killerEntity) {
          // Busca o jogador mais próximo que pode ter usado a arma
          array<Man> players = new array<Man>;
          GetGame().GetPlayers(players);
          
          float closestDistance = 1000; // Distância máxima para considerar
          PlayerBase closestPlayer = null;
          
          foreach (Man player : players) {
            if (player != this && player.IsAlive()) {
              PlayerBase pb = PlayerBase.Cast(player);
              if (pb && pb.GetIdentity()) {
                float dist = vector.Distance(GetPosition(), pb.GetPosition());
                if (dist < closestDistance) {
                  closestDistance = dist;
                  closestPlayer = pb;
                }
              }
            }
          }
          
          // Se encontrou um jogador próximo (dentro de 100m), considera como killer
          if (closestPlayer && closestDistance <= 100) {
            killerPlayer = closestPlayer;
          }
        }
      }
      
      if (killerPlayer && killerPlayer.GetIdentity()) {
        // Verificar se o assassino é a própria vítima (suicídio)
        if (killerPlayer.GetIdentity().GetPlainId() == GetIdentity().GetPlainId()) {
          killerInfo = VigiliaLogger.GetPlayerInfo(this);
          if (weaponUsed != "") {
            deathCause = "Suicídio com " + weaponUsed + " - " + GetIdentity().GetName();
          } else {
            deathCause = "Suicídio - " + GetIdentity().GetName();
          }
          deathType = Vigilia_DeathType.SUICIDE;
        } else {
          killerInfo = VigiliaLogger.GetPlayerInfo(killerPlayer);
          if (weaponUsed != "") {
            deathCause = "PvP com " + weaponUsed + " - " + killerPlayer.GetIdentity().GetName();
          } else {
            deathCause = "PvP - " + killerPlayer.GetIdentity().GetName();
          }
          deathType = Vigilia_DeathType.PVP;
        }
      } else if (killer && killer.IsInherited(ZombieBase)) {
        deathCause = "Zombie - " + killer.GetType();
        deathType = Vigilia_DeathType.PVE_ZOMBIE;
        killerInfo = "0" + VIGILIA_LOG_SEPARATOR + killer.GetType() + VIGILIA_LOG_SEPARATOR + VigiliaLogger.GetParedPosition(killer.GetPosition());
      } else if (killer && killer.IsInherited(AnimalBase)) {
        deathCause = "Animal - " + killer.GetType();
        deathType = Vigilia_DeathType.PVE_ANIMAL;
        killerInfo = "0" + VIGILIA_LOG_SEPARATOR + killer.GetType() + VIGILIA_LOG_SEPARATOR + VigiliaLogger.GetParedPosition(killer.GetPosition());
      } else if (killer) {
        deathCause = "Object - " + killer.GetType();
        deathType = Vigilia_DeathType.OBJECT;
        killerInfo = "0" + VIGILIA_LOG_SEPARATOR + killer.GetType() + VIGILIA_LOG_SEPARATOR + VigiliaLogger.GetParedPosition(killer.GetPosition());
      } else {
        // CORREÇÃO: Lógica melhorada para classificar mortes ambientais
        // Priorizar as causas mais específicas primeiro
        if (blood <= 0) {
          deathCause = "Blood Loss (Bleeding)";
          deathType = Vigilia_DeathType.ENVIRONMENTAL;
        } else if (health <= 0) {
          deathCause = "Health Loss (Starvation/Disease)";
          deathType = Vigilia_DeathType.ENVIRONMENTAL;
        } else if (shock >= 100) {
          deathCause = "Unconsciousness/Shock";
          deathType = Vigilia_DeathType.ENVIRONMENTAL;
        } else {
          // Apenas mortes que não se enquadram em nenhuma categoria específica
          // devem ser consideradas ambientais genéricas
          deathCause = "Environmental";
          deathType = Vigilia_DeathType.ENVIRONMENTAL;
        }
        killerInfo = "0" + VIGILIA_LOG_SEPARATOR + "Environmental" + VIGILIA_LOG_SEPARATOR + VigiliaLogger.GetParedPosition(deathPos);
      }

      string playerSteamId = GetIdentity().GetPlainId();
      string playerName = GetIdentity().GetName();
      string playerPos = VigiliaLogger.GetParedPosition(deathPos);

      string sep = VIGILIA_LOG_SEPARATOR;
      string logEntry = "DEATH" + sep + playerSteamId + sep + playerName + sep + playerPos;
      logEntry += sep + VigiliaLogger.GetDeathType(deathType) + sep + deathCause;
      logEntry += sep + killerInfo + sep + GetGame().GetTime().ToString();
      logEntry += sep + health.ToString() + sep + blood.ToString() + sep + shock.ToString();

      // Converter tempo do jogo para timestamp Unix
      int unixTimestamp = (int)(GetGame().GetTickTime() / 1000) + 946684800; // Ajuste baseado no ano 2000

      // Mensagem ajustada para killfeed
      string message = "**Jogador:** " + playerName + "\n"; // ||(" + playerSteamId + ")||
      message = message + "**Causa da Morte:** " + deathCause + "\n";
      // message = message + "**Posição:** " + playerPos + "\n";
      // message = message + "**Hora:** <t:" + unixTimestamp.ToString() + "> (<t:" + unixTimestamp.ToString() + ":R>)\n";

      // Informações de status no momento da morte
      message = message + "**Status Final:**\n";
      message = message + " - Vida: " + health.ToString() + "%\n";
      message = message + " - Sangue: " + blood.ToString() + "/5000\n";
      message = message + " - Trauma Corporal: " + GetShockCategory(shock) + "\n\n";
      
      // Informações do killer se houver
      if (killerPlayer && killerPlayer.GetIdentity()) {
          if (killerPlayer.GetIdentity().GetPlainId() == GetIdentity().GetPlainId()) {
              // message = message + "**Tipo:** Suicídio\n";
          } else {
              message = message + "**Assassino:** " + killerPlayer.GetIdentity().GetName() + "\n";
          }
      } else if (killer) {
          message = message + "**Morto por:** " + killer.GetType() + "\n";
      }
      
      // message = message + "**Log Completo:**\n" + logEntry;

      VigiliaLogger.LogDeath(logEntry);
      VigiliaLogger.SendToDiscord(VIGILIA_Config.deathDiscordUrl, VIGILIA_Config.deathDiscordTitle, message);

      if (VIGILIA_Config.isDebugEnabled) {
        VigiliaLogger.Debug("Player death logged: " + playerName + " (" + playerSteamId + ") - Cause: " + deathCause);
      }
  }

  void LogPlayerDisconnect(string reason) {
    if (!VigiliaLogger || !m_VIGILIA_PlayerLoaded || !GetIdentity()) {
      return;
    }

    string playerSteamId = GetIdentity().GetPlainId();
    string playerName = GetIdentity().GetName();
    vector playerPos = GetPosition();

    float health = GetHealth01("GlobalHealth", "Health") * 100;
    float blood = GetHealth01("GlobalHealth", "Blood") * 5000;
    float healthPercent = health;

    bool inCombat = (GetGame().GetTime() - m_VIGILIA_LastCombatTime) < 30;
    bool nearEnemies = false;
    int nearbyPlayerCount = 0;

    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    foreach (Man player : players) {
      if (player != this && player.IsAlive()) {
        float dist = vector.Distance(GetPosition(), player.GetPosition());
        if (dist < 100) {
          nearbyPlayerCount++;
          if (dist < 50) {
            nearEnemies = true;
          }
        }
      }
    }

    bool suspiciousDisconnect = false;
    string suspiciousReason = "";

    if (inCombat) {
      suspiciousDisconnect = true;
      suspiciousReason += "Em combate recente; ";
    }
    if (nearEnemies) {
      suspiciousDisconnect = true;
      suspiciousReason += "Jogadores muito proximos; ";
    }
    if (healthPercent < 25) {
      suspiciousDisconnect = true;
      suspiciousReason += "Saude critica (" + healthPercent.ToString() + "%); ";
    }
    if (blood < 2500) {
      suspiciousDisconnect = true;
      suspiciousReason += "Sangue baixo (" + blood.ToString() + "); ";
    }

    if (suspiciousDisconnect || VIGILIA_Config.isDebugEnabled) {
      string sep = VIGILIA_LOG_SEPARATOR;
      string logEntry = "DISCONNECT" + sep + playerSteamId + sep + playerName + sep + VigiliaLogger.GetParedPosition(playerPos) + sep + reason + sep + healthPercent.ToString() + sep + blood.ToString() + sep + inCombat.ToString() + sep + nearbyPlayerCount.ToString();

      VigiliaLogger.LogDisconnect(logEntry);

      if (VIGILIA_Config.isDebugEnabled) {
        VigiliaLogger.Debug("Player disconnect logged: " + playerName + " (" + playerSteamId + ") - Suspicious: " + suspiciousDisconnect.ToString() + " - Reason: " + suspiciousReason);
      }
    }
  }

  override void OnCommandMoveStart() {
    super.OnCommandMoveStart();
    if (m_VIGILIA_PlayerLoaded) {
      m_VIGILIA_LastKnownPosition = GetPosition();
    }
  }

  override void OnCommandMoveFinish() {
    super.OnCommandMoveFinish();

    if (m_VIGILIA_IsInCombat && (GetGame().GetTime() - m_VIGILIA_LastCombatTime) > 60) {
      m_VIGILIA_IsInCombat = false;

      if (VIGILIA_Config && VIGILIA_Config.isDebugEnabled) {
        VigiliaLogger.Debug("Player " + GetIdentity().GetName() + " exited combat mode");
      }
    }
  }

  void ForceLogDisconnect(string customReason) {
    if (VIGILIA_Config && VIGILIA_Config.isDisconnectLoggingEnabled) {
      LogPlayerDisconnect(customReason);
    }
  }
};
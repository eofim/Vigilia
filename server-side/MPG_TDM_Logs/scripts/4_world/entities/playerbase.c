// Corrigido para evitar uso direto de IsPlayer() em Object
modded class PlayerBase {
  MPG_TDM_Logger MPG_TDMLogger = MPG_TDM_Logger.Cast(GetPlugin(MPG_TDM_Logger));
  MPG_TDML_ModConfig MPG_TDML_Config = g_MPG_TDML_ModConfig;

  private bool m_MPG_IsInCombat = false;
  private float m_MPG_LastCombatTime = 0;
  private vector m_MPG_LastKnownPosition;
  private bool m_MPG_PlayerLoaded = false;

  override void OnPlayerLoaded() {
    super.OnPlayerLoaded();
    m_MPG_LastKnownPosition = GetPosition();
    m_MPG_PlayerLoaded = true;

    if (MPG_TDML_Config && MPG_TDML_Config.isDebugEnabled) {
      MPG_TDMLogger.Debug("Player loaded: " + GetIdentity().GetName());
    }
  }

  override void EEKilled(Object killer) {
    super.EEKilled(killer);

    if (MPG_TDML_Config && MPG_TDML_Config.isDeathLoggingEnabled && m_MPG_PlayerLoaded) {
      LogPlayerDeath(killer);
    }
  }

  override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef) {
    super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

    PlayerBase attacker = PlayerBase.Cast(source);
    if (attacker) {
      m_MPG_IsInCombat = true;
      m_MPG_LastCombatTime = GetGame().GetTime();

      if (MPG_TDML_Config && MPG_TDML_Config.isDebugEnabled && attacker.GetIdentity()) {
        MPG_TDMLogger.Debug("Player " + GetIdentity().GetName() + " entered combat with " + attacker.GetIdentity().GetName());
      }
    }
  }

  void LogPlayerDeath(Object killer) {
    if (!MPG_TDMLogger || !m_MPG_PlayerLoaded || !GetIdentity()) {
      return;
    }

    string playerInfo = MPG_TDMLogger.GetPlayerInfo(this);
    string killerInfo = "Unknown";
    string deathCause = "Unknown";
    MPG_TDML_DeathType deathType = MPG_TDML_DeathType.UNKNOWN;
    vector deathPos = GetPosition();

    float health = GetHealth01("GlobalHealth", "Health") * 100;
    float blood = GetHealth01("GlobalHealth", "Blood") * 5000;
    float shock = GetHealth01("GlobalHealth", "Shock") * 100;

    PlayerBase killerPlayer = PlayerBase.Cast(killer);
    if (killerPlayer && killerPlayer.GetIdentity()) {
      killerInfo = MPG_TDMLogger.GetPlayerInfo(killerPlayer);
      deathCause = "PvP - " + killerPlayer.GetIdentity().GetName();
      deathType = MPG_TDML_DeathType.PVP;
    } else if (killer && killer.IsInherited(ZombieBase)) {
      deathCause = "Zombie - " + killer.GetType();
      deathType = MPG_TDML_DeathType.PVE_ZOMBIE;
      killerInfo = "0" + MPG_TDML_LOG_SEPARATOR + killer.GetType() + MPG_TDML_LOG_SEPARATOR + MPG_TDMLogger.GetParedPosition(killer.GetPosition());
    } else if (killer && killer.IsInherited(AnimalBase)) {
      deathCause = "Animal - " + killer.GetType();
      deathType = MPG_TDML_DeathType.PVE_ANIMAL;
      killerInfo = "0" + MPG_TDML_LOG_SEPARATOR + killer.GetType() + MPG_TDML_LOG_SEPARATOR + MPG_TDMLogger.GetParedPosition(killer.GetPosition());
    } else if (killer) {
      deathCause = "Object - " + killer.GetType();
      deathType = MPG_TDML_DeathType.OBJECT;
      killerInfo = "0" + MPG_TDML_LOG_SEPARATOR + killer.GetType() + MPG_TDML_LOG_SEPARATOR + MPG_TDMLogger.GetParedPosition(killer.GetPosition());
    } else {
      if (health <= 0 && blood > 50) {
        deathCause = "Health Loss (Starvation/Disease)";
        deathType = MPG_TDML_DeathType.ENVIRONMENTAL;
      } else if (blood <= 0) {
        deathCause = "Blood Loss (Bleeding)";
        deathType = MPG_TDML_DeathType.ENVIRONMENTAL;
      } else if (shock >= 100) {
        deathCause = "Unconsciousness/Shock";
        deathType = MPG_TDML_DeathType.ENVIRONMENTAL;
      } else {
        deathCause = "Environmental/Suicide";
        deathType = MPG_TDML_DeathType.SUICIDE;
      }
      killerInfo = "0" + MPG_TDML_LOG_SEPARATOR + "Environmental" + MPG_TDML_LOG_SEPARATOR + MPG_TDMLogger.GetParedPosition(deathPos);
    }

    string playerSteamId = GetIdentity().GetPlainId();
    string playerName = GetIdentity().GetName();
    string playerPos = MPG_TDMLogger.GetParedPosition(deathPos);

    string sep = MPG_TDML_LOG_SEPARATOR;
    string logEntry = "DEATH" + sep + playerSteamId + sep + playerName + sep + playerPos;
    logEntry += sep + MPG_TDMLogger.GetDeathType(deathType) + sep + deathCause;
    logEntry += sep + killerInfo + sep + GetGame().GetTime().ToString();
    logEntry += sep + health.ToString() + sep + blood.ToString() + sep + shock.ToString();


    MPG_TDMLogger.LogDeath(logEntry);

    if (MPG_TDML_Config.isDebugEnabled) {
      MPG_TDMLogger.Debug("Player death logged: " + playerName + " (" + playerSteamId + ") - Cause: " + deathCause);
    }
  }

  void LogPlayerDisconnect(string reason) {
    if (!MPG_TDMLogger || !m_MPG_PlayerLoaded || !GetIdentity()) {
      return;
    }

    string playerSteamId = GetIdentity().GetPlainId();
    string playerName = GetIdentity().GetName();
    vector playerPos = GetPosition();

    float health = GetHealth01("GlobalHealth", "Health") * 100;
    float blood = GetHealth01("GlobalHealth", "Blood") * 5000;
    float healthPercent = health;

    bool inCombat = (GetGame().GetTime() - m_MPG_LastCombatTime) < 30;
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

    if (suspiciousDisconnect || MPG_TDML_Config.isDebugEnabled) {
      string sep = MPG_TDML_LOG_SEPARATOR;
      string logEntry = "DISCONNECT" + sep + playerSteamId + sep + playerName + sep + MPG_TDMLogger.GetParedPosition(playerPos) + sep + reason + sep + healthPercent.ToString() + sep + blood.ToString() + sep + inCombat.ToString() + sep + nearbyPlayerCount.ToString();

      MPG_TDMLogger.LogDisconnect(logEntry);

      if (MPG_TDML_Config.isDebugEnabled) {
        MPG_TDMLogger.Debug("Player disconnect logged: " + playerName + " (" + playerSteamId + ") - Suspicious: " + suspiciousDisconnect.ToString() + " - Reason: " + suspiciousReason);
      }
    }
  }

  override void OnCommandMoveStart() {
    super.OnCommandMoveStart();
    if (m_MPG_PlayerLoaded) {
      m_MPG_LastKnownPosition = GetPosition();
    }
  }

  override void OnCommandMoveFinish() {
    super.OnCommandMoveFinish();

    if (m_MPG_IsInCombat && (GetGame().GetTime() - m_MPG_LastCombatTime) > 60) {
      m_MPG_IsInCombat = false;

      if (MPG_TDML_Config && MPG_TDML_Config.isDebugEnabled) {
        MPG_TDMLogger.Debug("Player " + GetIdentity().GetName() + " exited combat mode");
      }
    }
  }

  void ForceLogDisconnect(string customReason) {
    if (MPG_TDML_Config && MPG_TDML_Config.isDisconnectLoggingEnabled) {
      LogPlayerDisconnect(customReason);
    }
  }
};
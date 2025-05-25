modded class ItemBase {
  MPG_TDM_Logger MPG_TDMLogger = MPG_TDM_Logger.Cast(GetPlugin(MPG_TDM_Logger));
  MPG_TDML_ModConfig MPG_TDML_Config = g_MPG_TDML_ModConfig;

  private bool MPG_TDML_isFlagSet = false;
  private bool MPG_TDML_Ignored = false;

  bool MPG_TDML_IsIgnored() {
    if (!MPG_TDML_Config) {
      return false;
    }
    if (!MPG_TDML_isFlagSet) {
      if (GetInventory() && GetInventory().GetCargo()) {
        MPG_TDML_Ignored = MPG_TDML_Config.IsIgnoredContainer(GetType());
      } else {
        MPG_TDML_Ignored = MPG_TDML_Config.IsIgnoredItem(GetType());
      }
      MPG_TDML_isFlagSet = true;
    }
    return MPG_TDML_Ignored;
  }

  string MPG_TDML_GetItemId() {
    int b1, b2, b3, b4;
    GetPersistentID(b1, b2, b3, b4);
    
    if (b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0) {
      vector pos = GetPosition();
      string posStr = pos[0].ToString() + pos[1].ToString() + pos[2].ToString();
      return GetType() + "_" + posStr.Hash().ToString();
    }
    
    return b1.ToString() + "-" + b2.ToString() + "-" + b3.ToString() + "-" + b4.ToString();
  }

  string MPG_TDML_GetLocParentId(notnull InventoryLocation itemLoc) {
    ItemBase parentItem = ItemBase.Cast(itemLoc.GetParent());
    string persisId = "0-0-0-0";

    if (parentItem) {
      if (itemLoc.GetType() == InventoryLocationType.ATTACHMENT || itemLoc.GetType() == InventoryLocationType.CARGO) {
        persisId = parentItem.MPG_TDML_GetItemId();
      }
    }
    return persisId;
  }

  void MPG_TDMLogItemBase(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc) {
    if (!MPG_TDML_Config || MPG_TDML_Config.isModDisabled || !MPG_TDML_Config.isLootLoggingEnabled) {
      return;
    }

    InventoryLocationType oldLocType = oldLoc.GetType();
    InventoryLocationType newLocType = newLoc.GetType();

    if (oldLocType == InventoryLocationType.UNKNOWN) {
      return;
    }
    if (oldLocType == InventoryLocationType.GROUND && newLocType == InventoryLocationType.GROUND) {
      return;
    }

    if (MPG_TDML_IsIgnored()) {
      return;
    }

    bool oldCargoIgnored = false;
    bool newCargoIgnored = false;

    if (oldLocType == InventoryLocationType.CARGO) {
      ItemBase oldCargo = ItemBase.Cast(oldLoc.GetParent());
      if (oldCargo && oldCargo.MPG_TDML_IsIgnored()) {
        oldCargoIgnored = true;
      }
    }

    if (newLocType == InventoryLocationType.CARGO) {
      ItemBase newCargo = ItemBase.Cast(newLoc.GetParent());
      if (newCargo && newCargo.MPG_TDML_IsIgnored()) {
        newCargoIgnored = true;
      }
    }

    if (oldCargoIgnored || newCargoIgnored) {
      return;
    }

    EntityAI oldLocParent = oldLoc.GetParent();
    EntityAI newLocParent = newLoc.GetParent();
    string oldLocParentType = "null";
    string newLocParentType = "null";
    string oldLocParentName = "null";
    string newLocParentName = "null";
    vector oldLocPos;
    vector newLocPos;

    if ((oldLocParent == newLocParent) && (oldLocType == InventoryLocationType.CARGO && newLocType == InventoryLocationType.CARGO)) {
      return;
    }

    string sep = MPG_TDML_LOG_SEPARATOR;
    string persistentID = MPG_TDML_GetItemId();
    string itemType = this.GetType();
    string itemName = this.GetDisplayName();
    
    // Determinar se é stackable de forma simples
    bool isStackable = false;
    int stackSize = 1;
    int maxQuantity = 1;
    
    InventoryItem invItem = InventoryItem.Cast(this);
    if (invItem) {
      maxQuantity = invItem.GetQuantityMax();
      stackSize = invItem.GetQuantity();
      if (maxQuantity > 1) {
        isStackable = true;
      }
    }

    string oldLocText = MPG_TDMLogger.GetLocType(oldLocType);
    string newLocText = MPG_TDMLogger.GetLocType(newLocType);

    PlayerBase new_player = null;
    PlayerBase old_player = null;
    float distance = -1.0;
    vector itemPos;

    if (oldLocParent) {
      oldLocParentType = oldLocParent.GetType();
      oldLocParentName = oldLocParent.GetDisplayName();
      oldLocPos = oldLocParent.GetHierarchyRoot().GetPosition();
      old_player = PlayerBase.Cast(oldLocParent.GetHierarchyRootPlayer());
    }

    if (newLocParent) {
      newLocParentType = newLocParent.GetType();
      newLocParentName = newLocParent.GetDisplayName();
      newLocPos = newLocParent.GetHierarchyRoot().GetPosition();
      new_player = PlayerBase.Cast(newLocParent.GetHierarchyRootPlayer());
    }

    string oldPlayerInfo = MPG_TDMLogger.GetPlayerInfo(old_player);
    string newPlayerInfo = MPG_TDMLogger.GetPlayerInfo(new_player);

    if (old_player && old_player == new_player) {
      return;
    }

    MPG_TDML_ActionType actionType = MPG_TDML_ActionType.MOVE;

    if (oldLocType == InventoryLocationType.GROUND || (!old_player && new_player)) {
      actionType = MPG_TDML_ActionType.TAKE;
    }

    if (newLocType == InventoryLocationType.GROUND || (old_player && !new_player)) {
      actionType = MPG_TDML_ActionType.DROP;
    }

    if (!old_player && new_player) {
      if (new_player.IsPlayerLoaded()) {
        if (oldLocParent) {
          itemPos = oldLocPos;
        } else {
          itemPos = GetPosition();
        }

        if (itemPos[0] > 0) {
          distance = vector.Distance(new_player.GetPosition(), itemPos);
        }
      }
    } else {
      if (oldLocPos[0] > 0 && newLocPos[0] > 0) {
        distance = vector.Distance(oldLocPos, newLocPos);
      }
    }

    string oldPlace = oldLocText + sep + oldLocParentType + sep + oldLocParentName + sep + MPG_TDMLogger.GetParedPosition(oldLocPos) + sep + MPG_TDML_GetLocParentId(oldLoc) + sep + oldPlayerInfo;
    string newPlace = newLocText + sep + newLocParentType + sep + newLocParentName + sep + MPG_TDMLogger.GetParedPosition(newLocPos) + sep + MPG_TDML_GetLocParentId(newLoc) + sep + newPlayerInfo;

    string actionStr = MPG_TDMLogger.GetActionType(actionType);
    string distanceStr = distance.ToString();
    string stackableStr = "false";
    if (isStackable) {
      stackableStr = "true";
    }
    string stackSizeStr = stackSize.ToString();
    
    string logEntry = actionStr + sep + persistentID + sep + itemType + sep + itemName + sep + stackableStr + sep + stackSizeStr + sep + distanceStr + sep + oldPlace + sep + newPlace;
    
    MPG_TDMLogger.Log(logEntry);

    if (distance >= MPG_TDML_Config.maxDistance && MPG_TDML_Config.lootDiscordUrl != string.Empty) {
      string pInfo = newPlayerInfo;
      if (!new_player) {
        pInfo = oldPlayerInfo;
      }
      
      string itemDisplay = itemName + " (" + itemType + ")";
      if (isStackable) {
        itemDisplay = itemDisplay + " x" + stackSizeStr;
      }
      
      string oldPosStr = oldLocPos[0].ToString() + ";" + oldLocPos[2].ToString();
      string newPosStr = newLocPos[0].ToString() + ";" + newLocPos[2].ToString();
      string cheatLog = "CHEAT" + sep + logEntry;
      
      // Construir mensagem manualmente sem Replace()
      string message = "**DETECCAO DE CHEAT - LOOT**\n\n";
      message = message + "**Jogador:** " + pInfo + "\n";
      message = message + "**Item:** " + itemDisplay + "\n";
      message = message + "**Distancia:** " + distanceStr + " metros\n\n";
      message = message + "**Coordenadas:**\n";
      message = message + "De: " + oldPosStr + "\n";
      message = message + "Para: " + newPosStr + "\n\n";
      message = message + "**ID do Item:** " + persistentID + "\n\n";
      message = message + "**Log Completo:**\n" + cheatLog;

      MPG_TDMLogger.Log(cheatLog);
      MPG_TDMLogger.SendToDiscord(MPG_TDML_Config.lootDiscordUrl, MPG_TDML_Config.lootDiscordTitle, message);
    }
  }

  override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc) {
    super.EEItemLocationChanged(oldLoc, newLoc);
    MPG_TDMLogItemBase(oldLoc, newLoc);
  }
};
modded class ItemBase {
  Vigilia_Logger VigiliaLogger = Vigilia_Logger.Cast(GetPlugin(Vigilia_Logger));
  Vigilia_ModConfig VIGILIA_Config = g_Vigilia_ModConfig;

  private bool VIGILIA_isFlagSet = false;
  private bool VIGILIA_Ignored = false;

  bool VIGILIA_IsIgnored() {
    if (!VIGILIA_Config) {
      return false;
    }
    if (!VIGILIA_isFlagSet) {
      if (GetInventory() && GetInventory().GetCargo()) {
        VIGILIA_Ignored = VIGILIA_Config.IsIgnoredContainer(GetType());
      } else {
        VIGILIA_Ignored = VIGILIA_Config.IsIgnoredItem(GetType());
      }
      VIGILIA_isFlagSet = true;
    }
    return VIGILIA_Ignored;
  }

  string VIGILIA_GetItemId() {
    int b1, b2, b3, b4;
    GetPersistentID(b1, b2, b3, b4);
    
    if (b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0) {
      vector pos = GetPosition();
      string posStr = pos[0].ToString() + pos[1].ToString() + pos[2].ToString();
      return GetType() + "_" + posStr.Hash().ToString();
    }
    
    return b1.ToString() + "-" + b2.ToString() + "-" + b3.ToString() + "-" + b4.ToString();
  }

  string VIGILIA_GetLocParentId(notnull InventoryLocation itemLoc) {
    ItemBase parentItem = ItemBase.Cast(itemLoc.GetParent());
    string persisId = "0-0-0-0";

    if (parentItem) {
      if (itemLoc.GetType() == InventoryLocationType.ATTACHMENT || itemLoc.GetType() == InventoryLocationType.CARGO) {
        persisId = parentItem.VIGILIA_GetItemId();
      }
    }
    return persisId;
  }

  void VIGILIALogItemBase(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc) {
    if (!VIGILIA_Config || VIGILIA_Config.isModDisabled || !VIGILIA_Config.isLootLoggingEnabled) {
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

    if (VIGILIA_IsIgnored()) {
      return;
    }

    bool oldCargoIgnored = false;
    bool newCargoIgnored = false;

    if (oldLocType == InventoryLocationType.CARGO) {
      ItemBase oldCargo = ItemBase.Cast(oldLoc.GetParent());
      if (oldCargo && oldCargo.VIGILIA_IsIgnored()) {
        oldCargoIgnored = true;
      }
    }

    if (newLocType == InventoryLocationType.CARGO) {
      ItemBase newCargo = ItemBase.Cast(newLoc.GetParent());
      if (newCargo && newCargo.VIGILIA_IsIgnored()) {
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

    string sep = VIGILIA_LOG_SEPARATOR;
    string persistentID = VIGILIA_GetItemId();
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

    string oldLocText = VigiliaLogger.GetLocType(oldLocType);
    string newLocText = VigiliaLogger.GetLocType(newLocType);

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

    string oldPlayerInfo = VigiliaLogger.GetPlayerInfo(old_player);
    string newPlayerInfo = VigiliaLogger.GetPlayerInfo(new_player);

    if (old_player && old_player == new_player) {
      return;
    }

    Vigilia_ActionType actionType = Vigilia_ActionType.MOVE;

    if (oldLocType == InventoryLocationType.GROUND || (!old_player && new_player)) {
      actionType = Vigilia_ActionType.TAKE;
    }

    if (newLocType == InventoryLocationType.GROUND || (old_player && !new_player)) {
      actionType = Vigilia_ActionType.DROP;
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

    string oldPlace = oldLocText + sep + oldLocParentType + sep + oldLocParentName + sep + VigiliaLogger.GetParedPosition(oldLocPos) + sep + VIGILIA_GetLocParentId(oldLoc) + sep + oldPlayerInfo;
    string newPlace = newLocText + sep + newLocParentType + sep + newLocParentName + sep + VigiliaLogger.GetParedPosition(newLocPos) + sep + VIGILIA_GetLocParentId(newLoc) + sep + newPlayerInfo;

    string actionStr = VigiliaLogger.GetActionType(actionType);
    string distanceStr = distance.ToString();
    string stackableStr = "false";
    if (isStackable) {
      stackableStr = "true";
    }
    string stackSizeStr = stackSize.ToString();
    
    string logEntry = actionStr + sep + persistentID + sep + itemType + sep + itemName + sep + stackableStr + sep + stackSizeStr + sep + distanceStr + sep + oldPlace + sep + newPlace;
    
    VigiliaLogger.Log(logEntry);

    if (distance >= VIGILIA_Config.maxDistance && VIGILIA_Config.lootDiscordUrl != string.Empty) {
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

      VigiliaLogger.Log(cheatLog);
      VigiliaLogger.SendToDiscord(VIGILIA_Config.lootDiscordUrl, VIGILIA_Config.lootDiscordTitle, message);
    }
  }

  override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc) {
    super.EEItemLocationChanged(oldLoc, newLoc);
    VIGILIALogItemBase(oldLoc, newLoc);
  }
};
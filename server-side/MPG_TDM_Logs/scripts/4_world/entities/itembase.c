modded class ItemBase {
  MPG_TDM_Logger MPG_TDMLogger = MPG_TDM_Logger.Cast(GetPlugin(MPG_TDM_Logger));
  MPG_TDML_ModConfig MPG_TDML_Config = g_MPG_TDML_ModConfig;

  // clang-format off
  private bool MPG_TDML_isFlagSet = false;
  private bool MPG_TDML_Ignored = false;
  // clang-format on

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

  string MPG_TDML_GeiId() {
    int b1, b2, b3, b4;
    GetPersistentID(b1, b2, b3, b4);

    return string.Format("%1-%2-%3-%4", b1, b2, b3, b4);
  }

  string MPG_TDML_GetLocParentId(notnull InventoryLocation itemLoc) {
    ItemBase parentItem = ItemBase.Cast(itemLoc.GetParent());
    string persisId = "0-0-0-0";

    if (parentItem) {
      if (itemLoc.GetType() == InventoryLocationType.ATTACHMENT || itemLoc.GetType() == InventoryLocationType.CARGO) {
        persisId = parentItem.MPG_TDML_GeiId();
      }
    }

    return persisId;
  }

  void MPG_TDMLogItemBase(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc) {
    // Se o mod está desabilitado, não carregado, ou loot logging desabilitado, não faz sentido continuar
    if (!MPG_TDML_Config || MPG_TDML_Config.isModDisabled || !MPG_TDML_Config.isLootLoggingEnabled) {
      return;
    }

    InventoryLocationType oldLocType = oldLoc.GetType();
    InventoryLocationType newLocType = newLoc.GetType();

    // Se o local antigo é apenas criado, pula
    if (oldLocType == InventoryLocationType.UNKNOWN) {
      return;
    }
    // Se tanto o local antigo quanto o novo são chão, pula, isso não acontece, são logs lixo.
    if (oldLocType == InventoryLocationType.GROUND && newLocType == InventoryLocationType.GROUND) {
      return;
    }

    // Se o item está na lista de ignorados, pula
    if (MPG_TDML_IsIgnored()) {
      if (MPG_TDML_Config.isDebugEnabled) {
        MPG_TDMLogger.Debug("Item Ignored: " + GetType());
      }
      return;
    }

    // Agora vamos verificar se os contêineres entre os quais o movimento ocorre estão na lista de ignorados
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

    if (MPG_TDML_Config.isDebugEnabled) {
      MPG_TDMLogger.Debug("===================================");
    }

    // Se o local antigo ou novo é um contêiner ignorado - pula.
    if (oldCargoIgnored || newCargoIgnored) {
      if (MPG_TDML_Config.isDebugEnabled) {
        MPG_TDMLogger.Debug("oldCargoIgnored: " + oldCargoIgnored + "  newCargoIgnored: " + newCargoIgnored);
      }
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

    // Se o local antigo e o novo são o mesmo, e ambos são contêineres, pula
    if ((oldLocParent == newLocParent) && (oldLocType == InventoryLocationType.CARGO && newLocType == InventoryLocationType.CARGO)) {
      if (MPG_TDML_Config.isDebugEnabled) {
        MPG_TDMLogger.Debug("same place, ignored");
      }
      return;
    }

    string sep = MPG_TDML_LOG_SEPARATOR;
    string persistentID = MPG_TDML_GeiId();

    string itemType = this.GetType();
    string itemName = this.GetDisplayName();

    string oldLocText = MPG_TDMLogger.GetLocType(oldLocType);
    string newLocText = MPG_TDMLogger.GetLocType(newLocType);

    if (MPG_TDML_Config.isDebugEnabled) {
      // Vamos simplificar um pouco o código
      MPG_TDMLogger.Debug("persistentID" + sep + persistentID);
      MPG_TDMLogger.Debug("itemType    " + sep + itemType);

      MPG_TDMLogger.Debug("===== oldLoc");
      MPG_TDMLogger.Debug("oldLocText  " + sep + oldLocText);
      MPG_TDMLogger.Debug("oldLocParent" + sep + oldLocParent);
      if (oldLocParent) {
        MPG_TDMLogger.Debug("oldId   " + sep + MPG_TDML_GetLocParentId(oldLoc));
        MPG_TDMLogger.Debug("oldLocPlayer" + sep + oldLocParent.GetHierarchyRootPlayer());
      }

      MPG_TDMLogger.Debug("===== newLoc");
      MPG_TDMLogger.Debug("newLocText  " + sep + newLocText);
      MPG_TDMLogger.Debug("newLocParent" + sep + newLocParent);
      if (newLocParent) {
        MPG_TDMLogger.Debug("newId   " + sep + MPG_TDML_GetLocParentId(newLoc));
        MPG_TDMLogger.Debug("newLocPlayer" + sep + newLocParent.GetHierarchyRootPlayer());
      }
    }

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

    // Se tanto o jogador antigo quanto o novo são o mesmo, pula
    if (old_player && old_player == new_player) {
      if (MPG_TDML_Config.isDebugEnabled) {
        MPG_TDMLogger.Debug("moving inside player, ignored");
      }
      return;
    }

    // Por padrão o tipo de ação é movimento, os outros tipos são basicamente subtipos de movimento
    MPG_TDML_ActionType actionType = MPG_TDML_ActionType.MOVE;

    // Se não há jogador antigo e há um novo e se o item está no chão - TAKE
    if (oldLocType == InventoryLocationType.GROUND || (!old_player && new_player)) {
      actionType = MPG_TDML_ActionType.TAKE;
    }

    // Se não há novo jogador, e enquanto isso o item está no chão ou em cargo, então é DROP
    if (newLocType == InventoryLocationType.GROUND || (old_player && !new_player)) {
      actionType = MPG_TDML_ActionType.DROP;
    }

    if (!old_player && new_player) {
      if (new_player.IsPlayerLoaded()) {

        if (oldLocParent) {
          itemPos = oldLocPos;
          if (MPG_TDML_Config.isDebugEnabled) {
            MPG_TDMLogger.Debug("itemPos oldLocParent" + sep + itemPos.ToString());
          }
        } else {
          itemPos = GetPosition();
          if (MPG_TDML_Config.isDebugEnabled) {
            MPG_TDMLogger.Debug("itemPos on ground" + sep + itemPos.ToString());
          }
        }

        if (itemPos[0] > 0) {
          distance = vector.Distance(new_player.GetPosition(), itemPos);
        }

        if (MPG_TDML_Config.isDebugEnabled) {
          MPG_TDMLogger.Debug("distance between LOOT and PLAYER" + sep + distance.ToString());
        }
      }
    } else {
      if (oldLocPos[0] > 0 && newLocPos[0] > 0) {
        distance = vector.Distance(oldLocPos, newLocPos);
      }

      if (MPG_TDML_Config.isDebugEnabled) {
        MPG_TDMLogger.Debug("distance between containers" + sep + distance.ToString());
      }
    }

    if (MPG_TDML_Config.isDebugEnabled) {
      MPG_TDMLogger.Debug("actionType" + sep + MPG_TDMLogger.GetActionType(actionType));
      MPG_TDMLogger.Debug("===================================");
    }

    string oldPlace = oldLocText + sep + oldLocParentType + sep + oldLocParentName + sep + MPG_TDMLogger.GetParedPosition(oldLocPos) + sep + MPG_TDML_GetLocParentId(oldLoc) + sep + oldPlayerInfo;
    string newPlace = newLocText + sep + newLocParentType + sep + newLocParentName + sep + MPG_TDMLogger.GetParedPosition(newLocPos) + sep + MPG_TDML_GetLocParentId(newLoc) + sep + newPlayerInfo;

    MPG_TDMLogger.Log(MPG_TDMLogger.GetActionType(actionType) + sep + persistentID + sep + itemType + sep + itemName + sep + distance + sep + oldPlace + sep + newPlace);

    // Verificar se excede distância máxima e enviar alerta para Discord de loot
    if (distance >= MPG_TDML_Config.maxDistance && MPG_TDML_Config.lootDiscordUrl != string.Empty) {
      string pInfo = newPlayerInfo;
      if (!new_player) {
        pInfo = oldPlayerInfo;
      }
      string message = string.Format(MPG_TDML_Config.lootDiscordTextTpl,
                                     pInfo,
                                     itemName + " (" + itemType + ")",
                                     distance,
                                     oldLocPos[0].ToString() + ";" + oldLocPos[2].ToString(),
                                     newLocPos[0].ToString() + ";" + newLocPos[2].ToString(),
                                     persistentID,
                                     "CHEAT" + sep + persistentID + sep + itemType + sep + itemName + sep + distance + sep + oldPlace + sep + newPlace);

      MPG_TDMLogger.Log("CHEAT" + sep + persistentID + sep + itemType + sep + itemName + sep + distance + sep + oldPlace + sep + newPlace);

      MPG_TDMLogger.SendToDiscord(MPG_TDML_Config.lootDiscordUrl, MPG_TDML_Config.lootDiscordTitle, message);
    }
  }

  override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc) {
    super.EEItemLocationChanged(oldLoc, newLoc);

    MPG_TDMLogItemBase(oldLoc, newLoc);
  }

  //  override void EEInventoryIn(Man newParentMan, EntityAI diz, EntityAI newParent) {
  //    super.EEInventoryIn(newParentMan, diz, newParent);
  //    PlayerBase player = PlayerBase.Cast(newParentMan);
  //    if (newParentMan.ToString() != newParent.ToString() && player && player.IsPlayerLoaded()) {
  //      MPG_TDMLogger.Log("EEInventoryIn\t" + newParentMan + sep + diz + sep + newParent);
  //    }
  //  }
  //
  //    override void EEInventoryOut(Man oldParentMan, EntityAI diz, EntityAI newParent) {
  //      super.EEInventoryOut(oldParentMan, diz, newParent);
  //      MPG_TDMLogger.Log("EEInventoryOut\t" + oldParentMan + MPG_TDML_LOG_SEPARATOR + diz + MPG_TDML_LOG_SEPARATOR + newParent);
  //    }
};